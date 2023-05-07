/* Assembler code fragment for LC-2K */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int isDefinedLabel(char *);
int getLabelAddress(char *);
int isValidReg(char *);
int calculateOffset(char *);

typedef struct
{
	char name[7];
	int address;
} LABEL;

LABEL labelArr[MAXLINELENGTH] = {0};
int labelLength = 0;

int main(int argc, char *argv[])
{
	char *inFileString, *outFileString;
	FILE *inFilePtr, *outFilePtr;
	char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
		arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

	if (argc != 3)
	{
		printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
			   argv[0]);
		exit(1);
	}

	inFileString = argv[1];
	outFileString = argv[2];

	inFilePtr = fopen(inFileString, "r");
	if (inFilePtr == NULL)
	{
		printf("error in opening %s\n", inFileString);
		exit(1);
	}
	outFilePtr = fopen(outFileString, "w");
	if (outFilePtr == NULL)
	{
		printf("error in opening %s\n", outFileString);
		exit(1);
	}

	/* TODO: Phase-1 label calculation */
	int address = 0;
	while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2))
	{
		if (!strcmp(label, ""))
		{
			address++;
			continue;
		}
		if (isDefinedLabel(label))
		{
			printf("Duplicate definition of labels\n");
			exit(1);
		}
		strcpy(labelArr[labelLength].name, label);
		labelArr[labelLength++].address = address++;
	}

	/* this is how to rewind the file ptr so that you start reading from the
		 beginning of the file */
	rewind(inFilePtr);

	/* TODO: Phase-2 generate machine codes to outfile */
	int pc = 0;
	while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2))
	{
		int output, offset = 0;
		int noRegField = 0;

		if (!strcmp(opcode, "halt"))
		{
			noRegField = 1;
			output = 6 << 22;
		}
		else if (!strcmp(opcode, "noop"))
		{
			noRegField = 1;
			output = 7 << 22;
		}
		else if (!strcmp(opcode, ".fill"))
		{
			if (isNumber(arg0))
				output = atoi(arg0);
			else
				output = getLabelAddress(arg0);
			noRegField = 1;
		}

		if (!noRegField)
		{
			// check arg0, arg1 field (register fields)
			if (!isValidReg(arg0) || !isValidReg(arg1))
			{
				printf("Register Field Error\n");
				exit(1);
			}
			if (!strcmp(opcode, "add"))
			{
				// check destReg for R-type format
				if (!isValidReg(arg2))
				{
					printf("Register Field Error\n");
					exit(1);
				}
				output = 0 << 22;
				output |= atoi(arg0) << 19;
				output |= atoi(arg1) << 16;
				output |= atoi(arg2) << 0;
			}
			else if (!strcmp(opcode, "nor"))
			{
				// check destReg for R-type format
				if (!isValidReg(arg2))
				{
					printf("Register Field Error\n");
					exit(1);
				}
				output = 1 << 22;
				output |= atoi(arg0) << 19;
				output |= atoi(arg1) << 16;
				output |= atoi(arg2) << 0;
			}
			else if (!strcmp(opcode, "lw"))
			{
				offset = calculateOffset(arg2) & 0xFFFF;
				output = 2 << 22;
				output |= atoi(arg0) << 19;
				output |= atoi(arg1) << 16;
				output |= offset << 0;
			}
			else if (!strcmp(opcode, "sw"))
			{
				offset = calculateOffset(arg2) & 0xFFFF;
				output = 3 << 22;
				output |= atoi(arg0) << 19;
				output |= atoi(arg1) << 16;
				output |= offset << 0;
			}
			else if (!strcmp(opcode, "beq"))
			{
				offset = calculateOffset(arg2);
				if (!isNumber(arg2))
					offset -= pc + 1;
				offset = offset & 0xFFFF;
				output = 4 << 22;
				output |= atoi(arg0) << 19;
				output |= atoi(arg1) << 16;
				output |= offset << 0;
			}
			else if (!strcmp(opcode, "jalr"))
			{
				output = 5 << 22;
				output |= atoi(arg0) << 19;
				output |= atoi(arg1) << 16;
			}
			else
			{
				printf("Unrecognized Opcode\n");
				exit(1);
			}
		}
		fprintf(outFilePtr, "%d\n", output);
		// printf("(address %d) : %d\n", pc, output);
		pc++;
	}
	if (inFilePtr)
	{
		fclose(inFilePtr);
	}
	if (outFilePtr)
	{
		fclose(outFilePtr);
	}
	exit(0);
	return (0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
				 char *arg1, char *arg2)
{
	char line[MAXLINELENGTH];
	char *ptr = line;

	/* delete prior values */
	label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

	/* read the line from the assembly-language file */
	if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL)
	{
		/* reached end of file */
		return (0);
	}

	/* check for line too long (by looking for a \n) */
	if (strchr(line, '\n') == NULL)
	{
		/* line too long */
		printf("error: line too long\n");
		exit(1);
	}

	/* is there a label? */
	ptr = line;
	if (sscanf(ptr, "%[^\t\n\r ]", label))
	{
		/* successfully read label; advance pointer over the label */
		ptr += strlen(label);
	}

	/*
	 * Parse the rest of the line.  Would be nice to have real regular
	 * expressions, but scanf will suffice.
	 */
	sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%"
				"[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",
		   opcode, arg0, arg1, arg2);
	return (1);
}

int isNumber(char *string)
{
	/* return 1 if string is a number */
	int i;
	return ((sscanf(string, "%d", &i)) == 1);
}
int isDefinedLabel(char *string)
{
	for (int i = 0; i < labelLength; i++)
	{
		if (!strcmp(string, labelArr[i].name))
		{
			return 1;
		}
	}
	return 0;
}
int getLabelAddress(char *string)
{
	for (int i = 0; i < labelLength; i++)
	{
		if (!strcmp(string, labelArr[i].name))
		{
			return labelArr[i].address;
		}
	}
	return -1;
}

int isValidReg(char *string)
{
	int num;
	if (!isNumber(string))
		return 0;
	num = atoi(string);
	if (num < 0 || num > 7)
		return 0;
	return 1;
}

int calculateOffset(char *string)
{
	int offset;
	if (isNumber(string))
	{
		offset = atoi(string);
		if (offset > 32767 || offset < -32768)
		{
			printf("offsetFields don't fit in 16 bits\n");
			exit(1);
		}
		return offset;
	}
	else
	{
		if (!isDefinedLabel(string))
		{
			printf("Use of undefined labels\n");
			exit(1);
		}
		return getLabelAddress(string);
	}
}

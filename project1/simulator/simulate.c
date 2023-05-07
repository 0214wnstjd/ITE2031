/* LC-2K Instruction-level simulator */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8       /* number of machine registers */
#define MAXLINELENGTH 1000
typedef struct stateStruct
{
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);
int convertNum(int num);
void decode(int, int *, int *, int *, int *);

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    state.pc = 0;
    for (int i = 0; i < NUMREGS; i++)
    {
        state.reg[i] = 0;
    }

    if (argc != 2)
    {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL)
    {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
         state.numMemory++)
    {
        if (sscanf(line, "%d", state.mem + state.numMemory) != 1)
        {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

    /* TODO: */
    printState(&state);
    int lineCount = 0;
    while (1)
    {
        int opcode, arg0, arg1, arg2 = 0;
        decode(state.mem[state.pc], &opcode, &arg0, &arg1, &arg2);
        switch (opcode)
        {
        case 0:
            state.reg[arg2] = state.reg[arg0] + state.reg[arg1];
            break;
        case 1:
            state.reg[arg2] = ~(state.reg[arg0] | state.reg[arg1]);
            break;

        case 2:
            state.reg[arg1] = state.mem[state.reg[arg0] + convertNum(arg2)];
            break;
        case 3:
            state.mem[state.reg[arg0] + convertNum(arg2)] = state.reg[arg1];
            break;
        case 4:
            if (state.reg[arg0] == state.reg[arg1])
                state.pc += convertNum(arg2);
            break;
        case 5:
            state.reg[arg1] = state.pc + 1;
            state.pc = state.reg[arg0] - 1;
            break;
        case 6:
            printf("machine halted\n");
            printf("total of %d instruction executed\n", ++lineCount);
            printf("final state of machine:\n");
            state.pc++;
            printState(&state);
            return (0);
        default:
            break;
        }
        state.pc++;
        lineCount++;
        printState(&state);
    }

    fclose(filePtr);
    return (0);
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i = 0; i < statePtr->numMemory; i++)
    {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i = 0; i < NUMREGS; i++)
    {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}

int convertNum(int num)
{
    /* convert a 16-bit number into a 32-bit Linux integer */
    if (num & (1 << 15))
    {
        num -= (1 << 16);
    }
    return (num);
}

void decode(int line, int *opcode, int *arg0, int *arg1, int *arg2)
{
    *opcode = (line >> 22) & 0x7;
    *arg0 = (line >> 19) & 0x7;
    *arg1 = (line >> 16) & 0x7;
    *arg2 = line & 0xFFFF;
}
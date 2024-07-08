# How to run
**Assembler**    
  1. `cd assembler`
  2. `make`
  3. `./assembler spec.as spec.mc`

**Simulator**
  1. `cd simulator`
  2. `make`
  3. `./simulate spec.as > spec.out`

**Multiplication**
  1. `./assembler/assembler mult.as mult.mc`
  2. `./simulator/simulate mult.mc > mult.out`

# Specification

## Goal
**Assembler 구현**
- write a program to take an assembly-language program and translate it into machine language.    

**Simulator 구현**
- write a program that can simulate any legal LC-2K machine-code program.
  
**Multiplication 작성**
- write an assembly-language program to multiply two numbers.

## Environment
MacOS     
gcc

## Details

The LC-2K is a 32-bit RISC architecture which includes
- 8 x 32-bit registers: reg01 to reg7
- Each address is 32-bits and it stores a word
- 65536 words of memory (addressed by word index, not byte)    

```
R-type instructions (add, nor):    
    bits 24-22: opcode    
    bits 21-19: reg A   
    bits 18-16: reg B   
    bits 15-3:  unused (should all be 0)   
    bits 2-0:   destReg   
I-type instructions (lw, sw, beq):    
    bits 24-22: opcode    
    bits 21-19: reg A    
    bits 18-16: reg B    
    bits 15-0:  offsetField (a 16-bit value with a range of -32768 to 32767)     
J-type instructions (jalr):    
    bits 24-22: opcode    
    bits 21-19: reg A    
    bits 18-16: reg B    
    bits 15-0:  unused (should all be 0)    
O-type instructions (halt, noop):    
    bits 24-22: opcode    
    bits 21-0:  unused (should all be 0)    
```
```
-------------------------------------------------------------------------------
Assembly language 	Opcode in binary		Action
name for instruction	(bits 24-22)
-------------------------------------------------------------------------------
add (R-type format)	000 			Add contents of regA with
						contents of regB, store
						results in destReg.

nor (R-type format)	001			Nor contents of regA with
						contents of regB, store
						results in destReg.
						This is a bitwise nor;
						each bit is treated independently.

lw (I-type format)	010			Load regB from memory. Memory
						address is formed by adding
						offsetField with the contents of
						regA.

sw (I-type format)	011			Store regB into memory. Memory
						address is formed by adding
						offsetField with the contents of
						regA.

beq (I-type format)	100			If the contents of regA and
						regB are the same, then branch
						to the address PC+1+offsetField,
						where PC is the address of the
						beq instruction.

jalr (J-type format)	101			First store PC+1 into regB,
						where PC is the address of
						this jalr instruction. Then
						branch to the address contained
						in regA. Note that this implies
						if regA and regB refer to the same
						register, the net effect will
						be jumping to PC+1.
						

halt (O-type format)	110			Increment the PC (as with all
						instructions), then halt the
						machine (let the simulator
						notice that the machine
						halted).

noop (O-type format)	111			Do nothing.
```

R-type instructions (add, nor) instructions require 3 fields: field0 is regA, field1 is regB, and field2 is destReg.    

I-type instructions (lw, sw, beq) require 3 fields: field0 is regA, field1 is regB, and field2 is either a numeric value for offsetField or a symbolic address. Numeric offsetFields can be positive or negative; symbolic addresses are discussed below.    

J-type instructions (jalr) require 2 fields: field0 is regA, and field1 is regB. O-type instructions (noop, halt) require no fields.   

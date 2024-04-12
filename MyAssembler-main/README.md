# Project README

## Hardware Description

The computer in this project consists of a CPU, registers, and RAM. Some of the RAM is used as a stack.

- Processor: 9 registers (r0 to r7, PSW), each 14 bits
- Program Counter (PC): Holds the memory address of the current command
- RAM Size: 4096 cells, each 14 bits (0-4095)
- Data Types: Z type numbers (positive, negative, and 0), ASCII characters

## Machine Code Structure

- Machine Code: Encrypted BASE 4 (0 = *, 1 = #, 2 = %, 3 = !)
- Structure: 1 to 5 memory words per code
- Types: A (Absolute), R (Relocatable), E (External)

## Instruction Set

Commands are divided into 3 groups:

1. Group 1 (2 Operands):
   - mov, cmp, add, sub, lea

2. Group 2 (1 Operand):
   - not, clr, inc, dec, jmp, bne, red, prn, jsr

3. Group 3 (0 Operands):
   - rts, hlt

## Macros

- Macro Format: m_mcr
- Example Macro:
```c
mcr m_mcr
inc r2
mov A,r1
endmcr
```

## File Structure

- Line Length: Up to 80 characters
- Sentence Types: Empty, Note, Label, Command, Define
- Labeled Sentences: .data, .string, .entry, .extern

## Variables

- Label Format: Starts with alphabetic letter, max 31 characters
- Data Types: .string, .data, command

## Input/Output

- Input: Text files (source code), file names passed as ARGV
- Output Files: .am (source code after macro translation), .ob (final machine code), .ext (external labels), .ent (entry labels)

## Object File Format

- Header: Total length of instruction segment, total length of data segment
- Content: Address (base 10) | Encrypted BASE 4 (7 digits)

## Entry and Externals Files

- Entry File Format: Name of Label | Address of Label Initiation
- Externals File Format: Name of Label | Address of Usage in Code

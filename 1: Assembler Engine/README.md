# Assembler Engine

This folder contains a fully-functional assembler engine for the LC-3b instruction set architecture (ISA).  
It takes LC-3b assembly source files and produces machine-code object files, ready for simulation.

## Features
- Parses LC-3b assembly pseudo-ops (.ORIG, .END, .FILL) and standard instructions. :contentReference[oaicite:1]{index=1}  
- Two-pass symbol table approach: first pass binds labels, second pass emits opcodes.  
- Error detection: invalid .ORIG addresses, mis-aligned memory access, unsupported pseudo-ops. :contentReference[oaicite:2]{index=2}  
- Output in hex-formatted object file compatible with LC-3b simulator.

## Usage
```bash
./assembler input.asm output.obj

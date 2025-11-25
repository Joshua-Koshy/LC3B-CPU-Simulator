


# Added Interrupts, Exceptions functionality + Designed new hardware
**Filename:** `lab4/README.md`

```markdown
# LC-3b Extended Microarchitecture Simulator

This component introduces support for interrupts, exceptions, and prepares the architecture for virtual memory extension.  
It builds upon the cycle-level simulator and adds fault-handling and protection support.

## Features
- Implements exception detection and handling mechanisms for illegal operations, mis-aligned memory access, and privilege violations.  
- Simulates interrupt servicing routines with context save/restore semantics.  
- Sets the stage for full virtual memory integration (completed in the next module).

## Usage
```bash
./lc3b_ext_sim program.obj

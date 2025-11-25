

# Added Virtual Memory functionality + Designed new hardware
**Filename:** `lab5/README.md`

```markdown
# LC-3b Virtual Memory & Protection Simulator

This module completes the architecture by adding full virtual-memory support and user/supervisor mode protection.  
It simulates how the LC-3b transitions from simple physical memory to a paged virtual-memory system.

## Features
- Implements virtual-to-physical address translation using page tables. :contentReference[oaicite:5]{index=5}  
- Divides address space into user and system zones; enforces access-mode protection. :contentReference[oaicite:6]{index=6}  
- Handles page faults, invalid access, and translation exceptions with detailed logging.  
- Maintains compatibility with earlier instruction- and cycle-level simulators for full system integration.

## Usage
```bash
./lc3b_vm_sim program.obj pageTable.pt

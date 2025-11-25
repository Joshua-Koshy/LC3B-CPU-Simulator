


# Cycle-Level Simulator  
**Filename:** `lab3/README.md`

```markdown
# LC-3b Cycle-Level Simulator

This module advances the simulation from single-instruction granularity to cycle-level detail.  
It models the internal pipeline of the LC-3b, capturing how instructions propagate through the datapath stage-by-stage.

## Features
- Implements cycle-by-cycle simulation of instruction execution (fetch, decode, execute, memory, write-back).  
- Tracks timing of each instruction, hazards, stalls, and resource conflicts.  
- Provides detailed logs of pipeline state per cycle, enabling performance analysis.

## Usage
```bash
./lc3b_cycle_sim program.obj

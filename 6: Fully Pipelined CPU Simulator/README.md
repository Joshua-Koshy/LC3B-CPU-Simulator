# Final Pipelined Simulator

This module implements a fully pipelined execution engine for the LC-3b architecture.  
It extends the single-cycle and multi-cycle simulators with a realistic pipeline model that supports overlapping instruction execution, hazard detection, and dynamic stall behavior.

## Overview

The pipeline engine models the canonical 5-stage microarchitecture:

1. **IF** – Instruction Fetch  
2. **ID** – Decode & Register Read  
3. **EX** – ALU and Address Calculation  
4. **MEM** – Memory Access  
5. **WB** – Write-Back  

Each instruction moves through the pipeline one stage per cycle, subject to stalls and hazard constraints.

## Features

### 🔹 Pipeline Simulation  
- Accurate stage-by-stage flow of instructions  
- Models register bypassing/forwarding when applicable  
- Configurable memory latency for load/use penalties  
- Full visibility into pipeline registers and stage progression  

### 🔹 Hazard Detection  
- Detects data hazards (RAW, WAR where applicable)  
- Inserts pipeline stalls for unresolved dependencies  
- Supports forwarding paths to minimize bubbles  

### 🔹 Control-Flow Handling  
- Branch flush logic  
- PC redirection and pipeline invalidation  
- Cycle-accurate modeling of control penalties  

### 🔹 Pipeline Diagnostics  
- Cycle-by-cycle dump of pipeline registers  
- Human-readable execution timeline  
- Summary of:
  - total cycles  
  - CPI (cycles per instruction)  
  - stall counts  
  - branch penalties  

## Usage

```bash
./lc3b_pipeline_sim program.obj

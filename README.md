# CPU-Simulator

This repository provides a complete, modular implementation of the LC-3b computer architecture—from source-level assembly to a fully pipelined CPU core. **This includes original hardware and microcode designed by Joshua Koshy, adding new functionality to the original microarchitecture.** Each module represents a self-contained component of the architecture and can be used independently or as part of a larger toolchain.

The overall system models how real processors evolve from basic instruction interpreters to multi-stage pipelines with protection, exceptions, and virtual memory.

---

## Module Index

### **0 — Environment & Utilities**
Supporting scripts, configuration, and baseline utilities used throughout the simulation suite.

### **1 — Assembler Engine**
A full LC-3b assembler that converts symbolic assembly into machine-level object code:
- Two-pass label resolution
- Instruction + pseudo-op encoding
- Error detection and range checking
- Output compatible with the LC-3b simulators in this repo

### **2 — Instruction-Level CPU Simulator**
A direct architectural simulator for LC-3b:
- Executes object code instruction by instruction
- Models architectural state (register file, PC, condition codes)
- Accurate state transitions and side effects
- Useful for debugging assembly and validating CPU behavior

### **3 — Cycle-Level CPU Simulator**
A detailed timing simulator that advances the machine one cycle at a time:
- Models datapath components at micro-operation granularity
- Exposes register transfers, control signals, and state evolution
- Enables fine-grained debugging and performance analysis

### **4 — Cycle-Level Simulator with Interrupts & Exceptions**
An extended microarchitecture simulator featuring:
- Supervisor/user mode transitions
- Exception vectors, trap handling, and internal faults
- Precise state save/restore and privilege-based protection
- Full control-flow redirection based on fault/interrupt events

### **5 — Virtual Memory & Address Translation Engine**
A complete virtual memory subsystem integrated into the cycle-accurate core:
- Page-table–based virtual→physical address translation
- Permission checking (R/W/X, privilege)
- Page faults and exception routing
- Compatible with both user and supervisor mode execution

### **6 — Fully Pipelined CPU Simulator**
A full 5-stage pipelined LC-3b architecture:
- IF → ID → EX → MEM → WB pipeline
- Hazard detection (RAW, control hazards)
- Forwarding and stall insertion
- Branch penalty modeling and pipeline flushing
- Cycle-accurate pipeline register tracking
- Pipeline performance statistics (CPI, stalls, penalties)

---

## Architectural Themes Demonstrated

This suite demonstrates layered CPU design principles, including:

- Assembler construction and symbolic translation
- ISA-level architectural state modeling
- Microarchitectural register-transfer-level execution
- Exception and interrupt control paths
- Virtual memory and protection mechanisms
- Pipeline parallelism and hazard resolution
- Performance analysis across architectural levels

Each module builds on prior components, forming a complete vertical slice of a working CPU toolchain.

---

## Usage

Each numbered directory (`0/`, `1/`, `2/`, … `6/`) includes:
- A dedicated `README.md` describing the component  
- Source code for that module  
- Example programs and testing utilities  

Most simulators run similarly:

```bash
./sim program.obj

/*
    Name 1: Your full name
    UTEID 1: Your UT EID
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         isaprogram   LC-3b machine language program file    */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void eval_micro_sequencer();
void cycle_memory();
void eval_bus_drivers();
void drive_bus();
void latch_datapath_values();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Definition of the control store layout.                     */
/***************************************************************/
#define CONTROL_STORE_ROWS 64
#define INITIAL_STATE_NUMBER 18

/***************************************************************/
/* Definition of bit order in control store word.              */
/***************************************************************/
// IRD			 Cond		 J						 LD.MAR	 LD.MDR	 LD.IR	 LD.BEN	 LD.REG	 LD.CC	 LD.PC	 GatePC	 GateMDR	 GateALU	 GateMARMUX	 GateSHF	 PCMUX		 DRMUX		 SR1MUX		 ADDR1MUX	 ADDR2MUX		 MARMUX	 ALUK		 MIO.EN	 R.W	 DATA.SIZE	 LSHF1	LD.PSR	LD.Priv	LD.Vector	LD.SavedUSP	LD.SavedSSP	LD.EXCV	Gate.PSR	Gate.SP	Gate.Vector	Gate.PCminus2	SPMUX		VectorMux

enum CS_BITS {                                                  
    IRD,
    COND1, COND0,
    J5, J4, J3, J2, J1, J0,
    LD_MAR,
    LD_MDR,
    LD_IR,
    LD_BEN,
    LD_REG,
    LD_CC,
    LD_PC,
    GATE_PC,
    GATE_MDR,
    GATE_ALU,
    GATE_MARMUX,
    GATE_SHF,
    PCMUX1, PCMUX0,
    DRMUX,
    SR1MUX,
    ADDR1MUX,
    ADDR2MUX1, ADDR2MUX0,
    MARMUX,
    ALUK1, ALUK0,
    MIO_EN,
    R_W,
    DATA_SIZE,
    LSHF1,
    CONTROL_STORE_BITS
} CS_BITS;

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD(int *x)           { return(x[IRD]); }
int GetCOND(int *x)          { return((x[COND1] << 1) + x[COND0]); }
int GetJ(int *x)             { return((x[J5] << 5) + (x[J4] << 4) +
				      (x[J3] << 3) + (x[J2] << 2) +
				      (x[J1] << 1) + x[J0]); }
int GetLD_MAR(int *x)        { return(x[LD_MAR]); }
int GetLD_MDR(int *x)        { return(x[LD_MDR]); }
int GetLD_IR(int *x)         { return(x[LD_IR]); }
int GetLD_BEN(int *x)        { return(x[LD_BEN]); }
int GetLD_REG(int *x)        { return(x[LD_REG]); }
int GetLD_CC(int *x)         { return(x[LD_CC]); }
int GetLD_PC(int *x)         { return(x[LD_PC]); }
int GetGATE_PC(int *x)       { return(x[GATE_PC]); }
int GetGATE_MDR(int *x)      { return(x[GATE_MDR]); }
int GetGATE_ALU(int *x)      { return(x[GATE_ALU]); }
int GetGATE_MARMUX(int *x)   { return(x[GATE_MARMUX]); }
int GetGATE_SHF(int *x)      { return(x[GATE_SHF]); }
int GetPCMUX(int *x)         { return((x[PCMUX1] << 1) + x[PCMUX0]); }
int GetDRMUX(int *x)         { return(x[DRMUX]); }
int GetSR1MUX(int *x)        { return(x[SR1MUX]); }
int GetADDR1MUX(int *x)      { return(x[ADDR1MUX]); }
int GetADDR2MUX(int *x)      { return((x[ADDR2MUX1] << 1) + x[ADDR2MUX0]); }
int GetMARMUX(int *x)        { return(x[MARMUX]); }
int GetALUK(int *x)          { return((x[ALUK1] << 1) + x[ALUK0]); }
int GetMIO_EN(int *x)        { return(x[MIO_EN]); }
int GetR_W(int *x)           { return(x[R_W]); }
int GetDATA_SIZE(int *x)     { return(x[DATA_SIZE]); } 
int GetLSHF1(int *x)         { return(x[LSHF1]); }

/***************************************************************/
/* The control store rom.                                      */
/***************************************************************/
int CONTROL_STORE[CONTROL_STORE_ROWS][CONTROL_STORE_BITS];

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
   There are two write enable signals, one for each byte. WE0 is used for 
   the least significant byte of a word. WE1 is used for the most significant 
   byte of a word. */

#define WORDS_IN_MEM    0x08000 
#define MEM_CYCLES      5
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */
int BUS;	/* value of the bus */

typedef struct System_Latches_Struct{

int PC,		/* program counter */
    MDR,	/* memory data register */
    MAR,	/* memory address register */
    IR,		/* instruction register */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P,		/* p condition bit */
    BEN;        /* ben register */

int READY;	/* ready bit */
  /* The ready bit is also latched as you dont want the memory system to assert it 
     at a bad point in the cycle*/

int REGS[LC_3b_REGS]; /* register file. */

int MICROINSTRUCTION[CONTROL_STORE_BITS]; /* The microintruction */

int STATE_NUMBER; /* Current State Number - Provided for debugging */ 
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int CYCLE_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands.                   */
/*                                                             */
/***************************************************************/
void help() {                                                    
    printf("----------------LC-3bSIM Help-------------------------\n");
    printf("go               -  run program to completion       \n");
    printf("run n            -  execute program for n cycles    \n");
    printf("mdump low high   -  dump memory from low to high    \n");
    printf("rdump            -  dump the register & bus values  \n");
    printf("?                -  display this help menu          \n");
    printf("quit             -  exit the program                \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  eval_micro_sequencer();   
  cycle_memory();
  eval_bus_drivers();
  drive_bus();
  latch_datapath_values();

  CURRENT_LATCHES = NEXT_LATCHES;

  CYCLE_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles.                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
    int i;

    if (RUN_BIT == FALSE) {
	printf("Can't simulate, Simulator is halted\n\n");
	return;
    }

    printf("Simulating for %d cycles...\n\n", num_cycles);
    for (i = 0; i < num_cycles; i++) {
	if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
	}
	cycle();
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed.                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
    if (RUN_BIT == FALSE) {
	printf("Can't simulate, Simulator is halted\n\n");
	return;
    }

    printf("Simulating...\n\n");
    while (CURRENT_LATCHES.PC != 0x0000)
	cycle();
    RUN_BIT = FALSE;
    printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
    int address; /* this is a byte address */

    printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
    int k; 

    printf("\nCurrent register/bus values :\n");
    printf("-------------------------------------\n");
    printf("Cycle Count  : %d\n", CYCLE_COUNT);
    printf("PC           : 0x%.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%.4x\n", CURRENT_LATCHES.IR);
    printf("STATE_NUMBER : 0x%.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%.4x\n", BUS);
    printf("MDR          : 0x%.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%.4x\n", CURRENT_LATCHES.MAR);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%.4x\n", CURRENT_LATCHES.IR);
    fprintf(dumpsim_file, "STATE_NUMBER : 0x%.4x\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%.4x\n", CURRENT_LATCHES.MAR);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
    fprintf(dumpsim_file, "\n");
    fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
    char buffer[20];
    int start, stop, cycles;

    printf("LC-3b-SIM> ");

    scanf("%s", buffer);
    printf("\n");

    switch(buffer[0]) {
    case 'G':
    case 'g':
	go();
	break;

    case 'M':
    case 'm':
	scanf("%i %i", &start, &stop);
	mdump(dumpsim_file, start, stop);
	break;

    case '?':
	help();
	break;
    case 'Q':
    case 'q':
	printf("Bye.\n");
	exit(0);

    case 'R':
    case 'r':
	if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
	else {
	    scanf("%d", &cycles);
	    run(cycles);
	}
	break;

    default:
	printf("Invalid Command\n");
	break;
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_control_store                              */
/*                                                             */
/* Purpose   : Load microprogram into control store ROM        */ 
/*                                                             */
/***************************************************************/
void init_control_store(char *ucode_filename) {                 
    FILE *ucode;
    int i, j, index;
    char line[200];

    printf("Loading Control Store from file: %s\n", ucode_filename);

    /* Open the micro-code file. */
    if ((ucode = fopen(ucode_filename, "r")) == NULL) {
	printf("Error: Can't open micro-code file %s\n", ucode_filename);
	exit(-1);
    }

    /* Read a line for each row in the control store. */
    for(i = 0; i < CONTROL_STORE_ROWS; i++) {
	if (fscanf(ucode, "%[^\n]\n", line) == EOF) {
	    printf("Error: Too few lines (%d) in micro-code file: %s\n",
		   i, ucode_filename);
	    exit(-1);
	}

	/* Put in bits one at a time. */
	index = 0;

	for (j = 0; j < CONTROL_STORE_BITS; j++) {
	    /* Needs to find enough bits in line. */
	    if (line[index] == '\0') {
		printf("Error: Too few control bits in micro-code file: %s\nLine: %d\n",
		       ucode_filename, i);
		exit(-1);
	    }
	    if (line[index] != '0' && line[index] != '1') {
		printf("Error: Unknown value in micro-code file: %s\nLine: %d, Bit: %d\n",
		       ucode_filename, i, j);
		exit(-1);
	    }

	    /* Set the bit in the Control Store. */
	    CONTROL_STORE[i][j] = (line[index] == '0') ? 0:1;
	    index++;
	}

	/* Warn about extra bits in line. */
	if (line[index] != '\0')
	    printf("Warning: Extra bit(s) in control store file %s. Line: %d\n",
		   ucode_filename, i);
    }
    printf("\n");
}

/************************************************************/
/*                                                          */
/* Procedure : init_memory                                  */
/*                                                          */
/* Purpose   : Zero out the memory array                    */
/*                                                          */
/************************************************************/
void init_memory() {                                           
    int i;

    for (i=0; i < WORDS_IN_MEM; i++) {
	MEMORY[i][0] = 0;
	MEMORY[i][1] = 0;
    }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {                   
    FILE * prog;
    int ii, word, program_base;

    /* Open program file. */
    prog = fopen(program_filename, "r");
    if (prog == NULL) {
	printf("Error: Can't open program file %s\n", program_filename);
	exit(-1);
    }

    /* Read in the program. */
    if (fscanf(prog, "%x\n", &word) != EOF)
	program_base = word >> 1;
    else {
	printf("Error: Program file is empty\n");
	exit(-1);
    }

    ii = 0;
    while (fscanf(prog, "%x\n", &word) != EOF) {
	/* Make sure it fits. */
	if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
		   program_filename, ii);
	    exit(-1);
	}

	/* Write the word to memory array. */
	MEMORY[program_base + ii][0] = word & 0x00FF;
	MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
	ii++;
    }

    if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

    printf("Read %d words from program into memory.\n\n", ii);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */ 
/*             and set up initial state of the machine.        */
/*                                                             */
/***************************************************************/
void initialize(char *ucode_filename, char *program_filename, int num_prog_files) { 
    int i;
    init_control_store(ucode_filename);

    init_memory();
    for ( i = 0; i < num_prog_files; i++ ) {
	load_program(program_filename);
	while(*program_filename++ != '\0');
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);

    NEXT_LATCHES = CURRENT_LATCHES;

    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
    FILE * dumpsim_file;

    /* Error Checking */
    if (argc < 3) {
	printf("Error: usage: %s <micro_code_file> <program_file_1> <program_file_2> ...\n",
	       argv[0]);
	exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv[1], argv[2], argc - 2);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
	printf("Error: Can't open dumpsim file\n");
	exit(-1);
    }

    while (1)
	get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   CONTROL_STORE
   MEMORY
   BUS

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */
/***************************************************************/


void eval_micro_sequencer() {

    int nextState = 0;
    if(GetIRD(CURRENT_LATCHES.MICROINSTRUCTION) == 1) { //if IRD = 1 it is state 32 
        nextState = ((CURRENT_LATCHES.IR & 0xF000) >> 12) & 0x3F; //0,0,IR[15:12]
    }
    else { //We send in J bits 
    //j[5]
    nextState |=  (CURRENT_LATCHES.MICROINSTRUCTION[J5] << 5); 
    //j[4]
    nextState |=  (CURRENT_LATCHES.MICROINSTRUCTION[J4] << 4); 
    //j[3]
    nextState |=  (CURRENT_LATCHES.MICROINSTRUCTION[J3] << 3);

    //ACTUAL CALCULATIONS NOW
    //j[2]       //OR GATE                             //AND GATE
    nextState |= ((CURRENT_LATCHES.MICROINSTRUCTION[J2] | (CURRENT_LATCHES.MICROINSTRUCTION[COND1] & ~CURRENT_LATCHES.MICROINSTRUCTION[COND0] & CURRENT_LATCHES.BEN)) << 2);

    //j[1]       //OR GATE                             //AND GATE
    nextState |= ((CURRENT_LATCHES.MICROINSTRUCTION[J1] | (~CURRENT_LATCHES.MICROINSTRUCTION[COND1] & CURRENT_LATCHES.MICROINSTRUCTION[COND0] & CURRENT_LATCHES.READY)) << 1);

    //j[1]       //OR GATE                             //AND GATE                                                                                       IR[11] So mask to get 11th bit
    nextState |= (CURRENT_LATCHES.MICROINSTRUCTION[J0] | (CURRENT_LATCHES.MICROINSTRUCTION[COND1] & CURRENT_LATCHES.MICROINSTRUCTION[COND0] & ((CURRENT_LATCHES.IR & 0x800) >> 11)));
  
    //CONtrol STORE [roW][Collumns]
                //[microstates][control signals]


    //THIS CODE PICKS THE STATE I, and then Goes through every control signal of state I

    //CONTROL_STORE_BITS is the number of signals

    //
    }
        for (int i = 0; i < CONTROL_STORE_BITS; i++){
        NEXT_LATCHES.MICROINSTRUCTION[i] = CONTROL_STORE[nextState][i];   
    }
    NEXT_LATCHES.STATE_NUMBER = nextState; //ASK TA IF I NEED TO UPDATE MORE THAN THIS
  /* 
   * Evaluate the address of the next state according to the 
   * micro sequencer logic. Latch the next microinstruction.
   */

}

int cycleCount = 0; 
int cycleCount2 = 0;

 
  /* 
   * This function emulates memory and the WE logic. 
   * Keep track of which cycle of MEMEN we are dealing with.  
   * If fourth, we need to latch Ready bit at the end of 
   * cycle to prepare microsequencer for the fifth cycle.  
   */
     //if memory is enabled then start cycle memory
void cycle_memory() {
    printf("CycleCount %d.\n", CURRENT_LATCHES.STATE_NUMBER);

    /* Check if memory is enabled */
    if (CURRENT_LATCHES.MICROINSTRUCTION[MIO_EN]) {
        /* Increment cycle counters */
        cycleCount++;
        cycleCount2++;

        /* Default READY is low */
        NEXT_LATCHES.READY = 0;

        /* Calculate memory index once */
        int mem_index = CURRENT_LATCHES.MAR >> 1;

        /* On the 4th cycle, perform memory operation */
        if (cycleCount == 4) {
            int is_read = (CURRENT_LATCHES.MICROINSTRUCTION[R_W] == 0);
            int is_byte = (CURRENT_LATCHES.MICROINSTRUCTION[DATA_SIZE] == 0);

            if (is_read) {
                /* Read from memory */
                int lo = MEMORY[mem_index][0] & 0xFF;
                int hi = MEMORY[mem_index][1] & 0xFF;
                NEXT_LATCHES.MDR = (hi << 8) | lo;
            } else {
                /* Write to memory */
                int mar_even = (CURRENT_LATCHES.MAR & 1) == 0;
                if (is_byte) {
                    MEMORY[mem_index][mar_even ? 0 : 1] = mar_even 
                        ? (CURRENT_LATCHES.MDR & 0xFF) 
                        : ((CURRENT_LATCHES.MDR >> 8) & 0xFF);
                } else {
                    MEMORY[mem_index][0] = CURRENT_LATCHES.MDR & 0xFF;
                    MEMORY[mem_index][1] = (CURRENT_LATCHES.MDR >> 8) & 0xFF;
                }
            }

            /* Set READY after memory access */
            NEXT_LATCHES.READY = 1;
        }

        /* Reset counter after 5th cycle */
        if (cycleCount == 5) cycleCount = 0;
    } else {
        /* If MIO_EN is not set, clear counter and READY */
        cycleCount = 0;
        NEXT_LATCHES.READY = 0;
    }
}


int www = 0;
int Gate_MARMUX = 0;
int Gate_PC     = 0;
int Gate_ALU    = 0;
int Gate_SHF    = 0;
int Gate_MDR    = 0;
int adder       = 0;   
int sign_extend(int x, int num_bits) {
    if (x & (1 << (num_bits - 1))) {
        x |= ~((1 << num_bits) - 1);
    }
    return x;
}
 /* ---------- Rewritten (semantically identical) ---------- */

void eval_bus_drivers() {
    /* Compute all candidate values for the bus. Must preserve exact semantics. */
    int *u = CURRENT_LATCHES.MICROINSTRUCTION;

    /* --- compute addr adder (ADDR1MUX + ADDR2MUX + optional LSHF1) --- */
    int addr1;
    if (GetADDR1MUX(u))
        addr1 = CURRENT_LATCHES.REGS[(CURRENT_LATCHES.IR >> 6) & 0x7];
    else
        addr1 = CURRENT_LATCHES.PC;

    int disp;
    switch (GetADDR2MUX(u)) {
        case 0:  disp = 0; break;
        case 1:  disp = sign_extend(CURRENT_LATCHES.IR & 0x003F, 6);  break;
        case 2:  disp = sign_extend(CURRENT_LATCHES.IR & 0x01FF, 9);  break;
        default: disp = sign_extend(CURRENT_LATCHES.IR & 0x07FF, 11); break;
    }

    if (GetLSHF1(u)) disp <<= 1;

    /* store into the original global 'adder' so other code can use it unchanged */
    adder = Low16bits(addr1 + disp);

    /* --- MARMUX: either zero-extended IR[7:0] << 1 or computed adder --- */
    if (GetMARMUX(u) == 0) {
        Gate_MARMUX = Low16bits((CURRENT_LATCHES.IR & 0x00FF) << 1);
    } else {
        Gate_MARMUX = Low16bits(adder);
    }

    /* --- PC gate --- */
    Gate_PC = Low16bits(CURRENT_LATCHES.PC);

    /* --- ALU path --- */
    int sr1_idx = GetSR1MUX(u) ? ((CURRENT_LATCHES.IR >> 6) & 0x7)
                              : ((CURRENT_LATCHES.IR >> 9) & 0x7);
    int sr1 = CURRENT_LATCHES.REGS[sr1_idx];

    int sr2;
    if (CURRENT_LATCHES.IR & 0x20) { /* imm5 */
        sr2 = sign_extend(CURRENT_LATCHES.IR & 0x1F, 5);
    } else {
        sr2 = CURRENT_LATCHES.REGS[CURRENT_LATCHES.IR & 0x7];
    }

    switch (GetALUK(u)) {
        case 0: Gate_ALU = Low16bits(sr1 + sr2); break;
        case 1: Gate_ALU = Low16bits(sr1 & sr2); break;
        case 2: Gate_ALU = Low16bits(sr1 ^ sr2); break;
        case 3: Gate_ALU = Low16bits(sr1);        break;
        default: Gate_ALU = 0;                    break;
    }

    /* --- SHF path --- */
    {
        int shamt = CURRENT_LATCHES.IR & 0xF;
        int shft_type = (CURRENT_LATCHES.IR >> 4) & 0x3;
        int v = sr1; /* value to shift */

        if (shft_type == 0) { /* LSHF */
            Gate_SHF = Low16bits(v << shamt);
        } else if (shft_type == 1) { /* RSHFL logical */
            Gate_SHF = Low16bits(((unsigned)v) >> shamt);
        } else { /* RSHFA arithmetic */
            int sign = (v & 0x8000) ? 0xFFFF : 0x0000;
            Gate_SHF = Low16bits((v >> shamt) | (sign << (16 - shamt)));
        }
    }

    /* --- MDR path (IMPORTANT: byte path ZERO-extends, word path passes through) --- */
    if (GetDATA_SIZE(u) == 0) { /* byte */
        if ((CURRENT_LATCHES.MAR & 0x1) == 0)
            Gate_MDR = Low16bits(CURRENT_LATCHES.MDR & 0x00FF);           /* zero-extend */
        else
            Gate_MDR = Low16bits((CURRENT_LATCHES.MDR >> 8) & 0x00FF);    /* zero-extend */
    } else { /* word */
        Gate_MDR = Low16bits(CURRENT_LATCHES.MDR);
    }
}

void drive_bus() {
    int *u = CURRENT_LATCHES.MICROINSTRUCTION;

    /* Count active gates exactly as original to detect contention */
    int active = GetGATE_PC(u) + GetGATE_ALU(u) +
                 GetGATE_SHF(u) + GetGATE_MDR(u) +
                 GetGATE_MARMUX(u);

    if (active != 1) { /* contention or none */
        BUS = 0;
        return;
    }

    /* follow the same priority/mapping as original implementation */
    if (GetGATE_PC(u))          BUS = Gate_PC;
    else if (GetGATE_MDR(u))    BUS = Gate_MDR;
    else if (GetGATE_ALU(u))    BUS = Gate_ALU;
    else if (GetGATE_MARMUX(u)) BUS = Gate_MARMUX;
    else if (GetGATE_SHF(u))    BUS = Gate_SHF;

    BUS = Low16bits(BUS);
}

void latch_datapath_values() {
    int *u = CURRENT_LATCHES.MICROINSTRUCTION;

    /* LD.MAR */
    if (GetLD_MAR(u)) {
        NEXT_LATCHES.MAR = Low16bits(BUS);
    }

    /* LD.MDR */
    if (GetLD_MDR(u)) {
        /* If memory I/O path active, and a read completed (READY), use memory contents */
        if (GetMIO_EN(u) && !GetR_W(u) && CURRENT_LATCHES.READY) {
            int A = (CURRENT_LATCHES.MAR >> 1) & 0x3FFF;
            int word = ((MEMORY[A][1] & 0xFF) << 8) | (MEMORY[A][0] & 0xFF);
            NEXT_LATCHES.MDR = Low16bits(word);
        } else {
            /* From bus: word or byte (byte replicated into both halves as zero-extended) */
            if (GetDATA_SIZE(u)) { /* word */
                NEXT_LATCHES.MDR = Low16bits(BUS);
            } else { /* byte */
                int b = BUS & 0x00FF;
                NEXT_LATCHES.MDR = Low16bits(b | (b << 8));
            }
        }
    }

    /* LD.IR */
    if (GetLD_IR(u)) {
        NEXT_LATCHES.IR = Low16bits(BUS);
    }

    /* LD.BEN */
    if (GetLD_BEN(u)) {
        int ir11 = (CURRENT_LATCHES.IR & 0x0800) >> 11;
        int ir10 = (CURRENT_LATCHES.IR & 0x0400) >> 10;
        int ir9  = (CURRENT_LATCHES.IR & 0x0200) >> 9;
        int n = CURRENT_LATCHES.N;
        int z = CURRENT_LATCHES.Z;
        int p = CURRENT_LATCHES.P;

        NEXT_LATCHES.BEN = ((ir11 & n) | (ir10 & z) | (ir9 & p));
    }

    /* LD.REG (and possibly LD.CC when LD_REG true) */
    if (GetLD_REG(u)) {
        int dr;
        if (GetDRMUX(u) == 0) dr = (CURRENT_LATCHES.IR & 0x0E00) >> 9;
        else dr = 7;

        int val = BUS;
        /* If byte-sized and the source was MDR, sign-extend the low byte into the register */
        if (GetDATA_SIZE(u) == 0 && GetGATE_MDR(u)) {
            val = sign_extend(BUS & 0x00FF, 8);
        }
        NEXT_LATCHES.REGS[dr] = Low16bits(val);

        /* Update CCs if requested when writing the reg */
        if (GetLD_CC(u)) {
            if (val & 0x8000) { NEXT_LATCHES.N = 1; NEXT_LATCHES.Z = 0; NEXT_LATCHES.P = 0; }
            else if ((val & 0xFFFF) == 0) { NEXT_LATCHES.N = 0; NEXT_LATCHES.Z = 1; NEXT_LATCHES.P = 0; }
            else { NEXT_LATCHES.N = 0; NEXT_LATCHES.Z = 0; NEXT_LATCHES.P = 1; }
        }
    }
    else if (GetLD_CC(u)) {
        /* Update CCs from bus (when not writing a register this cycle) */
        int v = Low16bits(BUS);
        if (v & 0x8000) { NEXT_LATCHES.N = 1; NEXT_LATCHES.Z = 0; NEXT_LATCHES.P = 0; }
        else if (v == 0) { NEXT_LATCHES.N = 0; NEXT_LATCHES.Z = 1; NEXT_LATCHES.P = 0; }
        else { NEXT_LATCHES.N = 0; NEXT_LATCHES.Z = 0; NEXT_LATCHES.P = 1; }
    }

    /* LD.PC */
    if (GetLD_PC(u)) {
        int sel = GetPCMUX(u); /* 0,1,2 mapping preserved */
        int val;
        if (sel == 0) val = Low16bits(CURRENT_LATCHES.PC + 2);
        else if (sel == 1) val = Low16bits(BUS);
        else val = Low16bits(adder); /* use the 'adder' computed in eval_bus_drivers */
        NEXT_LATCHES.PC = val;
    }
}

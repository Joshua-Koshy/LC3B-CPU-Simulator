/*
    Name 1: Your Name
    UTEID 1: Your UTEID
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Simulator                                           */
/*                                                             */
/*   EE 460N - Lab 5                                           */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files:  ucode        Microprogram file                      */
/*         pagetable    page table in LC-3b machine language   */
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
enum CS_BITS {                                                  
    IRD1, IRD0,
    COND2, COND1, COND0,
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
    DRMUX1, DRMUX0,
    SR1MUX1, SR1MUX0,
    ADDR1MUX,
    ADDR2MUX1, ADDR2MUX0,
    MARMUX,
    ALUK1, ALUK0,
    MIO_EN,
    R_W,
    DATA_SIZE,
    LSHF1,
    LD_Priv,
    LD_USP,
    LD_SSP,
    LD_EXCV,
    LD_Vector,
    Gate_PC_2,
    Gate_PSR,
    Gate_SP,
    Gate_Vector,
    PrivMUX,
    SPMUX1, SPMUX0,
    CCMUX,
    VectorMUX, 
    
    LD_STATE,
    LD_VA,
    LD_TempPsr, 
    LD_PTE, 
    Ld_Translate,
    LD_M,
    Ld_R,
    Gate_Va,
    GateTempPsr,
    Gate_PTE,
    Gate_PFN,
    VA_Mux1, VA_Mux0,
    Rmux,

/* MODIFY: you have to add all your new control signals */
    CONTROL_STORE_BITS
} CS_BITS;

/***************************************************************/
/* Functions to get at the control bits.                       */
/***************************************************************/
int GetIRD(int *x)           { return((x[IRD1] << 1) + x[IRD0]); }
int GetCOND(int *x)          { return((x[COND2] << 2) + (x[COND1] << 1) + x[COND0]); }
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
int GetDRMUX(int *x)         { return((x[DRMUX1] << 1) + x[DRMUX0]); }
int GetSR1MUX(int *x)        { return((x[SR1MUX1] << 1) + x[SR1MUX0]); }
int GetADDR1MUX(int *x)      { return(x[ADDR1MUX]); }
int GetADDR2MUX(int *x)      { return((x[ADDR2MUX1] << 1) + x[ADDR2MUX0]); }
int GetMARMUX(int *x)        { return(x[MARMUX]); }
int GetALUK(int *x)          { return((x[ALUK1] << 1) + x[ALUK0]); }
int GetMIO_EN(int *x)        { return(x[MIO_EN]); }
int GetR_W(int *x)           { return(x[R_W]); }
int GetDATA_SIZE(int *x)     { return(x[DATA_SIZE]); } 
int GetLSHF1(int *x)         { return(x[LSHF1]); }
int GetLD_Priv(int *x)         { return(x[LD_Priv]); }
int GetLD_USP(int *x)         { return(x[LD_USP]); }
int GetLD_SSP(int *x)         { return(x[LD_SSP]); }
int GetLD_EXCV(int *x)         { return(x[LD_EXCV]); }
int GetLD_Vector(int *x)         { return(x[LD_Vector]); }
int GetGate_PC_2(int *x)         { return(x[Gate_PC_2]); }
int GetGate_PSR(int *x)         { return(x[Gate_PSR]); }
int GetGate_SP(int *x)         { return(x[Gate_SP]); }
int GetGate_Vector(int *x)         { return(x[Gate_Vector]); }
int GetPrivMUX(int *x)         { return(x[PrivMUX]); }
int GetSPMux(int *x)        { return((x[SPMUX1] << 1) + x[SPMUX0]); }
int GetCCMux(int *x)         { return(x[CCMUX]); }
int GetVectorMUX(int *x)         { return(x[VectorMUX]); }
int GetLD_STATE(int *x)         { return(x[LD_STATE]); }
int GetLD_VA(int *x)           { return(x[LD_VA]); }
int GetLD_TempPsr(int *x)      { return(x[LD_TempPsr]); }
int GetLD_PTE(int *x)          { return(x[LD_PTE]); }
int GetLd_Translate(int *x)    { return(x[Ld_Translate]); }
int GetLD_M(int *x)            { return(x[LD_M]); }
int GetLD_R(int *x)            { return(x[Ld_R]); }

int GetGate_VA(int *x)         { return(x[Gate_Va]); }
int GetGateTEMP_PSR(int *x)     { return(x[GateTempPsr]); }
int GetGate_PTE(int *x)        { return(x[Gate_PTE]); }
int GetGate_PFN(int *x)        { return(x[Gate_PFN]); }


/* Two-bit muxes */
int GetVAMUX(int *x)           { return((x[VA_Mux1] << 1) + x[VA_Mux0]); }
int GetR_MUX(int *x)            { return(x[Rmux]); }

/* MODIFY: you can add more Get functions for your new control signals */

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

#define WORDS_IN_MEM    0x2000 /* 32 frames */ 
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

/* For lab 4 */
int INTV; /* Interrupt vector register */
int EXCV; /* Exception vector register */
int SSP; /* Initial value of system stack pointer */
/* MODIFY: you should add here any other registers you need to implement interrupts and exceptions */

/* For lab 5 */
int PTBR; /* This is initialized when we load the page table */
int VA;   /* Temporary VA register */
int TEMP_PSR;
int PTE;
int RBit;
int MBit;
int Translate;


 /* Interrupt vector register */
 /* Exception vector register */
int Priv; /* PSR[15] */
int Vector;
int Saved_USP;
int Saved_SSP;
int STATE;  
/* MODIFY: you should add here any other registers you need to implement virtual memory */

} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/* For lab 5 */
#define PAGE_NUM_BITS 9
#define PTE_PFN_MASK 0x3E00
#define PTE_VALID_MASK 0x0004
#define PAGE_OFFSET_MASK 0x1FF

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
  printf("Entered state: %d\n", CURRENT_LATCHES.STATE_NUMBER);

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

    printf("\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    printf("-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	printf("  0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
    printf("\n");

    /* dump the memory contents into the dumpsim file */
    fprintf(dumpsim_file, "\nMemory content [0x%0.4x..0x%0.4x] :\n", start, stop);
    fprintf(dumpsim_file, "-------------------------------------\n");
    for (address = (start >> 1); address <= (stop >> 1); address++)
	fprintf(dumpsim_file, " 0x%0.4x (%d) : 0x%0.2x%0.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
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
    printf("PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    printf("IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
printf("STATE_NUMBER : %d\n\n", CURRENT_LATCHES.STATE_NUMBER);
    printf("BUS          : 0x%0.4x\n", BUS);
    printf("MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    printf("MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    printf("Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	printf("%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
    printf("\n");

    /* dump the state information into the dumpsim file */
    fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
    fprintf(dumpsim_file, "-------------------------------------\n");
    fprintf(dumpsim_file, "Cycle Count  : %d\n", CYCLE_COUNT);
    fprintf(dumpsim_file, "PC           : 0x%0.4x\n", CURRENT_LATCHES.PC);
    fprintf(dumpsim_file, "IR           : 0x%0.4x\n", CURRENT_LATCHES.IR);
fprintf(dumpsim_file, "STATE_NUMBER : %d\n\n", CURRENT_LATCHES.STATE_NUMBER);
    fprintf(dumpsim_file, "BUS          : 0x%0.4x\n", BUS);
    fprintf(dumpsim_file, "MDR          : 0x%0.4x\n", CURRENT_LATCHES.MDR);
    fprintf(dumpsim_file, "MAR          : 0x%0.4x\n", CURRENT_LATCHES.MAR);
    fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
    fprintf(dumpsim_file, "Registers:\n");
    for (k = 0; k < LC_3b_REGS; k++)
	fprintf(dumpsim_file, "%d: 0x%0.4x\n", k, CURRENT_LATCHES.REGS[k]);
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

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
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
void load_program(char *program_filename, int is_virtual_base) {                   
    FILE * prog;
    int ii, word, program_base, pte, virtual_pc;

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

    if (is_virtual_base) {
      if (CURRENT_LATCHES.PTBR == 0) {
	printf("Error: Page table base not loaded %s\n", program_filename);
	exit(-1);
      }

      /* convert virtual_base to physical_base */
      virtual_pc = program_base << 1;
      pte = (MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][1] << 8) | 
	     MEMORY[(CURRENT_LATCHES.PTBR + (((program_base << 1) >> PAGE_NUM_BITS) << 1)) >> 1][0];

      printf("virtual base of program: %04x\npte: %04x\n", program_base << 1, pte);
		if ((pte & PTE_VALID_MASK) == PTE_VALID_MASK) {
	      program_base = (pte & PTE_PFN_MASK) | ((program_base << 1) & PAGE_OFFSET_MASK);
   	   printf("physical base of program: %x\n\n", program_base);
	      program_base = program_base >> 1; 
		} else {
   	   printf("attempting to load a program into an invalid (non-resident) page\n\n");
			exit(-1);
		}
    }
    else {
      /* is page table */
     CURRENT_LATCHES.PTBR = program_base << 1;
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
	MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;;
	ii++;
    }

    if (CURRENT_LATCHES.PC == 0 && is_virtual_base) 
      CURRENT_LATCHES.PC = virtual_pc;

    printf("Read %d words from program into memory.\n\n", ii);
}

/***************************************************************/
/*                                                             */
/* Procedure : initialize                                      */
/*                                                             */
/* Purpose   : Load microprogram and machine language program  */ 
/*             and set up initial state of the machine         */
/*                                                             */
/***************************************************************/
void initialize(char *argv[], int num_prog_files) { 
    int i;
    init_control_store(argv[1]);

    init_memory();
    load_program(argv[2],0);
    for ( i = 0; i < num_prog_files; i++ ) {
	load_program(argv[i + 3],1);
    }
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.STATE_NUMBER = INITIAL_STATE_NUMBER;
    memcpy(CURRENT_LATCHES.MICROINSTRUCTION, CONTROL_STORE[INITIAL_STATE_NUMBER], sizeof(int)*CONTROL_STORE_BITS);
    CURRENT_LATCHES.SSP = 0x3000; /* Initial value of system stack pointer */



    //ADDED

CURRENT_LATCHES.Saved_SSP = 0x3000; 
    CURRENT_LATCHES.Priv = 1;
    NEXT_LATCHES = CURRENT_LATCHES;
        CURRENT_LATCHES.PC = 0x3000;
    CURRENT_LATCHES.Priv = 1;          
    CURRENT_LATCHES.MDR  = 0;
    CURRENT_LATCHES.MAR = 0x3000;
    CURRENT_LATCHES.IR   = 0;
    CURRENT_LATCHES.INTV = 0;
    CURRENT_LATCHES.EXCV = 0;
    CURRENT_LATCHES.Vector = 0;
    CURRENT_LATCHES.STATE_NUMBER = 18; 
    CURRENT_LATCHES.STATE = 18;
    CURRENT_LATCHES.READY = 0;
    CURRENT_LATCHES.BEN = 0;
    CURRENT_LATCHES.N = 0;
    CURRENT_LATCHES.Z = 1;
    CURRENT_LATCHES.P = 0;
    RUN_BIT = TRUE;
    //


/* MODIFY: you can add more initialization code HERE */

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
    if (argc < 4) {
	printf("Error: usage: %s <micro_code_file> <page table file> <program_file_1> <program_file_2> ...\n",
	       argv[0]);
	exit(1);
    }

    printf("LC-3b Simulator\n\n");

    initialize(argv, argc - 3);

    if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
	printf("Error: Can't open dumpsim file\n");
	exit(-1);
    }

    while (1)
	get_command(dumpsim_file);

}

/***************************************************************/
/* Do not modify the above code, except for the places indicated 
   with a "MODIFY:" comment.
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
static inline int opcode(void) {
    return (CURRENT_LATCHES.IR >> 12) & 0xF;
}

static inline int is_fetch_state(int st) {
    // Use your actual fetch states here. From your code/comments,
    // 18 is initial fetch; 33 is the “resume fetch”.
    return (st == 18) || (st == 33);
}

// If your microcode does a dedicated TRAP vector table read, list those state numbers.
static inline int is_trap_vector_read_state(int st) {
    // TODO: set these to your actual states that read 0x0000–0x01FF for TRAP.
    // If unknown, return 0 for now; you can refine once you paste those states.
    return 0;
}
enum {
    UNSIGNED, SIGNED
};

int power(int base, int exp) {
    // returns base^exp (handles 0 edge cases too)
    if (base == 0) return 0;

    int result = 1;
    int i = 0;
    while (i < exp) {
        result *= base;
        i++;
    }
    return result;
}
/* Exception encoding in EXC: ex2 ex1 ex0
 * 000 : no exception
 * 001 : unknown opcode      -> vector x05
 * 010 : unaligned access    -> vector x03
 * 100 : page fault          -> vector x02
 * 101 : protection fault    -> vector x04  (protection > page fault)
 */
#define EXC_NONE       0x0  /* 000 */
#define EXC_UNKNOWN    0x1  /* 001 */
#define EXC_UNALIGNED  0x2  /* 010 */
#define EXC_PAGEFAULT  0x4  /* 100 */
#define EXC_PROTECT    0x5  /* 101 */

int bitsToDec(int num, int hi, int lo, int isSigned) {
    // grab bits from hi..lo and convert to int (signed or unsigned)
    int upperMask, val;

    if (!isSigned) {
        upperMask = power(2, hi + 1) - 1 - (power(2, lo) - 1);
        val = (num & upperMask) >> lo;
    } else {
        upperMask = power(2, hi) - 1 - (power(2, lo) - 1);
        val = (num & upperMask) >> lo;
        if (num & power(2, hi)) val = -power(2, hi - lo) + val;
    }
    return val;
}

// figure out which register is destination based on DRMUX
int getDR(void) {
    int drmux = GetDRMUX(CURRENT_LATCHES.MICROINSTRUCTION);
    int result;

    if (drmux == 2) result = 6;
    else if (drmux == 1) result = 7;
    else result = bitsToDec(CURRENT_LATCHES.IR, 11, 9, UNSIGNED);

    return result;
}

// figure out source register 1 based on SR1MUX control
int getSR1(void) {
    int sr1mux = GetSR1MUX(CURRENT_LATCHES.MICROINSTRUCTION);
    int result;

    if (sr1mux == 2) result = 6;
    else if (sr1mux == 1) result = bitsToDec(CURRENT_LATCHES.IR, 8, 6, UNSIGNED);
    else result = bitsToDec(CURRENT_LATCHES.IR, 11, 9, UNSIGNED);

    return result;
}

// pick which vector (interrupt or exception) to use
int getVECTOR(void) {
    if (GetVectorMUX(CURRENT_LATCHES.MICROINSTRUCTION)) return CURRENT_LATCHES.EXCV;
    else return CURRENT_LATCHES.INTV;
}

#define DR getDR()
#define SR1 getSR1()
#define VECTOR getVECTOR()

int EXC; // latch to hold exception info
static int MEM_PHASE = 1; // keeps track of memory cycle phase
static int MEM_FIN  = 0;  // signals memory done

// values each bus source drives
static int BUS_MARMUX, BUS_PC, BUS_ALU, BUS_SHF, BUS_MDR;
static int BUS_PC_MINUS_2, BUS_PSR, BUS_SP, BUS_VECTOR;
static int BUS_VA, BUS_TEMP_PSR,BUS_PTE,BUS_PFN, VA_Val;

// sign-extend 8-bit to 16-bit
static inline int sext8_to16(int x) {
    int b7 = (x >> 7) & 1;
    return b7 ? (x | 0xFF00) : (x & 0x00FF);
}

// add two 16-bit values with wraparound
static inline int add16(int a, int b) {
    return Low16bits(a + b);
}

// compute first address input (either PC or reg[SR1])
static int addr1_value(void) {
    int sel = GetADDR1MUX(CURRENT_LATCHES.MICROINSTRUCTION);
    return (!sel) ? CURRENT_LATCHES.PC : CURRENT_LATCHES.REGS[SR1];
}

// compute offset based on ADDR2MUX and shift if needed
static int addr2_value(void) {
    int sel = GetADDR2MUX(CURRENT_LATCHES.MICROINSTRUCTION);
    int s = 0;
    if (sel == 0) s = 0;
    else if (sel == 1) s = bitsToDec(CURRENT_LATCHES.IR, 5, 0, SIGNED);
    else if (sel == 2) s = bitsToDec(CURRENT_LATCHES.IR, 8, 0, SIGNED);
    else s = bitsToDec(CURRENT_LATCHES.IR, 10, 0, SIGNED);
    if (GetLSHF1(CURRENT_LATCHES.MICROINSTRUCTION)) s <<= 1;
    return Low16bits(s);
}

// helper for final address calculation
static inline int adder_addr(void) {
    return add16(addr1_value(), addr2_value());
}

// compute address for trap vector table
static inline int trapvec_word(void) {
    return Low16bits(0x0200 | (CURRENT_LATCHES.Vector << 1));
}

// handles all microsequencer logic and state transitions
void eval_micro_sequencer() {
    int *u   = CURRENT_LATCHES.MICROINSTRUCTION;
    const int ird  = GetIRD(u);   /* 00,01,10,11 */
    const int cond = GetCOND(u);  /* 3 bits */
    const int j    = GetJ(u) & 0x3F;

    /* ---------- Lab 5: Interrupt injection (same as before) ---------- */
    if (CYCLE_COUNT == 299) NEXT_LATCHES.INTV = 0x01;
    if (CURRENT_LATCHES.STATE_NUMBER == 47) NEXT_LATCHES.INTV = 0x00;

    /* ---------- Compute exceptions according to your rules ---------- */
    int ex_unknown   = 0;
    int ex_unaligned = 0;
    int ex_prot      = 0;
    int ex_pf        = 0;

    const int opc = opcode();

    /* Unknown opcode: OPCODE == 10 or 11 (decimal) -> 0xA, 0xB */
    ex_unknown = (opc == 0xA || opc == 0xB);

    /* The VM-related checks only make sense in the special "exception
       check" microstates; we keep them gated on COND == 6 like before. */
    if (cond == 6) {
        /* Unaligned access:
         *  (VA & 0x1) && (state == 3 or 7)
         */
        if ( (CURRENT_LATCHES.VA & 0x1) &&
             (CURRENT_LATCHES.STATE_NUMBER == 3 ||
              CURRENT_LATCHES.STATE_NUMBER == 7) ) {
            ex_unaligned = 1;
        }

        /* Protection:
         *  (!(OPCODE == 15) && CURRENT_LATCHES.Priv && !(MDR & 0x8))
         *  - Don't fault on TRAP (opcode 15)
         *  - Priv == 1 => user mode (your convention)
         *  - MDR bit 3 (0x8) is the "permission" bit; 0 => not allowed
         */
        if (!(opc == 0xF) && CURRENT_LATCHES.Priv && !(CURRENT_LATCHES.MDR & 0x8)) {
            ex_prot = 1;
        }

        /* Page fault:
         *   !(MDR & 0x4)  -> PTE valid bit is 0
         */
        if (!(CURRENT_LATCHES.MDR & 0x4)) {
            ex_pf = 1;
        }
    }

    /* Build EXC bits with priority:
     *  protection > page fault > unaligned > unknown
     */
    EXC = EXC_NONE;
    if (ex_prot)      EXC = EXC_PROTECT;     /* 101 */
    else if (ex_pf)   EXC = EXC_PAGEFAULT;   /* 100 */
    else if (ex_unaligned) EXC = EXC_UNALIGNED; /* 010 */
    else if (ex_unknown)   EXC = EXC_UNKNOWN;   /* 001 */

    int next_state = 0;

    /* ---------- IRD decoding ---------- */
    if (ird == 1) {
        /* IRD = 01 : opcode decoder
         * - On unknown opcode -> state 63
         * - Else go to opcode[15:12]
         */
        if (ex_unknown) {
            next_state = 63;
        } else {
            next_state = (CURRENT_LATCHES.IR >> 12) & 0x000F;
        }
    }
    else if (ird == 2) {
        /* IRD = 10 : “ESR” / exception-type decoder using STATE latch.
         * If any VM exception (unaligned, protection, page fault) is set,
         * we go directly to state 63; otherwise decode based on STATE.
         */
        int vm_exc = (EXC == EXC_UNALIGNED) ||
                     (EXC == EXC_PAGEFAULT) ||
                     (EXC == EXC_PROTECT);

        if (vm_exc) {
            next_state = 63;
        } else {
            switch (CURRENT_LATCHES.STATE) {
                case 2:  next_state = 29; break;
                case 3:  next_state = 24; break;
                case 6:  next_state = 25; break;
                case 7:  next_state = 23; break;
                case 18:
                case 19: next_state = 33; break;
                default: next_state = 0;  break;
            }
        }
    }
    else if (ird == 3) {
        /* IRD = 11 : Translation decoder – uses Translate latch
           instead of STATE (this is your new VM decoder path). */
        switch (CURRENT_LATCHES.Translate) {
            case 8:  next_state = 40; break;
            case 26: next_state = 48; break;
            case 43: next_state = 45; break;
            case 39: next_state = 34; break;
            case 38: next_state = 58; break;
            case 18: next_state = 61; break;
            case 19: next_state = 61; break;
            case 2:  
            case 3:  
            case 6:  
            case 7:  next_state = 61; break;
            case 15: next_state = 28; break;
            default: next_state = 18; break;  /* fallback to fetch */
        }
    }
    else {
        /* IRD = 00 : normal J-field + COND patching */

        int patched = j;

        /* cond == 5 : OR in interrupt bit (J[4]) if INTV set */
        if (cond == 5 && CURRENT_LATCHES.INTV)  patched |= (1 << 4);

        /* cond == 4 : OR in privilege bit (J[3]) when in user mode */
        if (cond == 4 && CURRENT_LATCHES.Priv)  patched |= (1 << 3);

        /* cond == 2 : OR in BEN (J[2]) */
        if (cond == 2 && CURRENT_LATCHES.BEN)   patched |= (1 << 2);

        /* cond == 1 : OR in READY (J[1]) */
        if (cond == 1 && CURRENT_LATCHES.READY) patched |= (1 << 1);

        /* cond == 3 : test IR[11] bit (imm vs reg) -> J[0] */
        if (cond == 3 && (CURRENT_LATCHES.IR & 0x0800)) patched |= 1;

        /* cond == 6 : VM exception check (unaligned/protection/page fault).
           If any of those is set, go to state 63; else follow patched J. */
        if (cond == 6) {
            int vm_exc = (EXC == EXC_UNALIGNED) ||
                         (EXC == EXC_PAGEFAULT) ||
                         (EXC == EXC_PROTECT);
            next_state = vm_exc ? 63 : patched;
        } else {
            next_state = patched;
        }
    }

    /* Latch next microstate and microinstruction */
    NEXT_LATCHES.STATE_NUMBER = next_state;

    if (GetLD_STATE(u)) {
        /* Save the "old" state number for the IRD=10 ESR decoder */
        NEXT_LATCHES.STATE = CURRENT_LATCHES.STATE_NUMBER;
    }

    memcpy(NEXT_LATCHES.MICROINSTRUCTION,
           CONTROL_STORE[NEXT_LATCHES.STATE_NUMBER],
           sizeof(int) * CONTROL_STORE_BITS);
}


// controls memory timing and READY bit behavior
int cycleCount = 0;
int cycleCount2 = 0;
void cycle_memory() {
    printf("CycleCount %d.\n", CURRENT_LATCHES.STATE_NUMBER);

    if (CURRENT_LATCHES.MICROINSTRUCTION[MIO_EN]) {
        cycleCount++;
        cycleCount2++;
        NEXT_LATCHES.READY = 0;
        

        int mem_index = CURRENT_LATCHES.MAR >> 1;

        if (cycleCount == 4) {
            int is_read = (CURRENT_LATCHES.MICROINSTRUCTION[R_W] == 0);
            int is_byte = (CURRENT_LATCHES.MICROINSTRUCTION[DATA_SIZE] == 0);

            if (is_read) {
                int lo = MEMORY[mem_index][0] & 0xFF;
                int hi = MEMORY[mem_index][1] & 0xFF;
                //REMOVE REMOVE 
                NEXT_LATCHES.MDR = (hi << 8) | lo;
            } else {
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
            NEXT_LATCHES.READY = 1;
            MEM_FIN = 1;

        }

        if (cycleCount == 5) cycleCount = 0;
    } else {
        cycleCount = 0;
        NEXT_LATCHES.READY = 0;
    }
}

// figure out all values each component will put on the bus
/*    
Gate_Va,
GateTempPsr,
Gate_PTE,
Gate_PFN,
VA_Mux1, VA_Mux0,
Rmux,
*/


void eval_bus_drivers() {
    if (GetMARMUX(CURRENT_LATCHES.MICROINSTRUCTION))
        BUS_MARMUX = adder_addr();
    else
        BUS_MARMUX = Low16bits(bitsToDec(CURRENT_LATCHES.IR, 7, 0, UNSIGNED) << 1);

    BUS_PC = CURRENT_LATCHES.PC;

    // ALU operations: ADD/AND/XOR/PASSA
    {
        const int useImmediate = bitsToDec(CURRENT_LATCHES.IR, 5, 5, UNSIGNED);
        const int operandB = useImmediate ? bitsToDec(CURRENT_LATCHES.IR, 4, 0, SIGNED)
                                          : CURRENT_LATCHES.REGS[bitsToDec(CURRENT_LATCHES.IR, 2, 0, UNSIGNED)];
        const int operandA = CURRENT_LATCHES.REGS[SR1];

        switch (GetALUK(CURRENT_LATCHES.MICROINSTRUCTION)) {
            case 0:  BUS_ALU = add16(operandA, operandB);      break;
            case 1:  BUS_ALU = Low16bits(operandA & operandB); break;
            case 2:  BUS_ALU = Low16bits(operandA ^ operandB); break;
            default: BUS_ALU = Low16bits(operandA);            break;
        }
    }

    // handle shift instructions (LSHF, RSHFL, RSHFA)
    {
        const int shiftDirection = bitsToDec(CURRENT_LATCHES.IR, 4, 4, UNSIGNED);
        const int shiftType      = bitsToDec(CURRENT_LATCHES.IR, 5, 5, UNSIGNED);
        const int shiftAmount    = bitsToDec(CURRENT_LATCHES.IR, 3, 0, UNSIGNED);
        const int sourceValue    = CURRENT_LATCHES.REGS[SR1];
        int result = 0;

        if (shiftDirection == 0) result = Low16bits(sourceValue << shiftAmount);
        else if (shiftType == 0) result = Low16bits((unsigned)sourceValue >> shiftAmount);
        else {
            int temp = sourceValue;
            if (bitsToDec(sourceValue, 15, 15, UNSIGNED)) {
                for (int i = 0; i < shiftAmount; i++) temp = (temp >> 1) | 0x8000;
            } else temp >>= shiftAmount;
            result = Low16bits(temp);
        }
        BUS_SHF = result;
    }

    // get MDR output (word or byte)
    if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION))
        BUS_MDR = CURRENT_LATCHES.MDR;
    else {
        if (CURRENT_LATCHES.MAR & 1) {
            int hi = (CURRENT_LATCHES.MDR >> 8) & 0xFF;
            BUS_MDR = Low16bits(sext8_to16(hi));
        } else {
            int lo = CURRENT_LATCHES.MDR & 0xFF;
            BUS_MDR = Low16bits(sext8_to16(lo));
        }
    }

    BUS_PC_MINUS_2 = Low16bits(CURRENT_LATCHES.PC - 2);
    BUS_PSR = Low16bits((CURRENT_LATCHES.Priv << 15) |
                        (CURRENT_LATCHES.N << 2) |
                        (CURRENT_LATCHES.Z << 1) |
                         CURRENT_LATCHES.P);

    // stack pointer mux logic
    {
        const int sp_sel = GetSPMux(CURRENT_LATCHES.MICROINSTRUCTION);
        int sp_val = 0;
        if      (sp_sel == 0) sp_val = CURRENT_LATCHES.Saved_USP;
        else if (sp_sel == 1) sp_val = add16(CURRENT_LATCHES.REGS[SR1], 2);
        else if (sp_sel == 2) sp_val = add16(CURRENT_LATCHES.REGS[SR1], -2);
        else                  sp_val = CURRENT_LATCHES.Saved_SSP;
        BUS_SP = sp_val;
    }

    BUS_VECTOR = trapvec_word();

/*
    BUS_VA, BUS_TEMP_PSR,BUS_PTE,BUS_PFN;*/


 if(GetGate_VA((CURRENT_LATCHES.MICROINSTRUCTION))){
        if(GetVAMUX((CURRENT_LATCHES.MICROINSTRUCTION)) == 0){
            BUS_VA = Low16bits(CURRENT_LATCHES.PTBR + ((CURRENT_LATCHES.VA & 0xFE00) >> 8));
            // printf("VPN: 0x%0.4x\n", ((CURRENT_LATCHES.VA & 0xFE00) >> 8));
            // printf("VA Value: 0x%0.4x\n", VA_Val);
        }
        else if(GetVAMUX((CURRENT_LATCHES.MICROINSTRUCTION)) == 1){
            BUS_VA = Low16bits((CURRENT_LATCHES.PTE & 0x3E00) + (CURRENT_LATCHES.VA & 0x01FF));
        }
        else if(GetVAMUX((CURRENT_LATCHES.MICROINSTRUCTION)) == 2){
            BUS_VA = Low16bits(CURRENT_LATCHES.VA);
        }
        else if (GetVAMUX(CURRENT_LATCHES.MICROINSTRUCTION) == 3) {
                BUS_VA = Low16bits((CURRENT_LATCHES.PTE&0x3E00) + (CURRENT_LATCHES.VA & 0x01FF));
        }
    }
    
/////MAKE SURE U NEED PFN
    BUS_TEMP_PSR = CURRENT_LATCHES.TEMP_PSR;

    if(GetGate_PTE(CURRENT_LATCHES.MICROINSTRUCTION)){
       BUS_PTE = Low16bits(CURRENT_LATCHES.PTE);
    }
    if(GetGate_PTE(CURRENT_LATCHES.MICROINSTRUCTION)){
        BUS_PFN = Low16bits((CURRENT_LATCHES.PTE&0x3E00) + (CURRENT_LATCHES.VA & 0x01FF));
    }
    

}

// pick which signal actually drives the bus this cycle
void drive_bus() {
    if (GetGATE_ALU(CURRENT_LATCHES.MICROINSTRUCTION)) { BUS = BUS_ALU; return; }
    if (GetGATE_MARMUX(CURRENT_LATCHES.MICROINSTRUCTION)) { BUS = BUS_MARMUX; return; }
    if (GetGATE_PC(CURRENT_LATCHES.MICROINSTRUCTION)) { BUS = BUS_PC; return; }
    if (GetGATE_SHF(CURRENT_LATCHES.MICROINSTRUCTION)) { BUS = BUS_SHF; return; }
    if (GetGATE_MDR(CURRENT_LATCHES.MICROINSTRUCTION)) { BUS = BUS_MDR; return; }
    if (GetGate_PC_2(CURRENT_LATCHES.MICROINSTRUCTION)) { BUS = BUS_PC_MINUS_2; return; }
    if (GetGate_PSR(CURRENT_LATCHES.MICROINSTRUCTION)) { BUS = BUS_PSR; return; }
    if (GetGate_SP(CURRENT_LATCHES.MICROINSTRUCTION)) { BUS = BUS_SP; return; }
    if (GetGate_Vector(CURRENT_LATCHES.MICROINSTRUCTION)) { BUS = BUS_VECTOR; return; }
//new 
    if (GetGate_VA(CURRENT_LATCHES.MICROINSTRUCTION))       { BUS = BUS_VA;         return; }
    if (GetGateTEMP_PSR(CURRENT_LATCHES.MICROINSTRUCTION))   { BUS = BUS_TEMP_PSR;   return; }
    if (GetGate_PTE(CURRENT_LATCHES.MICROINSTRUCTION))      { BUS = BUS_PTE;        return; }
    if (GetGate_PFN(CURRENT_LATCHES.MICROINSTRUCTION))      { BUS = BUS_PFN;        return; }

    BUS = 0; // nothing drives the bus
}

void latch_datapath_values() {
    // MAR
    if (GetLD_MAR(CURRENT_LATCHES.MICROINSTRUCTION)) {
        NEXT_LATCHES.MAR = BUS;
    }

    // MDR
    if (GetLD_MDR(CURRENT_LATCHES.MICROINSTRUCTION)) {
        if (GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION)) {
            if (MEM_FIN) {
                /* Read complete: latch from MEMORY (word) */
                const int addr = (CURRENT_LATCHES.MAR >> 1) & (WORDS_IN_MEM - 1);
                NEXT_LATCHES.MDR = Low16bits((MEMORY[addr][1] << 8) | MEMORY[addr][0]);
                MEM_FIN = 0;
            }
        } else {
            /* From bus: if MAR[0]==1, replicate low 8 bits into both halves; else full word */
            if (CURRENT_LATCHES.MAR & 1) {
                const int b = BUS & 0x00FF;
                NEXT_LATCHES.MDR = Low16bits((b << 8) | b);
            } else {
                NEXT_LATCHES.MDR = BUS;
            }
        }
    }

    //Memory write 
    if (GetR_W(CURRENT_LATCHES.MICROINSTRUCTION) && GetMIO_EN(CURRENT_LATCHES.MICROINSTRUCTION)) {
        if (MEM_FIN) {
            const int addr = (CURRENT_LATCHES.MAR >> 1) & (WORDS_IN_MEM - 1);
            if (GetDATA_SIZE(CURRENT_LATCHES.MICROINSTRUCTION)) {
                /* STW */
                MEMORY[addr][0] = CURRENT_LATCHES.MDR & 0xFF;
                MEMORY[addr][1] = (CURRENT_LATCHES.MDR >> 8) & 0xFF;
            } else {
                /* STB: choose high/low byte by MAR[0] */
                int which = CURRENT_LATCHES.MAR & 1;
                int val8  = which ? ((CURRENT_LATCHES.MDR >> 8) & 0xFF) : (CURRENT_LATCHES.MDR & 0xFF);
                MEMORY[addr][which] = val8;
            }
            MEM_FIN = 0;
        }
    }

    //BEN
    if (GetLD_BEN(CURRENT_LATCHES.MICROINSTRUCTION)) {
        const int ir = CURRENT_LATCHES.IR;
        const int n_en = (ir >> 11) & 1;
        const int z_en = (ir >> 10) & 1;
        const int p_en = (ir >>  9) & 1;
        NEXT_LATCHES.BEN = (CURRENT_LATCHES.N && n_en) ||
                           (CURRENT_LATCHES.Z && z_en) ||
                           (CURRENT_LATCHES.P && p_en);
    }

    //ir
    if (GetLD_IR(CURRENT_LATCHES.MICROINSTRUCTION)) {
        NEXT_LATCHES.IR = BUS;
    }

    // Register file write 
    if (GetLD_REG(CURRENT_LATCHES.MICROINSTRUCTION)) {
        NEXT_LATCHES.REGS[DR] = BUS;
    }

    // CC 
    if (GetLD_CC(CURRENT_LATCHES.MICROINSTRUCTION)) {
        const int sval = bitsToDec(BUS, 15, 0, SIGNED);
        NEXT_LATCHES.N = (sval < 0);
        NEXT_LATCHES.Z = (sval == 0);
        NEXT_LATCHES.P = (sval > 0);
    }

    /* PC */
    if (GetLD_PC(CURRENT_LATCHES.MICROINSTRUCTION)) {
        const int pmux = GetPCMUX(CURRENT_LATCHES.MICROINSTRUCTION);
        int pc_next = CURRENT_LATCHES.PC; /* default for clarity */
        if (pmux == 0) {
            pc_next = Low16bits(CURRENT_LATCHES.PC + 2);
        } else if (pmux == 1) {
            pc_next = BUS;
        } else {
            pc_next = adder_addr();
        }
        NEXT_LATCHES.PC = pc_next;
    }

    /* Privilege latch (either from PSR on BUS or forced via PrivMUX) */
    if (GetLD_Priv(CURRENT_LATCHES.MICROINSTRUCTION)) {
        if (GetPrivMUX(CURRENT_LATCHES.MICROINSTRUCTION)) {
            NEXT_LATCHES.Priv = 0;
        } else {
            NEXT_LATCHES.Priv = (BUS & 0x8000) ? 1 : 0;
        }
    }

    /* Saved USP/SSP */
    if (GetLD_USP(CURRENT_LATCHES.MICROINSTRUCTION)) {
        NEXT_LATCHES.Saved_USP = CURRENT_LATCHES.REGS[SR1];
    }
    if (GetLD_SSP(CURRENT_LATCHES.MICROINSTRUCTION)) {
        NEXT_LATCHES.Saved_SSP = CURRENT_LATCHES.REGS[SR1];
    }

    /* Exception code latch */
    /* Exception code latch
     * EXC encodings:
     *  001 : unknown    -> vector x05
     *  010 : unaligned  -> vector x03
     *  100 : page fault -> vector x02
     *  101 : protection -> vector x04
     */
    if (GetLD_EXCV(CURRENT_LATCHES.MICROINSTRUCTION) || (CURRENT_LATCHES.STATE_NUMBER == 51)) {
        int vec = NEXT_LATCHES.EXCV;  /* default: keep old */
        switch (EXC) {
            case EXC_UNKNOWN:   vec = 0x05; break; /* unknown opcode */
            case EXC_UNALIGNED: vec = 0x03; break; /* unaligned */
            case EXC_PAGEFAULT: vec = 0x02; break; /* page fault */
            case EXC_PROTECT:   vec = 0x04; break; /* protection */
            default:            /* EXC_NONE or other -> leave vec as-is */ break;
        }
        if (EXC != EXC_NONE) {
            NEXT_LATCHES.EXCV = vec;
        }
    }


    /* Vector */
    if (GetLD_Vector(CURRENT_LATCHES.MICROINSTRUCTION)) {
        NEXT_LATCHES.Vector = GetVectorMUX(CURRENT_LATCHES.MICROINSTRUCTION) ? CURRENT_LATCHES.EXCV : CURRENT_LATCHES.INTV;
    }

    if (GetLD_VA(CURRENT_LATCHES.MICROINSTRUCTION)) {
        NEXT_LATCHES.VA = Low16bits(BUS);
    }

    if (GetLD_TempPsr(CURRENT_LATCHES.MICROINSTRUCTION)) {
        NEXT_LATCHES.TEMP_PSR = Low16bits(BUS);
    }


    if(GetLD_PTE(CURRENT_LATCHES.MICROINSTRUCTION)){
        NEXT_LATCHES.PTE = Low16bits(BUS);
        // printf("New PTE: 0x%0.4x\n", NEXT_LATCHES.PTE);
    }

    if (GetLd_Translate(CURRENT_LATCHES.MICROINSTRUCTION)) NEXT_LATCHES.Translate = CURRENT_LATCHES.STATE_NUMBER;

    if(GetLD_R(CURRENT_LATCHES.MICROINSTRUCTION)){
        if(GetR_MUX(CURRENT_LATCHES.MICROINSTRUCTION)) NEXT_LATCHES.PTE |= 0x0001;
        else NEXT_LATCHES.PTE &= 0xFFFE;
        // printf("New PTE: 0x%0.4x\n", NEXT_LATCHES.PTE); 
        //MAYBE CHANGE THIS
    }

if (GetLD_M(CURRENT_LATCHES.MICROINSTRUCTION)) {
    if ((CURRENT_LATCHES.Translate == 3) || (CURRENT_LATCHES.Translate == 7))
        NEXT_LATCHES.PTE |= 0x0002;   /* set M bit */
    else
        NEXT_LATCHES.PTE &= 0xFFFD;   /* clear M bit */
}


    

/*
    LD_STATE,
    LD_VA,
    LD_TempPsr, 
    LD_PTE, 
    Ld_Translate,
    LD_M,
    Ld_R,
*/

}

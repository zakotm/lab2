/*
    REFER TO THE SUBMISSION INSTRUCTION FOR DETAILS

    Name 1: Michael Stecklein 
    Name 2: Zakaria Alrmaih
    UTEID 1: mrs4239
    UTEID 2: za3488
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

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
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
*/

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

  int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
  int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {                                                    
  printf("----------------LC-3b ISIM Help-----------------------\n");
  printf("go               -  run program to completion         \n");
  printf("run n            -  execute program for n instructions\n");
  printf("mdump low high   -  dump memory from low to high      \n");
  printf("rdump            -  dump the register & bus values    \n");
  printf("?                -  display this help menu            \n");
  printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  process_instruction();
  CURRENT_LATCHES = NEXT_LATCHES;
  INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
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
/* Purpose   : Simulate the LC-3b until HALTed                 */
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
  printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
  printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  printf("Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
  fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
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

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */ 
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) { 
  int i;

  init_memory();
  for ( i = 0; i < num_prog_files; i++ ) {
    load_program(program_filename);
    while(*program_filename++ != '\0');
  }
  CURRENT_LATCHES.Z = 1;  
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
  if (argc < 2) {
    printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
           argv[0]);
    exit(1);
  }

  printf("LC-3b Simulator\n\n");

  initialize(argv[1], argc - 1);

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

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/









/****************************************************************/
/* DECLARATIONS													*/
/****************************************************************/


/****************************************************************/
/* Instruciton enum -	define list of instructions and their	*/
/*						respective opcodes						*/
/****************************************************************/
enum Instruction {
	ADD = 1,
	AND = 5,
	BR = 0,
	JMP = 12, /* includes RET */
	JSR = 4, /* includes JSRR */
	LDB = 2,
	LDW = 6,
	LEA = 14,
	RTI = 8,
	SHF = 13,
	STB = 3,
	STW = 7,
	TRAP = 15,
	XOR = 9 /* includes NOT */
};

/****************************************************************/
/* 																*/
/* fetchInstrunction -	returns the instruction that is 		*/
/*						currently pointed to by the PC 	 		*/
/*																*/
/* Output	: The integer instruction							*/
/*																*/
/****************************************************************/
int fetchInstrunction();

/****************************************************************/
/* 																*/
/* decodeOpcode - decodes the instruction's opcode 				*/
/*																*/
/* Input	: The instruction to decode 						*/
/* Output	: The integer opcode 								*/
/*																*/
/****************************************************************/
int decodeOpcode(int instruction);

/****************************************************************/
/* 																*/
/* execute - 	executes the instruction, altering the latch 	*/
/*				states and preincrementing the PC. Delegates	*/
/*				execution to an execute function specifically	*/
/*				written for the given instruction 				*/
/*																*/
/* Input	: The integer opcode and instruction to execute		*/
/*																*/
/****************************************************************/
void execute(int opcode, int instruction);

/****************************************************************/
/* executeADD													*/
/****************************************************************/
void executeADD(int opcode, int instruction);

/****************************************************************/
/* executeAND													*/
/****************************************************************/
void executeAND(int opcode, int instruction);

/****************************************************************/
/* executeBR													*/
/****************************************************************/
void executeBR(int opcode, int instruction);

/****************************************************************/
/* executeJMP													*/
/****************************************************************/
void executeJMP(int opcode, int instruction);

/****************************************************************/
/* executeJSR													*/
/****************************************************************/
void executeJSR(int opcode, int instruction);

/****************************************************************/
/* executeLDB													*/
/****************************************************************/
void executeLDB(int opcode, int instruction);

/****************************************************************/
/* executeLDW													*/
/****************************************************************/
void executeLDW(int opcode, int instruction);

/****************************************************************/
/* executeLEA													*/
/****************************************************************/
void executeLEA(int opcode, int instruction);

/****************************************************************/
/* executeRTI													*/
/****************************************************************/
void executeRTI(int opcode, int instruction);

/****************************************************************/
/* executeSHF													*/
/****************************************************************/
void executeSHF(int opcode, int instruction);

/****************************************************************/
/* executeSTB													*/
/****************************************************************/
void executeSTB(int opcode, int instruction);

/****************************************************************/
/* executeSTW													*/
/****************************************************************/
void executeSTW(int opcode, int instruction);

/****************************************************************/
/* executeTRAP													*/
/****************************************************************/
void executeTRAP(int opcode, int instruction);

/****************************************************************/
/* executeXOR													*/
/****************************************************************/
void executeXOR(int opcode, int instruction);






/****************************************************************/
/* GIVEN METHOD 												*/
/****************************************************************/

/*  function: process_instruction
*  
*    Process one instruction at a time  
*       -Fetch one instruction
*       -Decode 
*       -Execute
*       -Update NEXT_LATCHES
*/  
int temp = 0;
void process_instruction(){   

	if (!temp) {
		MEMORY[0x4000>>1][0] = 0xaa;
		MEMORY[0x4000>>1][1] = 0xbb;
		MEMORY[0x4002>>1][0] = 0xcc;
		MEMORY[0x4002>>1][1] = 0xdd;
		MEMORY[0x4004>>1][0] = 0x1b;
		MEMORY[0x4004>>1][1] = 0xff;
		MEMORY[0x4006>>1][0] = 0xee;
		MEMORY[0x4006>>1][1] = 0x99;
		MEMORY[0x4008>>1][0] = 0x88;
		temp = 1;
	}

   int instruction, opcode;

   /* fetch instruction */
   instruction = fetchInstrunction();

   /* decode instruction */
   opcode = decodeOpcode(instruction);

   /* execute and update */
   execute(opcode, instruction);
}






/****************************************************************/
/* HELPER METHODS												*/
/****************************************************************/
#define Low8bits(x) ( x & 0x00FF)

/****************************************************************/
/* loadWord - 	loads word (16 bit) from memory. This function 	*/
/*				is necessary because memory is store such that	*/
/*				the the last bit of an address is ignored. This */
/*				is very confusing to me, so I am encapsulating	*/
/*				it within this function.						*/
/*																*/
/* Input 	: address to load from in memory 					*/
/* Ouput 	: the 16bit word stored at that address				*/
/****************************************************************/
int loadWord(int address) {
	assert(address%2 == 0);
	return Low16bits( ( MEMORY[address>>1][1] << 8 ) + MEMORY[address>>1][0] );
}

/****************************************************************/
/* loadLSByte - loads least significnat byte from memory at the */
/*				given address									*/
/*																*/
/* Input 	: address to load from in memory 					*/
/* Ouput 	: the least significant byte stored at that address	*/
/****************************************************************/
int loadLSByte(int address) {
	assert(address%2 == 0);
	return Low8bits( MEMORY[address >> 1][0] );
}

/****************************************************************/
/* loadMSByte - loads most significnat byte from memory at the  */
/*        given address                 */
/*                                */
/* Input  : address to load from in memory          */
/* Ouput  : the most significant byte stored at that address  */
/****************************************************************/
int loadMSByte(int address) {
  assert(address%2 == 0);
  return Low8bits( MEMORY[address >> 1][1] );
}

/****************************************************************/
/* loadByte - loads byte from memory at the given address. The */
/*        address can be aligned or unaligned                 */
/*                                */
/* Input  : address to load from in memory          */
/* Ouput  : the most significant byte stored at that address  */
/****************************************************************/
int loadByte(int address) {
  const int LSB_MASK = 0x0001;
  if (address & LSB_MASK) { /* odd, unaligned */
    return loadMSByte(address & ~LSB_MASK);
  } else { /* even, aligned */
    return loadLSByte(address);
  }
}

/****************************************************************/
/* storeWord - 	stores word (16 bit) into memory				*/
/*																*/
/* Input 	: address to store into and value to store 			*/
/****************************************************************/
void storeWord(int address, int value) {
	assert(address%2 == 0);
	MEMORY[address>>1][1] = (value & 0x0000FF00) >> 8;
	MEMORY[address>>1][0] = (value & 0x00FF);
}

/****************************************************************/
/* storeLSByte - stores least byte into memory at the */
/*        given address's least significant byte                 */
/*                                */
/* Input  : address to store to and byte to store      */
/****************************************************************/
void storeLSByte(int address, int byteVal) {
  assert(address%2 == 0);
  MEMORY[address >> 1][0] = Low8bits(byteVal);
}

/****************************************************************/
/* storeMSByte - stores least byte into memory at the */
/*        given address's most significant byte                 */
/*                                */
/* Input  : address to store to and byte to store      */
/****************************************************************/
void storeMSByte(int address, int byteVal) {
  assert(address%2 == 0);
  MEMORY[address >> 1][1] = Low8bits(byteVal);
}

/****************************************************************/
/* storeByte - stores byte into memory at the given address. The */
/*        address can be aligned or unaligned                 */
/*                                */
/* Input  : address to store to and byte to store          */
/****************************************************************/
void storeByte(int address, int byteVal) {
  const int LSB_MASK = 0x0001;
  if (address & LSB_MASK) { /* odd, unaligned */
    storeMSByte(address & ~LSB_MASK, byteVal);
  } else { /* even, aligned */
    storeLSByte(address, byteVal);
  }
}

/****************************************************************/
/* getRegisterValue - returns the value in the given register in*/
/*					the current latch 							*/
/*																*/
/* Input 	: register number 									*/
/* Output 	: register value 									*/
/****************************************************************/
int getRegisterValue(int registerNumber) {
	return CURRENT_LATCHES.REGS[registerNumber];
}

/****************************************************************/
/* setRegisterValue - sets the value in the given register in 	*/
/*					the next latch 								*/
/*																*/
/* Input 	: register number and new value						*/
/****************************************************************/
void setRegisterValue(int registerNumber, int newValue) {
	NEXT_LATCHES.REGS[registerNumber] = newValue;
}

/****************************************************************/
/* signExtend - sign extends the lenInBits-1 bit and returns the*/
/*				value 											*/
/*																*/
/* Input 	: value to change and the length in bits of the 	*/
/*				value to extend 								*/
/* Output 	: the sign extended value 							*/
/****************************************************************/
int signExtend(int valueToExtend, int lenInBits) {
  assert(lenInBits > 0  &&  lenInBits < 16);
	const int BIT_MASK = 1 << (lenInBits - 1);

  if (BIT_MASK & valueToExtend) { /* sign bit is 1 */
    const int MASK = 0xFFFFFFFF << lenInBits;
    return valueToExtend | MASK;
  } else { /* sign bit is 0 */
    const int MASK = 0xFFFFFFFF >> (16 - lenInBits);
    return valueToExtend & MASK;
  }
}

/****************************************************************/
/* setConditionCodes - sets NEXT_LATCHES' nzp bits based on the	*/
/*						result value passed in 					*/
/*																*/
/* Input 	: the result of the operation, to set nzp bits		*/
/****************************************************************/
void setConditionCodes(int resultVal) {
	int newN, newZ, newP;
	newN = newZ = newP = 0;

	if (resultVal > 0) {
		newP = 1;
	}
	if (resultVal < 0) {
		newN = 1;
	}
	if (resultVal == 0) {
		newZ = 1;
	}

	NEXT_LATCHES.N = newN;
	NEXT_LATCHES.Z = newZ;
	NEXT_LATCHES.P = newP;
}

/****************************************************************/
/* terminate -	ends the program 								*/
/*																*/
/****************************************************************/
void terminate() {
	assert(FALSE);
}

/****************************************************************/
/* executeALUOperation - since ADD, AND, and XOR ALU operations	*/
/*				have similar microcode structures, this one 	*/
/*				function will execute all of them (not SHF)		*/
/*																*/
/* Input 	: opcode of operation and the instruction			*/
/****************************************************************/
void executeLogicOperation(int opcode, int instruction) {
	const int DR_MASK		= 0x0E00;
	const int SR1_MASK		= 0x01C0;
	const int MODE_BIT_MASK	= 0x0020;
	const int SR2_MASK 		= 0x0007; /* may not be used */
	const int imm5_MASK		= 0x001F; /* may not be used */

	/* load first parameter */
	int firstParam = getRegisterValue(( SR1_MASK & instruction ) >> 6);

	/* calculate 2nd parameter */
	int secondParam;
	int scndParamFromConst = MODE_BIT_MASK & instruction;
	if (scndParamFromConst) { /* will use imm5 */
		secondParam = imm5_MASK & instruction;
		secondParam = signExtend(secondParam, 5);
	} else { /* will use SR2 */
		secondParam = getRegisterValue(SR2_MASK & instruction);
	}

	/* perform operation */
	int result;
	if (opcode == ADD) {
		result = Low16bits(firstParam + secondParam );
	}
	if (opcode == AND) {
		result = Low16bits( firstParam & secondParam );
	}
	if (opcode == XOR) {
		result = Low16bits( firstParam ^ secondParam );
	}

	/* set correct register */
	int dr = (DR_MASK & instruction) >> 9;
	setRegisterValue(dr, result);

	/* setcc() */
	setConditionCodes(result);
}







/****************************************************************/
/* IMPLEMENTATIONS												*/
/****************************************************************/

/****************************************************************/
/* 																*/
/* fetchInstrunction -	returns the instruction that is 		*/
/*						currently pointed to by the current PC	*/
/*																*/
/* Output	: The integer instruction							*/
/*																*/
/****************************************************************/
int fetchInstrunction() {
	int currentPC = CURRENT_LATCHES.PC;
	int currentInstruction = loadWord(currentPC);
	return currentInstruction;
}

/****************************************************************/
/* 																*/
/* decodeOpcode - decodes the instruction's opcode 				*/
/*																*/
/* Input	: The instruction to decode 						*/
/* Output	: The integer opcode 								*/
/*																*/
/****************************************************************/
int decodeOpcode(int instruction) {
	const int OPCODE_MASK = 0x0000F000;
	return (instruction & OPCODE_MASK) >> 12;
}

/****************************************************************/
/* 																*/
/* execute - 	executes the instruction, altering the latch 	*/
/*				states and preincrementing the PC. Delegates	*/
/*				execution to an execute function specifically	*/
/*				written for the given instruction 				*/
/*																*/
/* Input	: The integer opcode and instruction to execute		*/
/*																*/
/****************************************************************/
void execute(int opcode, int instruction) {

	/* preincrement PC */
	CURRENT_LATCHES.PC += 2;
	NEXT_LATCHES.PC = CURRENT_LATCHES.PC;

	/* delegate execution to correct funtion */
	switch(opcode) {

		case 0: /* BR */
			executeBR(opcode, instruction);
		break;

		case 1: /* ADD */
			executeADD(opcode, instruction);
		break;
				
		case 2: /* LDB */
			executeLDB(opcode, instruction);
		break;
		
		case 3: /* STB */
			executeSTB(opcode, instruction);
		break;
		
		case 4: /* JSR (& JSRR) */
			executeJSR(opcode, instruction);
		break;
		
		case 5: /* AND */
			executeAND(opcode, instruction);
		break;
		
		case 6: /* LDW */
			executeLDW(opcode, instruction);
		break;
		
		case 7: /* STW */
			executeSTW(opcode, instruction);
		break;
		
		case 8: /* RTI */
			executeRTI(opcode, instruction);
		break;
		
		case 9: /* XOR (& NOT) */
			executeXOR(opcode, instruction);
		break;
		
		case 12: /* JMP (& RET) */
			executeJMP(opcode, instruction);
		break;
		
		case 13: /* SHF */
			executeSHF(opcode, instruction);
		break;
		
		case 14: /* LEA */
			executeLEA(opcode, instruction);
		break;
		
		case 15: /* TRAP */
			executeTRAP(opcode, instruction);
		break;

		default:
			terminate();
			printf("ERROR: Opcode 0x%.1x not in use\n", opcode);
		break;
	}
}

/****************************************************************/
/* executeADD													*/
/****************************************************************/
void executeADD(int opcode, int instruction) {
	/*
		if (bit[5] == 0)
			DR = SR1 + SR2;
		else
			DR = SR1 + SEXT(imm5);
		setcc();
	*/
	executeLogicOperation(opcode, instruction);
}

/****************************************************************/
/* executeAND													*/
/****************************************************************/
void executeAND(int opcode, int instruction) {
	/*
		if (bit[5] == 0)
			DR = SR1 AND SR2;
		else
			DR = SR1 AND SEXT(imm5);
		setcc();
	*/
	executeLogicOperation(opcode, instruction);
}

/****************************************************************/
/* executeBR													*/
/****************************************************************/
void executeBR(int opcode, int instruction) {
	/*
		if ((n AND N) OR (z AND Z) OR (p AND P))
			PC = PC + LSHF(SEXT(PCoffset9), 1);
	*/
	const int n_MASK = 1 << 11;
	const int z_MASK = 1 << 10;
	const int p_MASK = 1 << 9;
	const int PCoffset9_MASK = 0x01FF;
	int n = n_MASK & instruction;
	int z = z_MASK & instruction;
	int p = p_MASK & instruction;

	if ( ( n && CURRENT_LATCHES.N )  ||  ( z && CURRENT_LATCHES.Z )  ||  ( p && CURRENT_LATCHES.P ) ) {
		NEXT_LATCHES.PC = CURRENT_LATCHES.PC + ( signExtend(PCoffset9_MASK & instruction, 9) << 1 );
	}
}

/****************************************************************/
/* executeJMP													*/
/****************************************************************/
void executeJMP(int opcode, int instruction) {
	/*
		PC <- BaseR
	*/
	const int BaseR_MASK = 0x01C0;
	int baseR = (BaseR_MASK & instruction) >> 6;
	int regVal = getRegisterValue(baseR);
	NEXT_LATCHES.PC = regVal;
}

/****************************************************************/
/* executeJSR													*/
/****************************************************************/
void executeJSR(int opcode, int instruction) {
	/*
		R7 = PC;
		if (bit[11] == 0)
			PC = BaseR;
		else
			PC = PC + LSHF(SEXT(PCoffset11), 1);
	*/
	const int BIT_MASK = 0x0800;
	const int BaseR_MASK = 0x01C0;
	const int PCoffset11_MASK = 0x07FF;
	setRegisterValue(7, CURRENT_LATCHES.PC);
	if ((BIT_MASK & instruction)  ==  0) {
		int baseR = (BaseR_MASK & instruction) >> 6;
		NEXT_LATCHES.PC = getRegisterValue(baseR);
	} else {
		NEXT_LATCHES.PC = CURRENT_LATCHES.PC + (signExtend(instruction & PCoffset11_MASK, 11) << 1);
	}
}

/****************************************************************/
/* executeLDB													*/
/****************************************************************/
void executeLDB(int opcode, int instruction) {
  /*
    DR = SEXT(mem[BaseR + SEXT(boffset6)]);
    setcc();
  */
  const int DR_MASK = 0x0E00;
  const int BaseR_MASK = 0x01C0;
  const int boffset6_MASK = 0x003F;

  int dr = (instruction & DR_MASK) >> 9;
  int baseR = (instruction & BaseR_MASK) >> 6;
  int boffset6 = (instruction & boffset6_MASK);

  int result = signExtend( loadByte( getRegisterValue(baseR) + signExtend(boffset6, 6) ), 8 );
  setRegisterValue(dr, result);

  setConditionCodes(result);
}

/****************************************************************/
/* executeLDW													*/
/****************************************************************/
void executeLDW(int opcode, int instruction) {
	/*
		DR = MEM[BaseR + LSHF(SEXT(offset6), 1)];
		setcc();
	*/
	const int DR_MASK = 0x0E00;
	const int BaseR_MASK = 0x01C0;
	const int offset6_MASK = 0x003F;

	int dr = (instruction & DR_MASK) >> 9;
	int baseR = (instruction & BaseR_MASK) >> 6;
	int offset6 = (instruction & offset6_MASK);

	int result = loadWord( getRegisterValue(baseR) + (signExtend(offset6, 6) << 1) );
	setRegisterValue(dr, result);

	setConditionCodes(result);
}

/****************************************************************/
/* executeLEA													*/
/*	- should not set condition codes, as per Lab2's instructions*/
/****************************************************************/
void executeLEA(int opcode, int instruction) {
	/*
		DR = PC + LSHF(SEXT(PCoffset9),1);
	*/
	const int DR_MASK = 0x0E00;
	const int PCoffset11_MASK = 0x01FF;

	int result = CURRENT_LATCHES.PC + ( signExtend(PCoffset11_MASK & instruction, 9) << 1 );

	int dr = (DR_MASK & instruction) >> 9;
	setRegisterValue(dr, result);
}

/****************************************************************/
/* executeRTI													*/
/****************************************************************/
void executeRTI(int opcode, int instruction) {
	/* we do not need to implement RTI for this lab */
}

/****************************************************************/
/* executeSHF													*/
/*	- from what I found on the internet, right shift in C is 	*/
/*	ambiguous as to whether it is arithmetic or logical, so I 	*/
/*	ensured that the shifts were performed correctly by adding 	*/
/*	additional code 											*/
/****************************************************************/
void executeSHF(int opcode, int instruction) {
	/*
		if (bit[4] == 0)
			DR = LSHF(SR, amount4);
		else
			if (bit[5] == 0)
				DR = RSHF(SR, amount4, 0);
			else
				DR = RSHF(SR, amount4, SR[15]);
		setcc();
	*/
	const int DIRECTION_BIT_MASK = 0x0010;
	const int TYPE_BIT_MASK = 0x0020;
	const int DR_MASK = 0x0E00;
	const int SR_MASK = 0x01C0;
	const int amount4_MASK = 0x000F;

	int dr = (DR_MASK & instruction) >> 9;
	int sr = (SR_MASK & instruction) >> 6;
	int amount4 = (amount4_MASK & instruction);
	int result;

	if ( !(DIRECTION_BIT_MASK & instruction) ) { /* left shift */
		result = getRegisterValue(sr) << amount4;

	} else { /* right shift */

		if ( !(TYPE_BIT_MASK & instruction) ) { /* logical, 0 */

			/* calculate mask of length amount4 for logical shift */
			int LOGICAL_MASK = 0;
			if (amount4 < 15) {
				LOGICAL_MASK |= (1 << (15 - amount4 - 1));
			}

			result = ( getRegisterValue(sr) >> amount4 )  &  LOGICAL_MASK;

		} else { /* arithmetic, SR[15] */
			const int SR15_MASK = 0x00008000;

			int sr15 = SR15_MASK & getRegisterValue(sr);
			if (sr15) { /* sr15 is a 1 */
				
				/* calculate mask of length amount4 for arithmetic shift */
				int ARITHMETIC_MASK = 1;
				if (amount4 < 15) {
					ARITHMETIC_MASK &= (0xFFF7 << (15 - amount4 -1));
				}

				result = ( getRegisterValue(sr) >> amount4) | ARITHMETIC_MASK;

			} else { /* sr15 is a 0 */

				/* calculate mask of length amount4 for arithmetic shift */
				int ARITHMETIC_MASK = 0;
				if (amount4 < 15) {
					ARITHMETIC_MASK |= (1 << (15 - amount4 - 1));
				}

				result = ( getRegisterValue(sr) >> amount4 )  &  ARITHMETIC_MASK;
			}
		}
	}
	setRegisterValue(dr, result);
	setConditionCodes(result);
}

/****************************************************************/
/* executeSTB													*/
/****************************************************************/
void executeSTB(int opcode, int instruction) {
  /*
    mem[BaseR + SEXT(boffset6)] = SR[7:0];
  */
  const int SR_MASK = 0x0E00;
  const int BaseR_MASK = 0x01C0;
  const int boffset6_MASK = 0x003F;

  int sr = (SR_MASK & instruction) >> 9;
  int baseR = (BaseR_MASK & instruction) >> 6;
  int boffset6 = boffset6_MASK & instruction;

  int address = getRegisterValue(baseR) + signExtend(boffset6, 6);
  storeByte(address,getRegisterValue(sr));
}

/****************************************************************/
/* executeSTW													*/
/****************************************************************/
void executeSTW(int opcode, int instruction) {
	/*
		MEM[BaseR + LSHF(SEXT(offset6), 1)] = SR;
	*/
	const int SR_MASK = 0x0E00;
	const int BaseR_MASK = 0x01C0;
	const int offset6_MASK = 0x003F;

	int sr = (SR_MASK & instruction) >> 9;
	int baseR = (BaseR_MASK & instruction) >> 6;
	int offset6 = offset6_MASK & instruction;

	int address = getRegisterValue(baseR) + (signExtend(offset6, 6) << 1);
	storeWord(address,getRegisterValue(sr));
}

/****************************************************************/
/* executeTRAP													*/
/****************************************************************/
void executeTRAP(int opcode, int instruction) {
	/*
		R7 = PC;
		PC = MEM[LSHF(ZEXT(trapvect8), 1)];
	*/
	const int trapvect8_MASK = 0x00FF;
	int trapvect8 = trapvect8_MASK & instruction; /* this zero extends trapvector8 */

	setRegisterValue(7, CURRENT_LATCHES.PC);
	NEXT_LATCHES.PC = loadWord( trapvect8 << 1 );
}

/****************************************************************/
/* executeXOR													*/
/****************************************************************/
void executeXOR(int opcode, int instruction) {
	/*
		if (bit[5] == 0)
			DR = SR1 XOR SR2;
		else
			DR = SR1 XOR SEXT(imm5);
		setcc();
	*/
	executeLogicOperation(opcode, instruction);
}






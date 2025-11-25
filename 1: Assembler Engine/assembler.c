#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */


int isOpcode(char*);
FILE* infile = NULL;
FILE* outfile = NULL;
int startOfFile = 0x3000;
#define MAX_LINE_LENGTH 255
	enum
	{
	   DONE, OK, EMPTY_LINE
	};


#define MAX_LABEL_LEN 20    
#define MAX_SYMBOLS 255
typedef struct {
	int address;
	char label[MAX_LABEL_LEN + 1];	/* Question for the reader: Why do we need to add 1? */
} TableEntry;
TableEntry symbolTable[MAX_SYMBOLS];





int toNum( char * pStr )
{
   char * t_ptr;
   char * orig_pStr;
   int t_length,k;
   int lNum, lNeg = 0;
   long int lNumLong;

   orig_pStr = pStr;
   if( *pStr == '#' )				/* decimal */
   { 
     pStr++;
     if( *pStr == '-' )				/* dec is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isdigit(*t_ptr))
       {
	 printf("Error: invalid decimal operand, %s\n",orig_pStr);
	 exit(4);
       }
       t_ptr++;
     }
     lNum = atoi(pStr);
     if (lNeg)
       lNum = -lNum;
 
     return lNum;
   }
   else if( *pStr == 'x' )	/* hex     */
   {
     pStr++;
     if( *pStr == '-' )				/* hex is negative */
     {
       lNeg = 1;
       pStr++;
     }
     t_ptr = pStr;
     t_length = strlen(t_ptr);
     for(k=0;k < t_length;k++)
     {
       if (!isxdigit(*t_ptr))
       {
	 printf("Error: invalid hex operand, %s\n",orig_pStr);
	 exit(4);
       }
       t_ptr++;
     }
     lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
     lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
     if( lNeg )
       lNum = -lNum;
     return lNum;
   }
   else
   {
	printf( "Error: invalid operand, %s\n", orig_pStr);
	exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
   }
}


int findLabelIndex(const char *name, int symbolTableCount) {
    if (name == NULL) return -1; 
    for (int i = 0; i < symbolTableCount; i++) {
        if (strcmp(name, symbolTable[i].label) == 0) {
            return i;
        }
    }
    return -1; 
}


    int isOpcode(char* input) {
    if (strcmp(input, "add") == 0) 
        return 0;
    if (strcmp(input, "and") == 0) 
        return 0;
    if (strcmp(input, "br") == 0) 
        return 0;
    if (strcmp(input, "brn") == 0) 
        return 0;
    if (strcmp(input, "brz") == 0) 
        return 0;
    if (strcmp(input, "brp") == 0) 
        return 0;
    if (strcmp(input, "brnz") == 0) 
        return 0;
    if (strcmp(input, "brnp") == 0) 
        return 0;
    if (strcmp(input, "brzp") == 0) 
        return 0;
    if (strcmp(input, "brnzp") == 0) 
        return 0;
    if (strcmp(input, "halt") == 0) 
        return 0;
    if (strcmp(input, "jmp") == 0) 
        return 0;
    if (strcmp(input, "jsr") == 0) 
        return 0;
    if (strcmp(input, "jsrr") == 0) 
        return 0;
    if (strcmp(input, "ldb") == 0) 
        return 0;
    if (strcmp(input, "ldw") == 0) 
        return 0;
    if (strcmp(input, "lea") == 0) 
        return 0;
    if (strcmp(input, "nop") == 0) 
        return 0;
    if (strcmp(input, "not") == 0) 
        return 0;
    if (strcmp(input, "ret") == 0) 
        return 0;
    if (strcmp(input, "lshf") == 0) 
        return 0;
    if (strcmp(input, "rshfl") == 0) 
        return 0;
    if (strcmp(input, "rshfa") == 0) 
        return 0;
    if (strcmp(input, "rti") == 0) 
        return 0;
    if (strcmp(input, "stb") == 0) 
        return 0;
    if (strcmp(input, "stw") == 0) 
        return 0;
    if (strcmp(input, "trap") == 0) 
        return 0;
    if (strcmp(input, "xor") == 0) 
        return 0;

    return -1;
}


	int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char
	** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4
	)
	{
	   char * lRet, * lPtr;
	   int i;
	   if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )  //read one line and put into pLine;
		return( DONE );
	   for( i = 0; i < strlen( pLine ); i++ ) //covert everything to lowercase
		pLine[i] = tolower( pLine[i] );
	   
           /* convert entire line to lowercase */
	   *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

	   /* ignore the comments */
	   lPtr = pLine;

	   while( *lPtr != ';' && *lPtr != '\0' &&
	   *lPtr != '\n' )  //removes comments
		lPtr++;

	   *lPtr = '\0';
       
	   if( !(lPtr = strtok( pLine, "\t\n ," ) ) )  //Split each word with \n 
		return( EMPTY_LINE );

	   if( isOpcode(lPtr) == -1 && lPtr[0] != '.' ) {//If lPtr is not a known opcode 
		*pLabel = lPtr;                              //And does not start with dots like .FILL
	                                                 //Then it must be a label stores in *pLabel
		if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	   }
       
           *pOpcode = lPtr;  //put opcode into *pOpcode

	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	   
           *pArg1 = lPtr;   //*pArg1 argument 1
	   
           if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	   *pArg2 = lPtr; //*pArg1 argument 2
	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	   *pArg3 = lPtr; //*pArg1 argument 3

	   if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

	   *pArg4 = lPtr; //*pArg1 argument 4

	   return( OK );
	}

	/* Note: MAX_LINE_LENGTH, OK, EMPTY_LINE, and DONE are defined values */










//# of arguments, and argument list
int main(int argc, char* argv[]) {
///////////////////////Parsing Command Line Arguments/////////////////////////////
//assembler.exe input.txt output.txt

//what happens when you run this program without any command line arguments?  FIXED??
    if (argc < 3) {
        printf("incorrect input into command line");
        return 1;
    }

     char *prgName   = NULL;
     char *iFileName = NULL;
     char *oFileName = NULL;

     prgName   = argv[0]; 
     iFileName = argv[1]; //input file
     oFileName = argv[2]; //output file

     printf("program name = '%s'\n", prgName);
     printf("input file name = '%s'\n", iFileName);
     printf("output file name = '%s'\n", oFileName);    

///////////////////////////////////////////////////////////////////////////////////////







    //////////////////Opening And Closing Files/////////////////////////////////////////
     infile = fopen(argv[1], "r");   //r is reading
     outfile = fopen(argv[2], "w");  //w is writing
		 
     if (!infile) {
       printf("Error: Cannot open file %s\n", argv[1]);
       exit(4);
		 }
     if (!outfile) {
       printf("Error: Cannot open file %s\n", argv[2]);
       exit(4);
     }

    /////////////To call readAndParse, you would use the following:////////////////

     char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;  //init variables
     
	   int lRet;  //stores return value from readandParse: DONE, OK, EMPTY_LINE

       int iOpcode;
       int iArg1;
       int iArg2;
       int iArg3;
       int iArg4;
       int finalVal;
       int lineCount = 0;
       int symbolTableCount = 0;
       
	   FILE * lInfile;

	   //////lInfile = fopen( "data.in", "r" );	/* open the input file */
       lInfile = infile;        /////ASK JAY ABOUT THIS

    
    //////MAKE SYMBOL TABLE
     do
	   {
		lRet = readAndParse( lInfile, lLine, &lLabel,
			&lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
            
		if( lRet != DONE && lRet != EMPTY_LINE )
		{
        if (strcmp(lOpcode, ".orig") == 0) {
            startOfFile = toNum(lArg1);           //just getting the starting address to now calculate symbol table   
            }

            
static int currentAddress = 0;
if (strcmp(lOpcode, ".orig") == 0) {
    currentAddress = toNum(lArg1);
} else if (strcmp(lOpcode, ".end") == 0) {
    /* nothing */
} else if (strcmp(lOpcode, ".fill") == 0 || isOpcode(lOpcode) == 0) {
    // record label if present
    if (lLabel != NULL && strlen(lLabel) > 0) {
        symbolTable[symbolTableCount].address = currentAddress;
        for (int j = 0; j < (int)strlen(lLabel); j++) {
            symbolTable[symbolTableCount].label[j] = tolower(lLabel[j]);
        }
        symbolTable[symbolTableCount].label[strlen(lLabel)] = '\0';
        symbolTableCount++;
    }
    currentAddress += 1; // .fill and instructions emit 1 word
}

		}
	   } while( lRet != DONE );
	

        rewind(infile);
        lInfile = infile;  
       //SECOND LOOP
       lineCount = 0;
	   do
	   {
		lRet = readAndParse( lInfile, lLine, &lLabel,
			&lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
		if( lRet != DONE && lRet != EMPTY_LINE )
		{
            //.ORIG Function
                     if (strcmp(lOpcode, ".orig") == 0) {
                startOfFile = toNum(lArg1);
                fprintf(outfile, "0x%04X\n", startOfFile);      
            }
            //.FILL
            else if (strcmp(lOpcode, ".fill") == 0) {
                if (lArg1 == NULL || lArg1[0] == '\0') {
                    fprintf(stderr, "Error: .FILL missing operand\n");
                    exit(4);
                }

                if (lArg1[0] == 'x' || lArg1[0] == 'X' || lArg1[0] == '#') {
                    finalVal = toNum(lArg1) & 0xFFFF;
                } else {
                    int idx = findLabelIndex(lArg1, symbolTableCount);
                    if (idx < 0) {
                        fprintf(stderr, "Error: undefined label '%s' in .FILL\n", lArg1);
                        exit(4);
                    }
                    finalVal = symbolTable[idx].address & 0xFFFF;
                }
                fprintf(outfile, "0x%04X\n", finalVal);
                lineCount++;   /* .FILL emits a word */
            }

            //ADD Function 
            else if (strcmp(lOpcode, "add") == 0) {
                iOpcode = 1;
                iArg1 = lArg1[1]-'0';   
                iArg2 = lArg2[1]-'0';
                if (lArg3[0] == 'r') {
                    iArg3 = lArg3[1]-'0';
                    finalVal = (iOpcode << 12) | (iArg1 << 9) | (iArg2 << 6) | (0 << 3) | iArg3;
                }
                else {
                    iArg3 = toNum(lArg3);
                    finalVal = (iOpcode << 12) | (iArg1 << 9) | (iArg2 << 6) | (1 << 5) | (iArg3 & 0x1F);
                }
                fprintf(outfile, "0x%04X\n", finalVal);
                lineCount++;   /* instruction emits a word */
            }


            //AND Function
            else if(strcmp(lOpcode, "and") == 0) {
                iOpcode = 5;
                iArg1 = lArg1[1]-'0';
                iArg2 = lArg2[1]-'0';
                if (lArg3[0] == 'r') {
                    iArg3 = lArg3[1]-'0';
                    finalVal = 0;
                    finalVal = (iOpcode << 12) | (iArg1 << 9) | (iArg2 << 6) | (0 << 3) | iArg3;
                }
                else {
                    iArg3 = toNum(lArg3);
                    finalVal = (iOpcode << 12) | (iArg1 << 9) | (iArg2 << 6) | (1 << 5) | (iArg3 & 0x1F);
                }
                fprintf(outfile, "0x%04X\n", finalVal); 
                lineCount++; 
            }
            //BR Function
            else if(strcmp(lOpcode, "br") == 0){ 
                iOpcode = 0;
                int i = 0;
                iArg1 = 7;
                printf("label");
                printf("%s", lLabel);
                printf("symboltable");
                printf("%s", symbolTable[i].label);
                while(strcmp(lArg1, symbolTable[i].label) != 0) {
                    i++; //find the correct index 
                }
                
                int currentAddr = startOfFile + lineCount;   
                int offset = symbolTable[i].address - (currentAddr + 1); 


                iArg2 = offset & 0x1FF; 
                //iArg2 = (symbolTable[i].address - startOfFile) - lineCount; ///I WANT TO MAKE THE OFFSET VALUE THE END
                printf("%d", iArg2);
                finalVal = (iOpcode << 12) | (iArg1 << 9) | iArg2;
                fprintf(outfile, "0x%04X\n", finalVal & 0xFFFF);
                lineCount++; 


            }
            else if(strcmp(lOpcode, "brnzp") == 0){ 
                iOpcode = 0;
                int i = 0;
                iArg1 = 7;
                printf("label");
                printf("%s", lLabel);
                printf("symboltable");
                printf("%s", symbolTable[i].label);
                while(strcmp(lArg1, symbolTable[i].label) != 0) {
                    i++; //find the correct index 
                }
                int currentAddr = startOfFile + lineCount;   
                int offset = symbolTable[i].address - (currentAddr + 1);

                iArg2 = offset & 0x1FF; 
                //iArg2 = (symbolTable[i].address - startOfFile) - lineCount; ///I WANT TO MAKE THE OFFSET VALUE THE END
                printf("%d", iArg2);
                finalVal = (iOpcode << 12) | (iArg1 << 9) | iArg2;
                fprintf(outfile, "0x%04X\n", finalVal & 0xFFFF);
                lineCount++; 
            }
            else if(strcmp(lOpcode, "brn") == 0){ 
                iOpcode = 0;
                int i = 0;
                iArg1 = 0x4;
                printf("label");
                printf("%s", lLabel);
                printf("symboltable");
                printf("%s", symbolTable[i].label);
                while(strcmp(lArg1, symbolTable[i].label) != 0) {
                    i++; //find the correct index 
                }
                int currentAddr = startOfFile + lineCount;   
                int offset = symbolTable[i].address - (currentAddr + 1); 

                iArg2 = offset & 0x1FF; 
                //iArg2 = (symbolTable[i].address - startOfFile) - lineCount; ///I WANT TO MAKE THE OFFSET VALUE THE END
                printf("%d", iArg2);
                        
                finalVal = (iOpcode << 12) | (iArg1 << 9) | iArg2;
                fprintf(outfile, "0x%04X\n", finalVal & 0xFFFF);
                lineCount++; 
            }
            else if(strcmp(lOpcode, "brz") == 0){ 
                iOpcode = 0;
                int i = 0;
                iArg1 = 0x2;
                printf("label");
                printf("%s", lLabel);
                printf("symboltable");
                printf("%s", symbolTable[i].label);
                while(strcmp(lArg1, symbolTable[i].label) != 0) {
                    i++; //find the correct index 
                }
                int currentAddr = startOfFile + lineCount;   
                int offset = symbolTable[i].address - (currentAddr + 1); 

/* then mask to the appropriate bits, for branch (9-bit signed): */
                iArg2 = offset & 0x1FF; 
                //iArg2 = (symbolTable[i].address - startOfFile) - lineCount; ///I WANT TO MAKE THE OFFSET VALUE THE END
                printf("%d", iArg2);
                finalVal = (iOpcode << 12) | (iArg1 << 9) | iArg2;
                fprintf(outfile, "0x%04X\n", finalVal & 0xFFFF);
                lineCount++; 
            }
            else if(strcmp(lOpcode, "brp") == 0){ 
                iOpcode = 0;
                int i = 0;
                iArg1 = 0x1;
                printf("label");
                printf("%s", lLabel);
                printf("symboltable");
                printf("%s", symbolTable[i].label);
                while(strcmp(lArg1, symbolTable[i].label) != 0) {
                    i++; //find the correct index 
                }
                int currentAddr = startOfFile + lineCount;   
                int offset = symbolTable[i].address - (currentAddr + 1); 


                iArg2 = offset & 0x1FF; 
                //iArg2 = (symbolTable[i].address - startOfFile) - lineCount; ///I WANT TO MAKE THE OFFSET VALUE THE END
                printf("%d", iArg2);
                finalVal = (iOpcode << 12) | (iArg1 << 9) | iArg2;
                fprintf(outfile, "0x%04X\n", finalVal & 0xFFFF);
                lineCount++; 
            }
            else if(strcmp(lOpcode, "brnz") == 0){ 
                iOpcode = 0;
                int i = 0;
                iArg1 = 0x6;
                printf("label");
                printf("%s", lLabel);
                printf("symboltable");
                printf("%s", symbolTable[i].label);
                while(strcmp(lArg1, symbolTable[i].label) != 0) {
                    i++; //find the correct index 
                }
                int currentAddr = startOfFile + lineCount;  
                int offset = symbolTable[i].address - (currentAddr + 1); 


                iArg2 = offset & 0x1FF; 
                //iArg2 = (symbolTable[i].address - startOfFile) - lineCount; ///I WANT TO MAKE THE OFFSET VALUE THE END
                printf("%d", iArg2);
                finalVal = (iOpcode << 12) | (iArg1 << 9) | iArg2;
                fprintf(outfile, "0x%04X\n", finalVal & 0xFFFF);
                lineCount++; 
            }
            else if(strcmp(lOpcode, "brnp") == 0){ 
                iOpcode = 0;
                int i = 0;
                iArg1 = 0x5;
                printf("label");
                printf("%s", lLabel);
                printf("symboltable");
                printf("%s", symbolTable[i].label);
                while(strcmp(lArg1, symbolTable[i].label) != 0) {
                    i++; //find the correct index 
                }
                int currentAddr = startOfFile + lineCount;   
                int offset = symbolTable[i].address - (currentAddr + 1); 

                iArg2 = offset & 0x1FF;   
                //iArg2 = (symbolTable[i].address - startOfFile) - lineCount; ///I WANT TO MAKE THE OFFSET VALUE THE END
                printf("%d", iArg2);
                finalVal = (iOpcode << 12) | (iArg1 << 9) | iArg2;
                fprintf(outfile, "0x%04X\n", finalVal & 0xFFFF);
                lineCount++; 
            }
            else if(strcmp(lOpcode, "brzp") == 0){ 
                iOpcode = 0;
                int i = 0;
                iArg1 = 0x3;
                printf("label: ");
                printf("%s", lLabel);
                printf(": symboltable: ");
                printf("%s", symbolTable[i].label);
                while(strcmp(lArg1, symbolTable[i].label) != 0) {
                    i++; //find the correct index 
                }
                int currentAddr = startOfFile + lineCount;  
                int offset = symbolTable[i].address - (currentAddr + 1); 


                iArg2 = offset & 0x1FF; 
                //iArg2 = (symbolTable[i].address - startOfFile) - lineCount; ///I WANT TO MAKE THE OFFSET VALUE THE END
                printf("%d", iArg2);
                finalVal = (iOpcode << 12) | (iArg1 << 9) | iArg2;
                fprintf(outfile, "0x%04X\n", finalVal & 0xFFFF);
                lineCount++; 
            }



            //JMP Instruction
            else if(strcmp(lOpcode, "jmp") == 0) {
                printf("\n");
                printf("\n");
                printf("\n");
                iOpcode = 12;
                iArg1 = lArg1[1]-'0'; //BaseR   lArg1[0] R   lArg1[1] 0  
                iArg2 = 0;
                iArg3 = 0;
                finalVal = (iOpcode << 12) | (iArg2 << 9) | (iArg1 << 6) | (0 << 5) | iArg3;
                printf("%d %d %d %d", iOpcode, iArg1, iArg2, iArg3);
                fprintf(outfile, "0x%04X\n", finalVal & 0xFFFF);
                lineCount++; 
                printf("\n");
                printf("\n");
                printf("\n");

            }

            else if(strcmp(lOpcode, "jsr") == 0) {
                iOpcode = 0x4;
                iArg1= 1;
                int i = 0;
                while(strcmp(lArg1, symbolTable[i].label) != 0) {
                    i++; //find the correct index 
                }
                int currentAddr = startOfFile + lineCount;   
                int offset = symbolTable[i].address - (currentAddr + 1);


                iArg2 = offset & 0x7FF; 
                finalVal = (iOpcode << 12) | (iArg1 << 11) | (iArg2 & 0x7FF);   //UNDERSTAND WHY YOU OFFSET LIKE THIS DOES BR AND JSR NEED TO ALSO TAKE IN NONLABELS
                fprintf(outfile, "0x%04X\n", finalVal & 0xFFFF);
                lineCount++; 
            }

            else if(strcmp(lOpcode, "jsrr") == 0) {
                iOpcode = 0x4;              // 0100
                iArg1 = lArg1[1] - '0';     // BaseR
                finalVal = (iOpcode << 12) | (0 << 11) | (iArg1 << 6);
                fprintf(outfile, "0x%04X\n", finalVal & 0xFFFF); 
                lineCount++; 
            }

             else if(strcmp(lOpcode, "ldb") == 0) {
                 iOpcode = 0x2;                 // 0010
                iArg1 = lArg1[1] - '0';        // DR
                iArg2 = lArg2[1] - '0';        // BaseR
                iArg3 = toNum(lArg3);          // boffset6 ASK ABOUT THIS

                finalVal = (iOpcode << 12) | (iArg1 << 9) | (iArg2 << 6) | (iArg3 & 0x3F);
                fprintf(outfile, "0x%04X\n", finalVal & 0xFFFF);
                lineCount++; 
            }

            else if(strcmp(lOpcode, "ldw") == 0) {
                iOpcode = 0x6;                 // 0110
                iArg1 = lArg1[1] - '0';        // DR
                iArg2 = lArg2[1] - '0';        // BaseR
                iArg3 = toNum(lArg3);          // offset6 ASK ABOUT THIS

                finalVal = (iOpcode << 12) | (iArg1 << 9) | (iArg2 << 6) | (iArg3 & 0x3F);
                fprintf(outfile, "0x%04X\n", finalVal & 0xFFFF);
                lineCount++; 
        }
        
            else if(strcmp(lOpcode, "lea") == 0){ 
                iOpcode = 14;
                int i = 0;
                iArg1 = lArg1[1]-'0';
                printf("label: "); 
                printf("%s", lLabel);
                printf(": symboltable: ");
                printf("%s", symbolTable[i].label);
                while(strcmp(lArg2, symbolTable[i].label) != 0) {
                    i++; //find the correct index 
                }
                int currentAddr = startOfFile + lineCount;   
                int offset = symbolTable[i].address - (currentAddr + 1); 


                iArg2 = offset & 0x1FF;  
                //iArg2 = (symbolTable[i].address - startOfFile) - lineCount; ///I WANT TO MAKE THE OFFSET VALUE THE END
                printf("%d", iArg2);
                finalVal = (iOpcode << 12) | (iArg1 << 9) | iArg2;
                fprintf(outfile, "0x%04X\n", finalVal & 0xFFFF);
                lineCount++; 
            }

            else if(strcmp(lOpcode, "not") == 0) {
                iOpcode = 0x9;                 // 1001
                iArg1 = lArg1[1] - '0';        // DR
                iArg2 = lArg2[1] - '0';        // SR
                iArg3 = 0x3F;
                finalVal = (iOpcode << 12) | (iArg1 << 9) | (iArg2 << 6) | (iArg3 & 0x3F);
                fprintf(outfile, "0x%04X\n", finalVal & 0xFFFF);
                lineCount++; 
        }
            else if(strcmp(lOpcode, "ret") == 0) {
                iOpcode = 0xC;        // 1100
                iArg1 = 0;            // 000
                iArg2 = 7;            // 111
                iArg3 = 0;            // 000000

                finalVal = (iOpcode << 12) | (iArg1 << 9) | (iArg2 << 6) | (iArg3 & 0x3F);
                fprintf(outfile, "0x%04X\n", finalVal & 0xFFFF);
                lineCount++; 
        }
            else if(strcmp(lOpcode, "rti") == 0) {
                iOpcode = 8;        // 1000
                iArg1 = 0;            // 000000000000
                

                finalVal = (iOpcode << 12) | (iArg1 << 11);
                fprintf(outfile, "0x%04X\n", finalVal & 0xFFFF);
                lineCount++; 
        }
            else if(strcmp(lOpcode, "lshf") == 0) {
                iOpcode = 13;                // 1101
                iArg1 = lArg1[1] - '0';        // DR
                iArg2 = lArg2[1] - '0';        // SR
                iArg3 = 0;          // 00
                iArg4 = toNum(lArg3); //numerical offset like ldw

                finalVal = (iOpcode << 12) | (iArg1 << 9) | (iArg2 << 6) | (iArg3 << 4) | (iArg4 & 0xF);
                fprintf(outfile, "0x%04X\n", finalVal & 0xFFFF);
                lineCount++; 
        }
            else if(strcmp(lOpcode, "rshfl") == 0) {
                iOpcode = 13;                 // 1101
                iArg1 = lArg1[1] - '0';        // DR
                iArg2 = lArg2[1] - '0';        // SR
                iArg3 = 1;          // 01
                iArg4 = toNum(lArg3); //numerical offset like ldw

                finalVal = (iOpcode << 12) | (iArg1 << 9) | (iArg2 << 6) | (iArg3 << 4) | (iArg4 & 0xF);
                fprintf(outfile, "0x%04X\n", finalVal & 0xFFFF);
                lineCount++; 
        }
            else if(strcmp(lOpcode, "rshfa") == 0) {
                iOpcode = 13;                 // 1101
                iArg1 = lArg1[1] - '0';        // DR
                iArg2 = lArg2[1] - '0';        // SR
                iArg3 = 3;          // 11
                iArg4 = toNum(lArg3); //numerical offset like ldw

                finalVal = (iOpcode << 12) | (iArg1 << 9) | (iArg2 << 6) | (iArg3 << 4) | (iArg4 & 0xF);
                fprintf(outfile, "0x%04X\n", finalVal & 0xFFFF);
                lineCount++; 
        }
            else if(strcmp(lOpcode, "stb") == 0) {
                iOpcode = 3;                 // 0011
                iArg1 = lArg1[1] - '0';        // SR
                iArg2 = lArg2[1] - '0';        // BASER
                iArg3 = toNum(lArg3);          // boffset6 ASK ABOUT THIS
                
                finalVal = (iOpcode << 12) | (iArg1 << 9) | (iArg2 << 6) | (iArg3 & 0x3F);
                fprintf(outfile, "0x%04X\n", finalVal & 0xFFFF);
                lineCount++; 
        }
            else if(strcmp(lOpcode, "stw") == 0) {
                iOpcode = 7;                 // 0111
                iArg1 = lArg1[1] - '0';        // SR
                iArg2 = lArg2[1] - '0';        // BaseR
                iArg3 = toNum(lArg3);          // offset6 ASK ABOUT THIS

                finalVal = (iOpcode << 12) | (iArg1 << 9) | (iArg2 << 6) | (iArg3 & 0x3F);
                fprintf(outfile, "0x%04X\n", finalVal & 0xFFFF);
                lineCount++; 
        }
            else if((strcmp(lOpcode, "halt") == 0) || (strcmp(lOpcode, "trap") == 0)) {
                iOpcode = 0xF;                 // 0111
                iArg1 = 0000;        //0000
                iArg2 = 0x25;      // x25

                finalVal = (iOpcode << 12) | (iArg2 & 0xFF);
                fprintf(outfile, "0x%04X\n", finalVal & 0xFFFF);
                lineCount++; 
        }
            else if (strcmp(lOpcode, "xor") == 0) {
                iOpcode = 9; //1001
                iArg1 = lArg1[1]-'0';
                iArg2 = lArg2[1]-'0';
                if (lArg3[0] == 'r') {
                    iArg3 = lArg3[1]-'0';
                    finalVal = 0;
                    finalVal = (iOpcode << 12) | (iArg1 << 9) | (iArg2 << 6) | (0 << 3) | iArg3;
                }
                else {
                    iArg3 = toNum(lArg3);
                    finalVal = (iOpcode << 12) | (iArg1 << 9) | (iArg2 << 6) | (1 << 5) | (iArg3 & 0x1F);
                }
                fprintf(outfile, "0x%04X\n", finalVal);
                lineCount++; 
            }
            else if (strcmp(lOpcode, "nop") == 0) {
                iOpcode = 0; //1001
                    finalVal = 0;
                fprintf(outfile, "0x%04X\n", finalVal);
                lineCount++; 
            }
////////////ALL NEGATUVE NUMS NEED MASKS

    }
	
	   } while( lRet != DONE );
    //////////////////////////////////////////////////////////////////////




  

    fclose(infile);
    fclose(outfile);
	}









     //////////////////////////////////////////////////////////////////////////////


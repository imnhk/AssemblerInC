#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_INSTRUCTION_LEN 50

#pragma warning(disable:4996)

/*******************************************************
 * Function Declaration
 *
 *******************************************************/
char *change_file_ext(char *str);

const char* int2bin5(int num);
const char* int2bin16(int num);

/*******************************************************
 * Function: main
 *
 * Parameters:
 *  int
 *      argc: the number of argument
 *  char*
 *      argv[]: array of a sting argument
 *
 * Return:
 *  return success exit value
 *
 * Info:
 *  The typical main function in C language.
 *  It reads system arguments from terminal (or commands)
 *  and parse an assembly file(*.s).
 *  Then, it converts a certain instruction into
 *  object code which is basically binary code.
 *
 *******************************************************/
int
main(int argc, char *argv[])
{
	FILE *input, *output;
	char *filename;

	input = fopen("example1.s", "r");
	if (input == NULL){
		perror("ERROR");
		exit(EXIT_FAILURE);
	}

	filename = strdup("output_test.o"); // strdup() is not a standard C library but fairy used a lot.

	char instructionLine[MAX_INSTRUCTION_LEN];
	char *token = NULL;
	char *op1, *op2, *op3;
	int counter;
	int totalDataCount = 0, totalTextCount = 0;
	const int dataStartAddress = 4096; // 0001000000000000(2)
	int dataCount = 0;

	char binaryLine[] = "00000000000000000000000000000000"; // 32bit, temp

	while (fgets(instructionLine, MAX_INSTRUCTION_LEN, input) != NULL){

		// Skip until find .text
		token = strtok(instructionLine, "\n\t");
		totalDataCount++;
		if (strcmp(token, ".text") == 0) {
			totalDataCount -= 2; // exclude ".data" and ".text" line
			break;
		}
	}

	printf("Data: %d\n", totalDataCount);

	while (fgets(instructionLine, MAX_INSTRUCTION_LEN, input) != NULL){
		
		token = strtok(instructionLine, "\t\n, ");
		totalTextCount++;

		while (token){

			printf("[%s]\n", token);

			if (strcmp(token, "and") == 0) {
				// Instruction AND, format R
				// op3 <- op1 & op2
				op1 = strtok(NULL, "$\n, "); // rs
				op2 = strtok(NULL, "$\n, "); // rt
				op3 = strtok(NULL, "$\n, "); // rd

				// opcode of AND is 000000
				strcpy(binaryLine, "000000");

				// order is changed!
				strcat(binaryLine, int2bin5(atoi(op2)));
				strcat(binaryLine, int2bin5(atoi(op3)));
				strcat(binaryLine, int2bin5(atoi(op1)));

				// shamt of AND os 00000
				strcat(binaryLine, "00000");
				// funct of AND is 100100
				strcat(binaryLine, "000000");

				printf("and: %s\n", binaryLine);
			}
			else if (strcmp(token, "andi") == 0) {		
				// Instruction ANDI, format I
				// op2 = op1 & immediate(ZeroExtImm???)

				//printf("andi: %s %s %s\n", op1, op2, op3);
			}
			else if (strcmp(token, "or") == 0) {		
				// Instruction OR, format R
				// op3 = op1 | op2

				//printf("or: %s %s %s\n", op1, op2, op3);
			}
			else if (strcmp(token, "ori") == 0) {		
				// Instruction ORI, format I
				// op2 = op1 | immediate(ZeroExtImm???)
				
				//printf("andi: %s %s %s\n", op1, op2, op3);
			}
			else if (strcmp(token, "nor") == 0) {		
				// Instruction NOR, format R
				// op3 = ~(op1 | op2)
			}
			else if (strcmp(token, "la") == 0) {		
				// Instruction LA, format PSEUDO

				op1 = strtok(NULL, "$\n, "); // register
				op2 = strtok(NULL, "$\n, "); // label

				// opcode of LUI is 001111
				strcpy(binaryLine, "001111");
				// rs of LUI is empty
				strcat(binaryLine, "00000");

				// address of register to load
				strcat(binaryLine, int2bin5(atoi(op1)));

				// save data to register
				strcat(binaryLine, int2bin16(dataStartAddress));
				printf("lui: %s\n", binaryLine);

				if (dataCount != 0) {
					// use ORI insturction. opcode of ORI is 001101
					strcpy(binaryLine, "001101");
					strcat(binaryLine, int2bin5(atoi(op1)));
					strcat(binaryLine, int2bin5(atoi(op1)));
					strcat(binaryLine, int2bin16(dataCount*4));
					printf("ori: %s\n", binaryLine);
				}
				dataCount++;

			}
			else if (strcmp(token, "lw") == 0) {		// Instruction ???, format ?
				// TODO load word
			}
			else if (strcmp(token, "sw") == 0) {		// Instruction ???, format ?
				// TODO store word
			}
			else if (strcmp(token, "addiu") == 0) {		// Instruction ???, format ?
				// TODO add imm. unsigned

			}
			else if (strcmp(token, "addu") == 0) {		// Instruction ???, format ?
				// TODO
			}
			else if (strcmp(token, "subu") == 0) {		// Instruction ???, format ?
				// TODO subtract unsigned
			}
			else if (strcmp(token, "beq") == 0) {		// Instruction ???, format ?
				// TODO branch on ==
			}
			else if (strcmp(token, "bme") == 0) {		// Instruction ???, format ?
				// TODO branch on !=
			}
			else if (strcmp(token, "j") == 0) {			// Instruction ???, format ?
				// TODO jump
			}
			else if (strcmp(token, "jal") == 0) {		// Instruction ???, format ?
				// TODO jump and link
			}
			else if (strcmp(token, "jr") == 0) {		// Instruction ???, format ?
				// TODO jump register
			}
			else if (strcmp(token, "lui") == 0) {		// Instruction ???, format ?
				// TODO load upper imm.unsigned
			}
			else if (strcmp(token, "sltiu") == 0) {		// Instruction ???, format ?
				// TODO set less than imm.unsigned
			}
			else if (strcmp(token, "sltu") == 0) {		// Instruction ???, format ?
				// TODO set less than unsigned
			}
			else if (strcmp(token, "sll") == 0) {		// Instruction ???, format ?
				// TODO shift << logical
			}
			else if (strcmp(token, "jr") == 0) {		// Instruction ???, format ?
				// TODO shitf >> logical
			}
			else {
				// not insturctions. maybe Label?
				// 메모리 할당. 어떻게 하지?
			}


			token = strtok(NULL, "\n\t");
		}


	}

	// process();

	fclose(input);
	//fclose(output);
	exit(EXIT_SUCCESS);
}


/*******************************************************
 * Function: change_file_ext
 *
 * Parameters:
 *  char
 *      *str: a raw filename (without path)
 *
 * Return:
 *  return NULL if a file is not an assembly file.
 *  return empty string
 *
 * Info:
 *  This function reads filename and converst it into
 *  object extention name, *.o
 *
 *******************************************************/
char
*change_file_ext(char *str)
{
	char *dot = strrchr(str, '.');

	if (!dot || dot == str || (strcmp(dot, ".s") != 0)) {
		return NULL;
	}

	str[strlen(str) - 1] = 'o';
	return "";
}

const char* int2bin5(int num){
	char result[] = "00000";
	for (int i = 4; i >= 0; i--) {
		result[i] = (num%2 == 0) ? '0' : '1';
		num /= 2;
	}
	return strdup(result);
}

const char* int2bin16(int num) {
	char result[] = "0000000000000000";
	for (int i = 15; i >= 0; i--) {
		result[i] = (num % 2 == 0) ? '0' : '1';
		num /= 2;
	}
	return strdup(result);
}
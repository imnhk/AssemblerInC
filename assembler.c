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

const char* int2bin(int num);

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
	int dataLineCount = 0, textLineCount = 0;
	char binaryLine[] = "00000000000000000000000000000000"; // 32bit

	while (fgets(instructionLine, MAX_INSTRUCTION_LEN, input) != NULL){

		// Skip until find .text
		token = strtok(instructionLine, "\n\t");
		dataLineCount++;
		if (strcmp(token, ".text") == 0) {
			dataLineCount -= 2; // exclude ".data" and ".text" line
			break;
		}
	}

	printf("Data: %d\n", dataLineCount);

	while (fgets(instructionLine, MAX_INSTRUCTION_LEN, input) != NULL){
		
		token = strtok(instructionLine, "\t\n, ");
		textLineCount++;

		while (token){

			printf("[%s]\n", token);

			if (strcmp(token, "and") == 0) {
				// opcode of AND is 000000
				strcpy(binaryLine, "000000");

				// Instruction AND, format R
				// op3 <- op1 & op2
				op1 = strtok(NULL, "$\n, "); // rs 17
				op2 = strtok(NULL, "$\n, "); // rt 17
				op3 = strtok(NULL, "$\n, "); // rd 0

				strcat(binaryLine, int2bin(atoi(op2)));
				strcat(binaryLine, int2bin(atoi(op3)));
				strcat(binaryLine, int2bin(atoi(op1)));

				// funct of AND is 100100
				strcat(binaryLine, "000000");

				printf("bin: %s\n", binaryLine);

			}
			else if (strcmp(token, "andi") == 0) {		
				// Instruction ANDI, format I
				// op2 = op1 & immediate(ZeroExtImm???)

				op1 = strtok(NULL, "\n, "); // rs
				op2 = strtok(NULL, "\n, "); // rt
				op3 = strtok(NULL, "\n, "); // immediate
				printf("andi: %s %s %s\n", op1, op2, op3);
			}
			else if (strcmp(token, "or") == 0) {		
				// Instruction OR, format R
				// op3 = op1 | op2

				op1 = strtok(NULL, "\n, "); // rs
				op2 = strtok(NULL, "\n, "); // rt
				op3 = strtok(NULL, "\n, "); // rd
				printf("or: %s %s %s\n", op1, op2, op3);
			}
			else if (strcmp(token, "ori") == 0) {		
				// Instruction ORI, format I
				// op2 = op1 | immediate(ZeroExtImm???)

				op1 = strtok(NULL, "\n, "); // rs
				op2 = strtok(NULL, "\n, "); // rt
				op3 = strtok(NULL, "\n, "); // immediate
				printf("andi: %s %s %s\n", op1, op2, op3);
			}
			else if (strcmp(token, "nor") == 0) {		
				// Instruction NOR, format R
				// op3 = ~(op1 | op2)

				op1 = strtok(NULL, "\n, "); // rs
				op2 = strtok(NULL, "\n, "); // rt
				op3 = strtok(NULL, "\n, "); // rd
			}
			else if (strcmp(token, "la") == 0) {		
				// Instruction LA, format PSEUDO
				// Load Address is pseudo insturction
				// should be divided into two instruction
				// lui $register upper 16bit address +
				// ori $register lower 16bit address



				op1 = strtok(NULL, "\n, "); // register
				op2 = strtok(NULL, "\n, "); // label
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

const char* int2bin(int num){
	char result[6];
	result[5] = '\0';
	printf("Convert %d into ", num);
	for (int i = 4; i >= 0; i--) {
		result[i] = (num%2 == 0) ? '0' : '1';
		num /= 2;
	}
	printf("to %s\n", result);
	return strdup(result);
}
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

int hex2int(char* hex);

const char* int2bin5(int num);
const char* int2bin16(int num);
const char* int2bin32(int num);

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

	input = fopen("example5.s", "r");
	if (input == NULL){
		perror("ERROR");
		exit(EXIT_FAILURE);
	}

	filename = strdup("output_test.o");

	char *token = NULL;
	char *op1, *op2, *op3;
	char tempInstructionLine[MAX_INSTRUCTION_LEN]; // temp memory

	char textAreaBinary[1000] = "";
	char resultBinary[1000] = "";
	char tempBinary32[] = "00000000000000000000000000000000";

	const int dataStartAddress = 4096; // upper 16bit (0x1000 0000)
	const int textStartAddress = 64; // upper 16bit (0x0040 0000)

	struct data {
		int location;
		char name[20];
		char value[20];
	};
	struct data dataList[10];
	int dataCounter = 0;

	struct label {
		int location;
		char* name[20];
	};
	struct label labelList[10];
	int labelCounter = 0;

	int program[100][MAX_INSTRUCTION_LEN];
	int counter = 0;

	int totalTextCount = 0; // binary line count, exclude labels.
	int additionalTextCount = 0; // additional line created by LA insturction

	fgets(tempInstructionLine, MAX_INSTRUCTION_LEN, input); // first line should be .data

	// Load .data area
	while (fgets(tempInstructionLine, MAX_INSTRUCTION_LEN, input) != NULL){
		token = strtok(tempInstructionLine, "\n\t");
		
		if (strcmp(token, ".text") == 0) {
			break;
		}

		// this line has label(name)
		if (token[strlen(token)-1] == ':') {
			strcpy(labelList[labelCounter].name, token);
			token = strtok(NULL, "\n\t "); // Discard .text token

			labelList[labelCounter].location = dataStartAddress + (dataCounter * 4);
			labelCounter++;
		}
		else { // don't have label(non-first element of array)
			strcpy(dataList[dataCounter].name, "NONAME");
		}

		token = strtok(NULL, "\n\t ");
		strcpy(dataList[dataCounter].value, token);

		dataList[dataCounter].location = dataStartAddress + (4 * dataCounter);
		dataCounter++;
	}


	// Load .text area
	while (fgets(program[counter], MAX_INSTRUCTION_LEN, input) != NULL) {

		strcpy(tempInstructionLine, program[counter]);
		//printf("program: %s\n", tempInstructionLine);
		token = strtok(tempInstructionLine, "\n\t");

		// Save Label
		if (token[strlen(token)-1] == ':') {
			strcpy(labelList[labelCounter].name, token);
			labelList[labelCounter].location = textStartAddress + (counter * 4);

			labelCounter++;
			counter--;
		}
		counter++;
	}
	
	// Print .data table
	for (int i = 0; i < dataCounter; i++) {
		printf("DATA: %s at %d\n", dataList[i].value, dataList[i].location);
	}

	// Print labels
	for (int i = 0; i < labelCounter; i++) {
		printf("LABEL: %s at %d\n", labelList[i].name, labelList[i].location);
	}
	
	totalTextCount = counter;
	counter = 0;

	// process....
	while (counter < totalTextCount){

		token = strtok(program[counter], "\n\t$");
		//printf("counter: %d, token: %s\n", counter, token);

		if (strcmp(token, "and") == 0) {
			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinary32, "000000"); // opcode

			// rs, rt, rd
			strcat(tempBinary32, int2bin5(atoi(op2)));
			strcat(tempBinary32, int2bin5(atoi(op3)));
			strcat(tempBinary32, int2bin5(atoi(op1)));

			strcat(tempBinary32, "00000"); // shamt
			strcat(tempBinary32, "100100"); // funct

			printf("and  : %s\n", tempBinary32);
			strcat(textAreaBinary, tempBinary32);
		}
		else if (strcmp(token, "andi") == 0) {
			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinary32, "001100"); // opcode

			strcat(tempBinary32, int2bin5(atoi(op2))); // rs
			strcat(tempBinary32, int2bin5(atoi(op1))); // rt

			if (op3[0] == '0' && op3[1] == 'x') //immediate is hex
				strcat(tempBinary32, int2bin16(hex2int(op3)));
			else
				strcat(tempBinary32, int2bin16(atoi(op3)));

			printf("andi : %s\n", tempBinary32);
			strcat(textAreaBinary, tempBinary32);
		}
		else if (strcmp(token, "or") == 0) {
			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinary32, "000000"); // opcode

			// rs, rt, rd
			strcat(tempBinary32, int2bin5(atoi(op2)));
			strcat(tempBinary32, int2bin5(atoi(op3)));
			strcat(tempBinary32, int2bin5(atoi(op1)));

			strcat(tempBinary32, "00000"); // shamt
			strcat(tempBinary32, "100101"); // funct

			printf("or   : %s\n", tempBinary32);
			strcat(textAreaBinary, tempBinary32);
		}
		else if (strcmp(token, "ori") == 0) {

			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinary32, "001101"); // opcode

			strcat(tempBinary32, int2bin5(atoi(op2))); // rs
			strcat(tempBinary32, int2bin5(atoi(op1))); // rt

			if (op3[0] == '0' && op3[1] == 'x') //immediate is hex
				strcat(tempBinary32, int2bin16(hex2int(op3)));
			else
				strcat(tempBinary32, int2bin16(atoi(op3)));

			printf("ori  : %s\n", tempBinary32);
			strcat(textAreaBinary, tempBinary32);
		}
		else if (strcmp(token, "nor") == 0) {
			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinary32, "000000"); // opcode

			// rs, rt, rd
			strcat(tempBinary32, int2bin5(atoi(op2)));
			strcat(tempBinary32, int2bin5(atoi(op3)));
			strcat(tempBinary32, int2bin5(atoi(op1)));

			strcat(tempBinary32, "00000"); // shamt
			strcat(tempBinary32, "100111"); // funct

			printf("nor  : %s\n", tempBinary32);
			strcat(textAreaBinary, tempBinary32);
		}
		else if (strcmp(token, "la") == 0) {

			op1 = strtok(NULL, "$\n, "); // register
			op2 = strtok(NULL, "$\n, "); // label

			strcpy(tempBinary32, "001111"); // opcode
			strcat(tempBinary32, "00000"); // rs
			strcat(tempBinary32, int2bin5(atoi(op1))); // rt
			strcat(tempBinary32, int2bin16(dataStartAddress)); // immediate

			printf("lui  : %s\n", tempBinary32);
			strcat(textAreaBinary, tempBinary32);

			strcat(op2, ":");
			for (int i = 0; i < labelCounter; i++) {
				if (strcmp(labelList[i].name, op2) == 0) {
					if (labelList[i].location != dataStartAddress) {
						strcpy(tempBinary32, "001101"); // opcode
						strcat(tempBinary32, int2bin5(atoi(op1)));
						strcat(tempBinary32, int2bin5(atoi(op1)));
						strcat(tempBinary32, int2bin16(labelList[i].location - dataStartAddress));

						additionalTextCount++;

						printf("ori  : %s\n", tempBinary32);
						strcat(textAreaBinary, tempBinary32);
					}
					break;
				}
			}
		}
		else if (strcmp(token, "lw") == 0) {
			op1 = strtok(NULL, "$()\n, ");
			op2 = strtok(NULL, "$()\n, ");
			op3 = strtok(NULL, "$()\n, ");

			strcpy(tempBinary32, "100011");

			strcat(tempBinary32, int2bin5(atoi(op3)));
			strcat(tempBinary32, int2bin5(atoi(op1)));
			strcat(tempBinary32, int2bin16(atoi(op2)));

			printf("lw   : %s\n", tempBinary32);
			strcat(textAreaBinary, tempBinary32);
		}
		else if (strcmp(token, "sw") == 0) {
			op1 = strtok(NULL, "$()\n, ");
			op2 = strtok(NULL, "$()\n, ");
			op3 = strtok(NULL, "$()\n, ");

			strcpy(tempBinary32, "101011");

			strcat(tempBinary32, int2bin5(atoi(op3)));
			strcat(tempBinary32, int2bin5(atoi(op1)));
			strcat(tempBinary32, int2bin16(atoi(op2)));

			printf("sw   : %s\n", tempBinary32);
			strcat(textAreaBinary, tempBinary32);
		}
		else if (strcmp(token, "addiu") == 0) {

			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinary32, "001001");

			strcat(tempBinary32, int2bin5(atoi(op2)));
			strcat(tempBinary32, int2bin5(atoi(op1)));

			if (op3[0] == '0' && op3[1] == 'x') //immediate is hex
				strcat(tempBinary32, int2bin16(hex2int(op3)));
			else
				strcat(tempBinary32, int2bin16(atoi(op3)));

			printf("addiu: %s\n", tempBinary32);
			strcat(textAreaBinary, tempBinary32);
		}
		else if (strcmp(token, "addu") == 0) {

			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinary32, "000000"); // opcode

			strcat(tempBinary32, int2bin5(atoi(op2))); // rs
			strcat(tempBinary32, int2bin5(atoi(op3))); // rt
			strcat(tempBinary32, int2bin5(atoi(op1))); // rd

			strcat(tempBinary32, "00000"); // shamt
			strcat(tempBinary32, "100001"); // funct

			printf("addu : %s\n", tempBinary32);
			strcat(textAreaBinary, tempBinary32);
		}
		else if (strcmp(token, "subu") == 0) {
			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinary32, "000000"); // opcode

			strcat(tempBinary32, int2bin5(atoi(op2))); // rs
			strcat(tempBinary32, int2bin5(atoi(op3))); // rt
			strcat(tempBinary32, int2bin5(atoi(op1))); // rd

			strcat(tempBinary32, "00000"); // shamt
			strcat(tempBinary32, "100011"); // funct

			printf("subu : %s\n", tempBinary32);
			strcat(textAreaBinary, tempBinary32);
		}
		else if (strcmp(token, "beq") == 0) {
			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinary32, "000100"); // opcode

			strcat(tempBinary32, int2bin5(atoi(op1)));
			strcat(tempBinary32, int2bin5(atoi(op2)));

			strcat(op3, ":");
			for (int i = 0; i < labelCounter; i++) {
				if (strcmp(labelList[i].name, op3) == 0) {
					int dist = ((labelList[i].location - textStartAddress) / 4) - counter -1;
					strcat(tempBinary32, int2bin16(dist)); // immediate
					break;
				}
			}
			printf("beq  : %s\n", tempBinary32);
			strcat(textAreaBinary, tempBinary32);
		}
		else if (strcmp(token, "bne") == 0) {

			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, "); // label

			strcpy(tempBinary32, "000101"); // opcode

			strcat(tempBinary32, int2bin5(atoi(op1))); // rs
			strcat(tempBinary32, int2bin5(atoi(op2))); // rt

			strcat(op3, ":");
			for (int i = 0; i < labelCounter; i++) {
				if (strcmp(labelList[i].name, op3) == 0) {
					int dist = ((labelList[i].location - textStartAddress) / 4) - counter - 1;
					strcat(tempBinary32, int2bin16(dist)); // immediate
					break;
				}
			}
			printf("bne  : %s\n", tempBinary32);
			strcat(textAreaBinary, tempBinary32);
		}
		else if (strcmp(token, "j") == 0) {

			op1 = strtok(NULL, "$\n, "); // label

			strcpy(tempBinary32, "000010"); // opcode
			strcat(tempBinary32, "0000010000"); // I don't actually understand.
			// maybe this is always same??? just start address of .text area?

			strcat(op1, ":");
			for (int i = 0; i < labelCounter; i++) {
				if (strcmp(labelList[i].name, op1) == 0) {
					int loc = (labelList[i].location - textStartAddress) / 4 + additionalTextCount;
					strcat(tempBinary32, int2bin16(loc));
					break;
				}
			}
			printf("j    : %s\n", tempBinary32);
			strcat(textAreaBinary, tempBinary32);
		}
		else if (strcmp(token, "jal") == 0) {
			op1 = strtok(NULL, "$\n, "); // label
			
			strcpy(tempBinary32, "000011"); // opcode
			strcat(tempBinary32, "0000010000");

			strcat(op1, ":");
			for (int i = 0; i < labelCounter; i++) {
				if (strcmp(labelList[i].name, op1) == 0) {
					int loc = (labelList[i].location - textStartAddress) / 4;
					strcat(tempBinary32, int2bin16(loc));
					break;
				}
			}
			printf("jal  : %s\n", tempBinary32);
			strcat(textAreaBinary, tempBinary32);
		}
		else if (strcmp(token, "jr") == 0) {
			op1 = strtok(NULL, "$\n, ");
			
			strcpy(tempBinary32, "000000");
			strcat(tempBinary32, int2bin5(atoi(op1))); // rs
			strcat(tempBinary32, "00000");
			strcat(tempBinary32, "00000");
			strcat(tempBinary32, "00000");
			strcat(tempBinary32, "001000");

			printf("jr   : %s\n", tempBinary32);
			strcat(textAreaBinary, tempBinary32);
		}
		else if (strcmp(token, "lui") == 0) {
			op1 = strtok(NULL, "$\n, "); // register
			op2 = strtok(NULL, "$\n, "); // label

			strcpy(tempBinary32, "001111"); // opcode
			strcat(tempBinary32, "00000"); // rs

			strcat(tempBinary32, int2bin5(atoi(op1))); // rt

			if (op2[0] == '0' && op2[1] == 'x') //immediate is hex
				strcat(tempBinary32, int2bin16(hex2int(op2)));
			else
				strcat(tempBinary32, int2bin16(atoi(op2)));

			printf("lui  : %s\n", tempBinary32);
			strcat(textAreaBinary, tempBinary32);
		}
		else if (strcmp(token, "sltiu") == 0) {
			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinary32, "001011"); // opcode

			strcat(tempBinary32, int2bin5(atoi(op2))); // rt
			strcat(tempBinary32, int2bin5(atoi(op1))); // rd
			strcat(tempBinary32, int2bin16(atoi(op3))); // shamt

			printf("sltiu: %s\n", tempBinary32);
			strcat(textAreaBinary, tempBinary32);
		}
		else if (strcmp(token, "sltu") == 0) {
			op1 = strtok(NULL, "$\n, "); // 4 2 3
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinary32, "000000"); // opcode

			strcat(tempBinary32, int2bin5(atoi(op2))); // rs
			strcat(tempBinary32, int2bin5(atoi(op3))); // rt
			strcat(tempBinary32, int2bin5(atoi(op1))); // rd
			strcat(tempBinary32, "00000"); // rd
			strcat(tempBinary32, "101011"); // funct

			printf("sltu : %s\n", tempBinary32);
			strcat(textAreaBinary, tempBinary32);
		}
		else if (strcmp(token, "sll") == 0) {

			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinary32, "000000"); // opcode
			strcat(tempBinary32, "00000"); // rs

			strcat(tempBinary32, int2bin5(atoi(op2))); // rt
			strcat(tempBinary32, int2bin5(atoi(op1))); // rd
			strcat(tempBinary32, int2bin5(atoi(op3))); // shamt

			strcat(tempBinary32, "000000"); // funct

			printf("sll  : %s\n", tempBinary32);
			strcat(textAreaBinary, tempBinary32);

		}
		else if (strcmp(token, "srl") == 0) {

			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinary32, "000000"); // opcode
			strcat(tempBinary32, "00000"); // rs

			strcat(tempBinary32, int2bin5(atoi(op2))); // rt
			strcat(tempBinary32, int2bin5(atoi(op1))); // rd
			strcat(tempBinary32, int2bin5(atoi(op3))); // shamt

			strcat(tempBinary32, "000010"); // funct

			printf("srl  : %s\n", tempBinary32);
			strcat(textAreaBinary, tempBinary32);
		}

		counter++;
	}
	
	printf("@@@@ end of line @@@@\n\n");

	// print out count of data and text at front of code

	strcat(resultBinary, "0000000000000000");
	strcat(resultBinary, int2bin16((totalTextCount + additionalTextCount) * 4));
	//printf(".text: %s (%d) \n", textAreaBinary, (totalTextCount + additionalTextCount) * 4);

	strcat(resultBinary, "0000000000000000");
	strcat(resultBinary, int2bin16(dataCounter * 4));
	//printf(".data: %s (%d)\n", textAreaBinary, dataCounter * 4);

	strcat(resultBinary, textAreaBinary);

	// print out every data at end of code
	for (int i = 0; i < dataCounter; i++) {
		if (dataList[i].value[1] == 'x') { // data is hex(address)
			strcat(resultBinary, int2bin32(hex2int(dataList[i].value)));
		}
		else {
			strcat(resultBinary, int2bin32(atoi(dataList[i].value)));
		}
	}
	
	printf("@@@@@@@@@@@@@@@@@@@@@ RESULT @@@@@@@@@@@@@\n\n");
	printf("%s", resultBinary);

	
	fclose(input);
	//fclose(output);
	exit(EXIT_SUCCESS);
}

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

int hex2int(char* hex){
	hex += 2; // remove '0x'
	int result = 0;
	
	while ((*hex) != '\0'){
		if (('0' <= (*hex)) && ((*hex) <= '9'))
			result = result * 16 + (*hex) - '0';
		else if (('a' <= (*hex)) && ((*hex) <= 'f'))
			result = result * 16 + (*hex) - 'a' + 10;
		else if (('A' <= (*hex)) && ((*hex) <= 'F'))
			result = result * 16 + (*hex) - 'A' + 10;
		hex++;
	}
	return result;
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

	if (num < 0) {
		strcpy(result, int2bin16(~num));
		for (int i = 0; i < 16; i++) {
			if (result[i] == '0')
				result[i] = '1';
			else
				result[i] = '0';
		}
		return strdup(result);
	}

	for (int i = 15; i >= 0; i--) {
		result[i] = (num % 2 == 0) ? '0' : '1';
		num /= 2;
	}

	return strdup(result);
}

const char* int2bin32(int num) {
	char result[] = "00000000000000000000000000000000";
	for (int i = 31; i >= 0; i--) {
		result[i] = (num % 2 == 0) ? '0' : '1';
		num /= 2;
	}
	return strdup(result);
}
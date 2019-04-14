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

	input = fopen("example1.s", "r");
	if (input == NULL){
		perror("ERROR");
		exit(EXIT_FAILURE);
	}

	filename = strdup("output_test.o"); // strdup() is not a standard C library but fairy used a lot.

	char *token = NULL;
	char *op1, *op2, *op3;
	char tempInstructionLine[MAX_INSTRUCTION_LEN]; // temp memory

	char resultProgram[1000] = ""; // is this enough? 

	struct data {
		int location;
		char name[20];
		char value[20];
	};
	struct data dataList[10];
	int dataCounter = 0;
	const int dataStartAddress = 4096; // upper 16bit (0x1000 0000)

	struct label {
		int location;
		char* name[20];
	};
	struct label labelList[10];
	int labelCounter = 0;

	int program[100][MAX_INSTRUCTION_LEN];
	int counter = 0;
	const int textStartAddress = 64; // 0000 0000 0100 0000(2) upper 16bit (0x0040 0000)

	int totalTextCount = 0; // binary line count, exclude labels.
	int additionalTextCount = 0; // additional line created by LA insturction

	char tempBinaryLine[] = "00000000000000000000000000000000"; // 32bit, temp


	fgets(tempInstructionLine, MAX_INSTRUCTION_LEN, input); // first line should be .data
	//token = strtok(tempInstructionLine, "\n\t");

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
			//printf("Saved label %s at %d\n", labelList[labelCounter].name, labelList[labelCounter].location);
			labelCounter++;
		}
		else { // don't have label(non-first element of array)
			strcpy(dataList[dataCounter].name, "NONAME");
		}

		token = strtok(NULL, "\n\t ");
		strcpy(dataList[dataCounter].value, token); // value	

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
		printf("counter: %d, token: %s\n", counter, token);

		if (strcmp(token, "and") == 0) {
			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinaryLine, "000000"); // opcode

			// rs, rt, rd
			strcat(tempBinaryLine, int2bin5(atoi(op2)));
			strcat(tempBinaryLine, int2bin5(atoi(op3)));
			strcat(tempBinaryLine, int2bin5(atoi(op1)));

			strcat(tempBinaryLine, "00000"); // shamt
			strcat(tempBinaryLine, "100100"); // funct

			printf("and  : %s\n", tempBinaryLine);
		}
		else if (strcmp(token, "andi") == 0) {
			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinaryLine, "001100"); // opcode

			strcat(tempBinaryLine, int2bin5(atoi(op2))); // rs
			strcat(tempBinaryLine, int2bin5(atoi(op1))); // rt

			if (op3[0] == '0' && op3[1] == 'x') //immediate is hex
				strcat(tempBinaryLine, int2bin16(hex2int(op3)));
			else
				strcat(tempBinaryLine, int2bin16(atoi(op3)));

			printf("andi : %s\n", tempBinaryLine);
		}
		else if (strcmp(token, "or") == 0) {
			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinaryLine, "000000"); // opcode

			// rs, rt, rd
			strcat(tempBinaryLine, int2bin5(atoi(op2)));
			strcat(tempBinaryLine, int2bin5(atoi(op3)));
			strcat(tempBinaryLine, int2bin5(atoi(op1)));

			strcat(tempBinaryLine, "00000"); // shamt
			strcat(tempBinaryLine, "100101"); // funct

			printf("or   : %s\n", tempBinaryLine);
		}
		else if (strcmp(token, "ori") == 0) {

			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinaryLine, "001101"); // opcode

			strcat(tempBinaryLine, int2bin5(atoi(op2))); // rs
			strcat(tempBinaryLine, int2bin5(atoi(op1))); // rt

			if (op3[0] == '0' && op3[1] == 'x') //immediate is hex
				strcat(tempBinaryLine, int2bin16(hex2int(op3)));
			else
				strcat(tempBinaryLine, int2bin16(atoi(op3)));

			printf("ori  : %s\n", tempBinaryLine);
		}
		else if (strcmp(token, "nor") == 0) {
			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinaryLine, "000000"); // opcode

			// rs, rt, rd
			strcat(tempBinaryLine, int2bin5(atoi(op2)));
			strcat(tempBinaryLine, int2bin5(atoi(op3)));
			strcat(tempBinaryLine, int2bin5(atoi(op1)));

			strcat(tempBinaryLine, "00000"); // shamt
			strcat(tempBinaryLine, "100111"); // funct

			printf("nor  : %s\n", tempBinaryLine);
		}
		else if (strcmp(token, "la") == 0) {

			op1 = strtok(NULL, "$\n, "); // register
			op2 = strtok(NULL, "$\n, "); // label

			strcpy(tempBinaryLine, "001111"); // opcode
			strcat(tempBinaryLine, "00000"); // rs
			strcat(tempBinaryLine, int2bin5(atoi(op1))); // rt
			strcat(tempBinaryLine, int2bin16(dataStartAddress)); // immediate

			printf("lui  : %s\n", tempBinaryLine);

			strcat(op2, ":");
			for (int i = 0; i < labelCounter; i++) {
				if (strcmp(labelList[i].name, op2) == 0) {
					if (labelList[i].location != dataStartAddress) {
						strcpy(tempBinaryLine, "001101"); // opcode
						strcat(tempBinaryLine, int2bin5(atoi(op1)));
						strcat(tempBinaryLine, int2bin5(atoi(op1)));
						strcat(tempBinaryLine, int2bin16(labelList[i].location - dataStartAddress));

						counter++;
						additionalTextCount++;

						printf("ori  : %s\n", tempBinaryLine);
					}
					break;
				}
			}
		}
		else if (strcmp(token, "lw") == 0) {
			op1 = strtok(NULL, "$()\n, ");
			op2 = strtok(NULL, "$()\n, ");
			op3 = strtok(NULL, "$()\n, ");

			strcpy(tempBinaryLine, "100011");

			strcat(tempBinaryLine, int2bin5(atoi(op3))); // $3
			strcat(tempBinaryLine, int2bin5(atoi(op1))); // $5

			strcat(tempBinaryLine, int2bin16(atoi(op2)));

			printf("lw   : %s\n", tempBinaryLine);
		}
		else if (strcmp(token, "sw") == 0) {
		}
		else if (strcmp(token, "addiu") == 0) {

			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinaryLine, "001001");

			strcat(tempBinaryLine, int2bin5(atoi(op2)));
			strcat(tempBinaryLine, int2bin5(atoi(op1)));

			if (op3[0] == '0' && op3[1] == 'x') //immediate is hex
				strcat(tempBinaryLine, int2bin16(hex2int(op3)));
			else
				strcat(tempBinaryLine, int2bin16(atoi(op3)));

			printf("addiu: %s\n", tempBinaryLine);
		}
		else if (strcmp(token, "addu") == 0) {

			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinaryLine, "000000"); // opcode

			strcat(tempBinaryLine, int2bin5(atoi(op2))); // rs
			strcat(tempBinaryLine, int2bin5(atoi(op3))); // rt
			strcat(tempBinaryLine, int2bin5(atoi(op1))); // rd

			strcat(tempBinaryLine, "00000"); // shamt
			strcat(tempBinaryLine, "100001"); // funct

			printf("addu : %s\n", tempBinaryLine);
		}
		else if (strcmp(token, "subu") == 0) {
			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinaryLine, "000000"); // opcode

			strcat(tempBinaryLine, int2bin5(atoi(op2))); // rs
			strcat(tempBinaryLine, int2bin5(atoi(op3))); // rt
			strcat(tempBinaryLine, int2bin5(atoi(op1))); // rd

			strcat(tempBinaryLine, "00000"); // shamt
			strcat(tempBinaryLine, "100011"); // funct

			printf("subu : %s\n", tempBinaryLine);
		}
		else if (strcmp(token, "beq") == 0) {
			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinaryLine, "000100"); // opcode

			strcat(tempBinaryLine, int2bin5(atoi(op2)));
			strcat(tempBinaryLine, int2bin5(atoi(op1)));

			strcat(op3, ":");
			for (int i = 0; i < labelCounter; i++) {
				if (strcmp(labelList[i].name, op3) == 0) {
					int dist = ((labelList[i].location - textStartAddress) / 4) - counter -1;
					strcat(tempBinaryLine, int2bin16(dist)); // immediate
					break;
				}
			}
			printf("beq  : %s\n", tempBinaryLine);
		}
		else if (strcmp(token, "bne") == 0) {

			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, "); // label

			strcpy(tempBinaryLine, "000101"); // opcode

			strcat(tempBinaryLine, int2bin5(atoi(op1))); // rs
			strcat(tempBinaryLine, int2bin5(atoi(op2))); // rt

			strcat(op3, ":");
			for (int i = 0; i < labelCounter; i++) {
				if (strcmp(labelList[i].name, op3) == 0) {
					int dist = ((labelList[i].location - textStartAddress) / 4) - counter - 1;
					strcat(tempBinaryLine, int2bin16(dist)); // immediate
					break;
				}
			}
			printf("bne  : %s\n", tempBinaryLine);
		}
		else if (strcmp(token, "j") == 0) {

			op1 = strtok(NULL, "$\n, "); // label

			strcpy(tempBinaryLine, "000010"); // opcode
			strcat(tempBinaryLine, "0000010000"); // I don't actually understand.
			// maybe this is always same??? just start address of .text area?

			strcat(op1, ":");
			for (int i = 0; i < labelCounter; i++) {
				if (strcmp(labelList[i].name, op1) == 0) {
					int loc = (labelList[i].location - textStartAddress) / 4 + additionalTextCount;
					strcat(tempBinaryLine, int2bin16(loc));
					break;
				}
			}
			printf("j    : %s\n", tempBinaryLine);	
		}
		else if (strcmp(token, "jal") == 0) {
			op1 = strtok(NULL, "$\n, "); // label
			
			strcpy(tempBinaryLine, "000011"); // opcode
			strcat(tempBinaryLine, "0000010000");

			strcat(op1, ":");
			for (int i = 0; i < labelCounter; i++) {
				if (strcmp(labelList[i].name, op1) == 0) {
					int loc = (labelList[i].location - textStartAddress) / 4;
					strcat(tempBinaryLine, int2bin16(loc));
					break;
				}
			}
			printf("jal  : %s\n", tempBinaryLine);
		}
		else if (strcmp(token, "jr") == 0) {
			op1 = strtok(NULL, "$\n, ");
			
			strcpy(tempBinaryLine, "000000");
			strcat(tempBinaryLine, int2bin5(atoi(op1))); // rs
			strcat(tempBinaryLine, "00000");
			strcat(tempBinaryLine, "00000");
			strcat(tempBinaryLine, "00000");
			strcat(tempBinaryLine, "001000");

			printf("jr   : %s\n", tempBinaryLine);
		}
		else if (strcmp(token, "lui") == 0) {
			op1 = strtok(NULL, "$\n, "); // register
			op2 = strtok(NULL, "$\n, "); // label

			strcpy(tempBinaryLine, "001111"); // opcode
			strcat(tempBinaryLine, "00000"); // rs

			strcat(tempBinaryLine, int2bin5(atoi(op1))); // rt
			strcat(tempBinaryLine, int2bin16(atoi(op2))); // immediate

			printf("lui  : %s\n", tempBinaryLine);
		}
		else if (strcmp(token, "sltiu") == 0) {
			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinaryLine, "001011"); // opcode

			strcat(tempBinaryLine, int2bin5(atoi(op2))); // rt
			strcat(tempBinaryLine, int2bin5(atoi(op1))); // rd
			strcat(tempBinaryLine, int2bin16(atoi(op3))); // shamt

			printf("sltiu: %s\n", tempBinaryLine);
		}
		else if (strcmp(token, "sltu") == 0) {
			op1 = strtok(NULL, "$\n, "); // 4 2 3
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinaryLine, "000000"); // opcode

			strcat(tempBinaryLine, int2bin5(atoi(op2))); // rs
			strcat(tempBinaryLine, int2bin5(atoi(op3))); // rt
			strcat(tempBinaryLine, int2bin5(atoi(op1))); // rd
			strcat(tempBinaryLine, "00000"); // rd
			strcat(tempBinaryLine, "101011"); // funct

			printf("sltu : %s\n", tempBinaryLine);
		}
		else if (strcmp(token, "sll") == 0) {

			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinaryLine, "000000"); // opcode
			strcat(tempBinaryLine, "00000"); // rs

			strcat(tempBinaryLine, int2bin5(atoi(op2))); // rt
			strcat(tempBinaryLine, int2bin5(atoi(op1))); // rd
			strcat(tempBinaryLine, int2bin5(atoi(op3))); // shamt

			strcat(tempBinaryLine, "000000"); // funct

			printf("sll  : %s\n", tempBinaryLine);

		}
		else if (strcmp(token, "srl") == 0) {

			op1 = strtok(NULL, "$\n, ");
			op2 = strtok(NULL, "$\n, ");
			op3 = strtok(NULL, "$\n, ");

			strcpy(tempBinaryLine, "000000"); // opcode
			strcat(tempBinaryLine, "00000"); // rs

			strcat(tempBinaryLine, int2bin5(atoi(op2))); // rt
			strcat(tempBinaryLine, int2bin5(atoi(op1))); // rd
			strcat(tempBinaryLine, int2bin5(atoi(op3))); // shamt

			strcat(tempBinaryLine, "000010"); // funct

			printf("srl  : %s\n", tempBinaryLine);
		}

		counter++;
	}
	
	printf("@@@@ end of line @@@@\n\n");

	// print out count of data and text at front of code
	strcpy(resultProgram, "0000000000000000");
	strcat(resultProgram, int2bin16((totalTextCount + additionalTextCount) * 4));
	printf(".text: %s (%d) \n", resultProgram, (totalTextCount + additionalTextCount) * 4);

	strcpy(resultProgram, "0000000000000000");
	strcat(resultProgram, int2bin16(dataCounter * 4));
	printf(".data: %s (%d)\n", resultProgram, dataCounter * 4);

	printf("\nCode here\n\n");

	// print out every data at end of code
	for (int i = 0; i < dataCounter; i++) {
		if (dataList[i].value[1] == 'x') { // data is hex(address)
			printf("data : %s\n", int2bin32(hex2int(dataList[i].value)));
			strcat(resultProgram, int2bin32(hex2int(dataList[i].value)));
		}
		else {
			printf("data : %s\n", int2bin32(atoi(dataList[i].value)));
			strcat(resultProgram, int2bin32(atoi(dataList[i].value)));
		}
	}
	
	printf("@@@@@@@@@@@@@@@@@@@@@ RESULT @@@@@@@@@@@@@\n\n");
	printf("%s", resultProgram);

	
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
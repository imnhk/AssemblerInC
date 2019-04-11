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


	char dataArea[10][MAX_INSTRUCTION_LEN];
	int dataCounter = 0;
	const int dataStartAddress = 4096; // 0001 0000 0000 0000(2) upper 16bit (0x1000 0000)


	char textArea[100][MAX_INSTRUCTION_LEN];
	int counter = 0;
	const int textStartAddress = 64; // 0000 0000 0100 0000(2) upper 16bit (0x0040 0000)

	int totalTextCount = 0; // binary line count, exclude labels.
	int dataLoadCount = 0; // count 
	int labelCount = 0;


	struct variable {
		int location;
		char label[20];
		char value[20];
	};
	struct variable dataList[10];
	struct label {
		int location;
		char* name[20];
	};
	struct label labelList[10];

	char binaryLine[] = "00000000000000000000000000000000"; // 32bit, temp


	fgets(textArea[counter], MAX_INSTRUCTION_LEN, input); // first line should be .data
	token = strtok(textArea, "\n\t");

	while (fgets(textArea, MAX_INSTRUCTION_LEN, input) != NULL){
		token = strtok(textArea, "\n\t");
		printf("token: %s\n", token);

		if (strcmp(token, ".text") == 0) {
			break;
		}
		else {
			strcpy(dataList[dataCounter].label, token); // get name:

			token = strtok(NULL, "\n\t"); // this should be .word
			token = strtok(NULL, "\n\t"); // Now this is value
			strcpy(dataList[dataCounter].value, token); // get name:
			printf("got value %s\n", dataList[dataCounter].value);


			dataList[dataCounter].location = dataStartAddress + (4 * dataCounter);
			printf("Added %s at %d\n", dataList[dataCounter].label, dataList[dataCounter].location);
			dataCounter++;
		}
	}

	while (fgets(textArea, MAX_INSTRUCTION_LEN, input) != NULL){
		
		token = strtok(textArea, "\t\n, ");
		totalTextCount++;

		while (token){

			printf("token: %s\n", token);
			printf("counter: %d, ", counter);

			if (strcmp(token, "and") == 0) {
				op1 = strtok(NULL, "$\n, ");
				op2 = strtok(NULL, "$\n, ");
				op3 = strtok(NULL, "$\n, ");

				strcpy(binaryLine, "000000"); // opcode

				// rs, rt, rd
				strcat(binaryLine, int2bin5(atoi(op2)));
				strcat(binaryLine, int2bin5(atoi(op3)));
				strcat(binaryLine, int2bin5(atoi(op1))); 

				strcat(binaryLine, "00000"); // shamt
				strcat(binaryLine, "000000"); // funct

				printf("and  : %s\n", binaryLine);
			}
			else if (strcmp(token, "andi") == 0) {		
				// Instruction ANDI, format I
				// op2 = op1 & immediate(ZeroExtImm???)
			}
			else if (strcmp(token, "or") == 0) {		
				op1 = strtok(NULL, "$\n, ");
				op2 = strtok(NULL, "$\n, ");
				op3 = strtok(NULL, "$\n, ");

				strcpy(binaryLine, "000000"); // opcode
				
				// rs, rt, rd
				strcat(binaryLine, int2bin5(atoi(op2)));
				strcat(binaryLine, int2bin5(atoi(op3)));
				strcat(binaryLine, int2bin5(atoi(op1)));

				strcat(binaryLine, "00000"); // shamt
				strcat(binaryLine, "100101"); // funct

				printf("or   : %s\n", binaryLine);
			}
			else if (strcmp(token, "ori") == 0) {

				op1 = strtok(NULL, "$\n, ");
				op2 = strtok(NULL, "$\n, ");
				op3 = strtok(NULL, "$\n, ");

				strcpy(binaryLine, "001101"); // opcode

				strcat(binaryLine, int2bin5(atoi(op1))); // rs
				strcat(binaryLine, int2bin5(atoi(op2))); // rt

				if (op3[0] == '0' && op3[1] == 'x') //immediate is hex
					strcat(binaryLine, int2bin16(hex2int(op3)));
				else
					strcat(binaryLine, int2bin16(atoi(op3)));
				
				printf("ori  : %s\n", binaryLine);
			}
			else if (strcmp(token, "nor") == 0) {		
				op1 = strtok(NULL, "$\n, "); 
				op2 = strtok(NULL, "$\n, ");
				op3 = strtok(NULL, "$\n, ");

				strcpy(binaryLine, "000000"); // opcode

				// rs, rt, rd
				strcat(binaryLine, int2bin5(atoi(op2)));
				strcat(binaryLine, int2bin5(atoi(op3)));
				strcat(binaryLine, int2bin5(atoi(op1)));

				strcat(binaryLine, "00000"); // shamt
				strcat(binaryLine, "100111"); // funct

				printf("nor  : %s\n", binaryLine);
			}
			else if (strcmp(token, "la") == 0) {		

				op1 = strtok(NULL, "$\n, "); // register
				op2 = strtok(NULL, "$\n, "); // label

				strcpy(binaryLine, "001111"); // opcode
				strcat(binaryLine, "00000"); // rs
				strcat(binaryLine, int2bin5(atoi(op1))); // rt
				strcat(binaryLine, int2bin16(dataStartAddress)); // immediate

				printf("lui  : %s\n", binaryLine);

				if (dataLoadCount != 0) {
					strcpy(binaryLine, "001101"); // opcode
					strcat(binaryLine, int2bin5(atoi(op1)));
					strcat(binaryLine, int2bin5(atoi(op1)));
					strcat(binaryLine, int2bin16(dataLoadCount*4));

					totalTextCount++;
					printf("ori  : %s\n", binaryLine);
				}
				dataLoadCount++;

			}
			else if (strcmp(token, "lw") == 0) {		
			}
			else if (strcmp(token, "sw") == 0) {		
			}
			else if (strcmp(token, "addiu") == 0) {		

				op1 = strtok(NULL, "$\n, "); // rs
				op2 = strtok(NULL, "$\n, "); // rt
				op3 = strtok(NULL, "$\n, "); // immediate

				// opcode of ADDIU is 001001
				strcpy(binaryLine, "001001");

				strcat(binaryLine, int2bin5(atoi(op1)));
				strcat(binaryLine, int2bin5(atoi(op2)));

				if (op3[0] == '0' && op3[1] == 'x') //immediate is hex
					strcat(binaryLine, int2bin16(hex2int(op3)));
				else
					strcat(binaryLine, int2bin16(atoi(op3)));

				printf("addiu: %s\n", binaryLine);
			}
			else if (strcmp(token, "addu") == 0) {

				op1 = strtok(NULL, "$\n, "); // rs 5
				op2 = strtok(NULL, "$\n, "); // rt 5
				op3 = strtok(NULL, "$\n, "); // rd 31

				strcpy(binaryLine, "000000"); // opcode

				strcat(binaryLine, int2bin5(atoi(op2))); // rs
				strcat(binaryLine, int2bin5(atoi(op3))); // rt
				strcat(binaryLine, int2bin5(atoi(op1))); // rd

				strcat(binaryLine, "00000"); // shamt
				strcat(binaryLine, "100001"); // funct

				printf("addu : %s\n", binaryLine);
			}
			else if (strcmp(token, "subu") == 0) {		
			}
			else if (strcmp(token, "beq") == 0) {	
				op1 = strtok(NULL, "$\n, ");
				op2 = strtok(NULL, "$\n, "); 
				op3 = strtok(NULL, "$\n, ");

				strcpy(binaryLine, "000100"); // opcode
				
				strcat(binaryLine, int2bin5(atoi(op1))); // rs
				strcat(binaryLine, int2bin5(atoi(op2))); // rt

				// calc dist. to op3 label
				printf("beq  : todo\n");
			}
			else if (strcmp(token, "bne") == 0) {

				op1 = strtok(NULL, "$\n, ");
				op2 = strtok(NULL, "$\n, "); 
				op3 = strtok(NULL, "$\n, "); // label

				strcpy(binaryLine, "000101"); // opcode
				
				strcat(binaryLine, int2bin5(atoi(op1))); // rs
				strcat(binaryLine, int2bin5(atoi(op2))); // rt

				// calc dist. to op3 label
				printf("bne  : todo\n");
				for (int i = 0; i < labelCount; i++) {
					if (strcmp(labelList[i].name, op3) == 0) {
						printf("found label %d, located at %d.\n", labelList[i].name, labelList[i].location);

					}
				}

			}
			else if (strcmp(token, "j") == 0) {
				
				op1 = strtok(NULL, "$\n, "); // label

				strcpy(binaryLine, "000010"); // opcode

				// address
				printf("j    : todo\n");

			}
			else if (strcmp(token, "jal") == 0) {		
			}
			else if (strcmp(token, "jr") == 0) {		
			}
			else if (strcmp(token, "lui") == 0) {		
			}
			else if (strcmp(token, "sltiu") == 0) {		
			}
			else if (strcmp(token, "sltu") == 0) {		
			}
			else if (strcmp(token, "sll") == 0) {

				op1 = strtok(NULL, "$\n, ");
				op2 = strtok(NULL, "$\n, ");
				op3 = strtok(NULL, "$\n, ");

				strcpy(binaryLine, "000000"); // opcode
				strcat(binaryLine, "00000"); // rs

				strcat(binaryLine, int2bin5(atoi(op2))); // rt
				strcat(binaryLine, int2bin5(atoi(op1))); // rd
				strcat(binaryLine, int2bin5(atoi(op3))); // shamt

				strcat(binaryLine, "000000"); // funct

				printf("sll  : %s\n", binaryLine);

			}
			else if (strcmp(token, "srl") == 0) {

				op1 = strtok(NULL, "$\n, ");
				op2 = strtok(NULL, "$\n, ");
				op3 = strtok(NULL, "$\n, ");

				strcpy(binaryLine, "000000"); // opcode
				strcat(binaryLine, "00000"); // rs

				strcat(binaryLine, int2bin5(atoi(op2))); // rt
				strcat(binaryLine, int2bin5(atoi(op1))); // rd
				strcat(binaryLine, int2bin5(atoi(op3))); // shamt

				strcat(binaryLine, "000010"); // funct

				printf("srl  : %s\n", binaryLine);
			}
			else {
				// Label
				strcpy(labelList[labelCount].name, token); // get name:
				labelList[labelCount].location = textStartAddress + (counter * 4);

				printf("Added %s at %d\n", labelList[labelCount].name, labelList[labelCount].location);
				labelCount++;

				totalTextCount--;
			}

			token = strtok(NULL, "\n\t");
			counter++;
		}
	}
	printf("@@@@ end of line @@@@\n\n");


	// print out count of data and text at first of code
	strcpy(binaryLine, "0000000000000000");
	strcat(binaryLine, int2bin16(totalTextCount * 4));
	printf(".text: %s\n", binaryLine);
	strcpy(binaryLine, "0000000000000000");
	strcat(binaryLine, int2bin16(dataCounter * 4));
	printf(".data: %s\n", binaryLine);

	// print out every data at end of code
	for (int i = 0; i < dataCounter; i++) {
		if (dataList[i].value[1] == 'x') // data is hex(address)
			printf("data : %s\n", int2bin32(hex2int(dataList[i].value)));
		else
			printf("data : %s\n", int2bin32(atoi(dataList[i].value)));
	}
	

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
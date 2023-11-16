// assembler for pass1 and pass2 in sic

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_SIZE 10
#define MAX_SYMTAB_SIZE 100

int Dec(char *hex);	// turn hexadecimal into decimail
void read_opcode();
int find_opcode(char *opcode);
int is_find_op(char *opcode);
void pass1();
void pass2();
int formatObjectCode(int x, int add, int opcode);

FILE *source, *opc, *sym, *intermediate, *objcode, *objprog;

struct OPTAB {
	char OPTAB_op[MAX_SIZE];
	int OPTAB_code;
}op[59];

struct SYMTAB {
	char symtab_label[MAX_SIZE];
	int symtab_add;
}st[MAX_SYMTAB_SIZE];

int optab_num = 1, symtab_num = 0, length;

int main(){
	// start to read files for pass1
	source = fopen("source_sic.txt", "r");
    if(source == NULL) {
        printf("Error: Unable to open the source file.\n");
        return 1;
    }
    opc = fopen("opcode.txt", "r");
    if(opc == NULL) {
        printf("Error: Unable to open the opcode file.\n");
        fclose(source);
        return 1;
    }
    sym = fopen("symbol_table.txt", "w");
    if (sym == NULL) {
        printf("Error: Unable to create the symbol table file.\n");
        fclose(source);
        fclose(opc);
        return 1;
    }
    intermediate = fopen("intermediate_sic.txt", "w");
    if (intermediate == NULL) {
        printf("Error: Unable to create the intermediate file.\n");
        fclose(source);
        fclose(opc);
        fclose(sym);
        return 1;
    }
	read_opcode();
    pass1();
    fclose(sym);
    fclose(intermediate);
    
    sym = fopen("symbol_table.txt", "r");
    if (sym == NULL) {
        printf("Error: Unable to open the the symbol table file.\n");
        fclose(source);
        fclose(opc);
        return 1;
    }
    intermediate = fopen("intermediate_sic.txt", "r");
    if (intermediate == NULL) {
        printf("Error: Unable to open the intermediate file.\n");
        fclose(source);
        fclose(opc);
        fclose(sym);
        return 1;
    }
    objcode = fopen("object_code_sic.txt", "w");
    if (objcode == NULL) {
        printf("Error: Unable to create the object code.\n");
        fclose(source);
        fclose(opc);
        fclose(sym);
        fclose(intermediate);
        return 1;
    }
	objprog = fopen("object_program_sic.txt", "w");
    if (objcode == NULL) {
        printf("Error: Unable to create the object program.\n");
        fclose(source);
        fclose(opc);
        fclose(sym);
        fclose(intermediate);
        fclose(objcode);
        return 1;
    }
    pass2();
    fclose(source);
    fclose(opc);
    fclose(sym);
    fclose(intermediate); 
	fclose(objcode);
	fclose(objprog);
}
void pass1() {
	printf("pass1 start ! \n\n");
    printf("intermediate: \n");
    // read 1st line
    char label[MAX_SIZE], opcode[MAX_SIZE], operand[MAX_SIZE];
    int LOCCTR = 0, i, start;
    fscanf(source, "%s\t%s\t%s", label, opcode, operand);
    if(strcmp(opcode, "START") == 0) {
    	LOCCTR = Dec(operand);
    	start = LOCCTR;
    	printf("%X\t%s\t%s\t%s\n", LOCCTR, label, opcode, operand);
    	fprintf(intermediate, "%X\t%s\t%s\t%s\n", LOCCTR, label, opcode, operand);
	}
	else {
		LOCCTR = 0;
	}
	getc(source);//get rid of \n
	// read next line
	while(1) {// scan label, opcode and operand
		int col = 0;
		//set default for label, opcode and operand 
		strcpy(label, "");
		strcpy(opcode, "");
		strcpy(operand, ""); 
		char ch;
		while ((ch = getc(source)) != '\n' && ch != EOF) {
		    if (ch == '\t') {
		        col++;
		    }
			else {
		        switch (col) {
		            case 0:
		                strncat(label, &ch, 1);
		                break;
		            case 1:
		                strncat(opcode, &ch, 1);
		                break;
		            case 2:
		                strncat(operand, &ch, 1);
		                break;
		        }
		    }
		}

		//sscanf(label, "%s %s %s", label, opcode, operand);
		// omit comment
		if(label[0] == '#') {	
			continue;
		}
		
		// for end 
		if(strcmp(opcode, "END") == 0) {
			printf("\t%s\t%s\t%s\n", label, opcode, operand);
			fprintf(intermediate, "\t%s\t%s\t%s\n", label, opcode, operand);
			break;
		}
		
		// make symbol table w/ label and address
		if(strcmp(label, "") != 0) {
			strcpy(st[symtab_num].symtab_label, label);
			st[symtab_num].symtab_add = LOCCTR;
			symtab_num++;
		}
		
		// add new line of intermediate file
		printf("%04X\t%s\t%s\t%s\n", LOCCTR, label, opcode, operand);
		fprintf(intermediate, "%04X\t%s\t%s\t%s\n", LOCCTR, label, opcode, operand);
		
		// find opcode
		int found = find_opcode(opcode);
		int flag = is_find_op(opcode);
		if(flag) {	
			LOCCTR += 3;
		}
		else if(strcmp(opcode, "WORD") == 0) {
			LOCCTR = 3 + LOCCTR;
		}
		else if(strcmp(opcode, "RESB") == 0) {
			LOCCTR = atoi(operand) + LOCCTR;
		}
		else if(strcmp(opcode, "RESW") == 0) {
			LOCCTR = 3*atoi(operand) + LOCCTR;
		}
		else if(strcmp(opcode, "BYTE") == 0) {
			int leng = 0;
			for(i = 2; i < MAX_SIZE; i++) {
				if(operand[i] == '\'') {
					break;
				}
				leng++;
			}
			if(operand[0] == 'X') { 
				LOCCTR += (leng+1)/2;	//  1 byte = 2 hexa
			}
			else if(operand[0] == 'C') {
				LOCCTR += leng;	// 1 byte = 1 character
			}
			else {
				return ;
			}
		}
		else {
			return ;
		}
 	}
	printf("\n");
	
	//write symbol table 
	printf("symtab: \n");
	for(i = 0; i < symtab_num; i++) {
		printf("%s\t%04X\n", st[i].symtab_label, st[i].symtab_add);
		fprintf(sym, "%s\t%04X\n",  st[i].symtab_label, st[i].symtab_add);
	}
	length = LOCCTR-start;
    printf("\nLength:%x \n",length);
    printf("pass1 end ! \n");
    printf("-----------------------------------------------------------\n\n");
    return ;
}
void pass2(){
	printf("pass2 start ! \n\n");
	char label[MAX_SIZE], opcode[MAX_SIZE], operand[MAX_SIZE];
	symtab_num = 0;
    int LOCCTR = 0, i, start;
    //read symbol table
    while (fscanf(sym, "%s %X", st[symtab_num].symtab_label, &st[symtab_num].symtab_add) != EOF) {
        symtab_num++;
    }
    
    //read 1st line
    fscanf(intermediate, "%X%s%s%s", &LOCCTR, label, opcode, operand);
    if(strcmp(opcode, "START") == 0) {
    	LOCCTR = Dec(operand);
    	start = LOCCTR;
    	fprintf(objcode, "%X\t%s\t%s\t%s\n", LOCCTR, label, opcode, operand);
    	printf("%X\t%s\t%s\t%s\n", LOCCTR, label, opcode, operand);
		fprintf(objprog, "H%-6s%06s%06X\n", label, operand, length);
	}
	else {
		return;
	}
	
	// read next one line
	int obj_num = 0, obj_len = 0;
	char current_add[MAX_SIZE*MAX_SIZE];
	strcpy(current_add, "");
	getc(intermediate);
	while(1) {
		int col = 0,X = 0;LOCCTR = 0;
		char tmp[MAX_SIZE] = "";
		strcpy(label, "");
		strcpy(opcode, "");
		strcpy(operand, "");
		char ch;
   		while ((ch = getc(intermediate)) != '\n' && ch != EOF) {
	        if (ch == '\t') {
	            col++;
	        }
			else {
	            switch (col) {
	                case 0:
	                    strncat(tmp, &ch, 1);
	                    break;
	                case 1:
	                    strncat(label, &ch, 1);
	                    break;
	                case 2:
	                    strncat(opcode, &ch, 1);
	                    break;
	                case 3:
	                    if (ch == ',') {
	                        X = 1;
	                    }
	                    if (X == 0) {
	                        strncat(operand, &ch, 1);
	                    }
	                    break;
	            }
	        }
	    }
	    // omit comment
	    if(label[0] == '#') {	
			continue;
		}
		// for end
		if(strcmp(opcode, "END") == 0) {
			if(X == 1){
				strncat(operand, ",X", 2);
			}
			fprintf(objcode, "\t%s\t%s\t%s\n", label, opcode, operand);
			printf("\t%s\t%s\t%s\n", label, opcode, operand);
			fprintf(objprog, "%02X%s\nE%06X\n", obj_len, current_add, start);
			break;
		}
	    
	    int op_code = find_opcode(opcode);
	    int flag = is_find_op(opcode);
		int op_add, obj_code;
		char temp[7];
		if (flag) {
		    if (strcmp(operand, "") == 0) { 
		        op_add = 0; // operand is null
		    }
			else{// if there is an operand
		        op_add = find_symtab(operand); // find operand in symbol table
		        if (op_add == 0) { //can't find opcode 
		            return;
		        }
		    }
		    obj_code = formatObjectCode(X, op_add, op_code);
		    sprintf(temp, "%06X", obj_code);
		    strncat(current_add, temp, 6);
			obj_len += 3;
		}
		else if (strcmp(opcode, "WORD") == 0) {
		    obj_code = formatObjectCode(0, atoi(operand), 0);
		    sprintf(temp, "%06X", obj_code);
		    strncat(current_add, temp, 6);
			obj_len += 3;
		}
		else if (strcmp(opcode, "BYTE") == 0) {
		    int op_len = 0, tmplen = 0;
		    if (operand[0] == 'X') { // hexadecimal
		        char hex_op[MAX_SIZE] = "";
		        for (i = 2; operand[i] != '\''; i++) {//skip first x'
		            strncat(hex_op, &operand[i], 1);//count num of hexa
		            op_len++;
		        }
		        obj_len += (op_len + 1) / 2; // 1 hexadecimal = 0.5 byte
		        strncat(current_add, hex_op, op_len);
		        obj_code = Dec(hex_op);
		    }
			else if (operand[0] == 'C') { // character
		        for (i = 2; operand[i] != '\''; i++) {
		            obj_code = obj_code * 0x100 + operand[i];
		            op_len++;
		        }
		        obj_len += op_len; // 1 character = 1 byte
		        char tempe[op_len * 2 + 1];
		        sprintf(tempe, "%06X", obj_code);
		        strncat(current_add, tempe, op_len * 2);
		    }
			else {
		        printf("Error: invalid byte.\n");
		        return;
		    }
		}
		else if (strcmp(opcode, "RESW") != 0 && strcmp(opcode, "RESB") != 0) {
		    printf("Error: undefined opcode.\n");
		}
		LOCCTR = Dec(tmp);
		if(obj_num == 0) {
			fprintf(objprog, "T%06X", LOCCTR);// T w/ starting address
		}
		obj_num++;
		if(obj_num == 10) {// max object number is 10
			fprintf(objprog, "%02X", obj_len);// length of current object
			fprintf(objprog, "%s\n", current_add);// accumulate address to current object
			obj_num = 0; // reset new line of object number = 0
			obj_len = 0; // reset new line of object length = 0
			strcpy(current_add, ""); // reset clean current address 
		}
		if(X == 1){
			strncat(operand, ",X", 2);
		}
		fprintf(objcode, "%X\t%s\t%s\t%-15s%06X\n", LOCCTR, label, opcode, operand, obj_code);
		printf("%X\t%s\t%s\t%-15s%06X\n", LOCCTR, label, opcode, operand, obj_code);
	}
		printf("pass 2 end \n\n");
}
int Dec( char* hex) {
    int dec = 0;
    int len = strlen(hex);
    int i;
    for ( i = 0; i < len; i++) {
        int digit;
        
        if (hex[i] >= '0' && hex[i] <= '9') {
            digit = hex[i] - '0';
        } else if (hex[i] >= 'A' && hex[i] <= 'F') {
            digit = hex[i] - 'A' + 10;
        } else if (hex[i] >= 'a' && hex[i] <= 'f') {
            digit = hex[i] - 'a' + 10;
        } else {
            printf("Error: Invalid hexadecimal format '%c'\n", hex[i]);
            return -1;
        }
        
        dec = dec * 16 + digit;
    }
    
    return dec;
}
void read_opcode(){
	//count #opcode
	while (fscanf(opc, "%*s") != EOF) {
        optab_num++;
    }
	rewind(opc);// back to re-scan
	//scan opcode
	int i;
	 for (i = 0; i < optab_num; i++) {
        fscanf(opc, "%s\t%X", op[i].OPTAB_op, &op[i].OPTAB_code);
    }
}
int find_opcode(char *opcode){
	int i;
	for(i = 0; i < optab_num; i++) {
		if(strcmp(opcode, op[i].OPTAB_op) == 0) {
			return op[i].OPTAB_code;
		}
	}
	return 0;
}
int is_find_op(char *opcode){
	int i;
	for(i = 0; i < optab_num; i++) {
		if(strcmp(opcode, op[i].OPTAB_op) == 0) {
			return 1;
		}
	}
	return 0;
}
int find_symtab(char *operand){
	int i;
	for(i = 0; i < symtab_num; i++) {
		if(strcmp(operand, st[i].symtab_label) == 0) {
			return st[i].symtab_add;
		}
	}
	return 0;
}
int formatObjectCode(int x, int add, int opcode) {
	return opcode*0x10000 + x*0x8000 + add;
}

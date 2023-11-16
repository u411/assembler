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
int is_find_reg(char *operand);
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

struct REG{
    char reg_name[3];
    int reg_num;
} regi[9];

int optab_num = 1, symtab_num = 0, length;

int main(){
	// start to read files for pass1
	source = fopen("source_XE.txt", "r");
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
    intermediate = fopen("intermediate_sicxe.txt", "w");
    if (intermediate == NULL) {
        printf("Error: Unable to create the intermediate file.\n");
        fclose(source);
        fclose(opc);
        fclose(sym);
        return 1;
    }
	read_opcode();
	strcpy(regi[0].reg_name, "A");
    regi[0].reg_num = 0;
    strcpy(regi[1].reg_name, "X");
    regi[1].reg_num = 1;
    strcpy(regi[2].reg_name, "L");
    regi[2].reg_num = 2;
    strcpy(regi[3].reg_name, "B");
    regi[3].reg_num = 3;
    strcpy(regi[4].reg_name, "S");
    regi[4].reg_num = 4;
    strcpy(regi[5].reg_name, "T");
    regi[5].reg_num = 5;
    strcpy(regi[6].reg_name, "F");
    regi[6].reg_num = 6;
    strcpy(regi[7].reg_name, "PC");
    regi[7].reg_num = 8;
    strcpy(regi[8].reg_name, "SW");
    regi[8].reg_num = 9;
    
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
    intermediate = fopen("intermediate_sicxe.txt", "r");
    if (intermediate == NULL) {
        printf("Error: Unable to open the intermediate file.\n");
        fclose(source);
        fclose(opc);
        fclose(sym);
        return 1;
    }
    objcode = fopen("object_code_sicxe.txt", "w");
    if (objcode == NULL) {
        printf("Error: Unable to create the object code.\n");
        fclose(source);
        fclose(opc);
        fclose(sym);
        fclose(intermediate);
        return 1;
    }
	objprog = fopen("object_program_sicxe.txt", "w");
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
    	printf("%04X\t%s\t%s\t%s\n", LOCCTR, label, opcode, operand);
    	fprintf(intermediate, "%04X\t%s\t%s\t%s\n", LOCCTR, label, opcode, operand);
	}
	else {
		LOCCTR = 0;
	}
	getc(source);//get rid of \n
	// read next line
	while(1) {// scan label, opcode and operand
		int col = 0, e = 0, r2 = 0;
		//set default for label, opcode and operand 
		strcpy(label, "");
		strcpy(opcode, "");
		strcpy(operand, ""); 
		char reg2[MAX_SIZE] = "";
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
		            	if(ch == '+') {
							e = 1;
						}
						else {
			                strncat(opcode, &ch, 1);
			            }
		                break;
		            case 2:
						if(r2 == 1 ){
							strncat(reg2, &ch, 1);
						}
						if(ch == ','){
		            		r2 = 1;
						}
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
		if(strcmp(opcode, "BASE") == 0) {
			printf("%04X\t%s\t%s\t%s\n", LOCCTR, label, opcode, operand);
			fprintf(intermediate, "%04X\t%s\t%s\t%s\n", LOCCTR, label, opcode, operand);
			continue;
		}
		
		// make symbol table w/ label and address
		if(strcmp(label, "") != 0) {
			strcpy(st[symtab_num].symtab_label, label);
			st[symtab_num].symtab_add = LOCCTR;
			symtab_num++;
		}
		
		// add new line of intermediate file
		if( e == 1 ){
			printf("%04X\t%s\t+%s\t%s\n", LOCCTR, label, opcode, operand);
			fprintf(intermediate, "%04X\t%s\t+%s\t%s\n", LOCCTR, label, opcode, operand);
		}
		else{
			printf("%04X\t%s\t%s\t%s\n", LOCCTR, label, opcode, operand);
			fprintf(intermediate, "%04X\t%s\t%s\t%s\n", LOCCTR, label, opcode, operand);
		}
		
		// find opcode
		int found = find_opcode(opcode);
		int flag = is_find_op(opcode);
		int flag_reg2 = is_find_reg(reg2);
		if(flag) {	
			if(strcmp(opcode, "CLEAR") == 0 || flag_reg2 ==0|| flag_reg2 > 1 || is_find_reg(operand)>=0 ){
				LOCCTR += 2;
			}
			else if(e == 1) {
				LOCCTR += 4;
			}
			else{
				LOCCTR += 3;
			}
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
	for(i =0; i < 9 ; i++){
		printf("%s\t%X\n", regi[i].reg_name, regi[i].reg_num);
		fprintf(sym, "%s\t%X\n", regi[i].reg_name, regi[i].reg_num);
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
    int LOCCTR = 0, i, start, base = 0, pc, ad=0;
    //read symbol table
    while (fscanf(sym, "%s %X", st[symtab_num].symtab_label, &st[symtab_num].symtab_add) != EOF) {
        symtab_num++;
    }
    
    //read 1st line
    fscanf(intermediate, "%X%s%s%s", &LOCCTR, label, opcode, operand);
    if(strcmp(opcode, "START") == 0) {
		pc = LOCCTR;
    	LOCCTR = Dec(operand);
    	start = LOCCTR;
    	fprintf(objcode, "%04X\t%s\t%s\t%s\n", LOCCTR, label, opcode, operand);
    	printf("%04X\t%s\t%s\t%s\n", LOCCTR, label, opcode, operand);
		fprintf(objprog, "H%-6s%06s%06X\n", label, operand, length);
	}
	else {
		return;
	}
	
	// read next one line
	int obj_num = 0, obj_len = 0;
	char current_add[MAX_SIZE*MAX_SIZE], modify[MAX_SIZE*MAX_SIZE];
	int current;
	strcpy(current_add, "");
	strcpy(modify, "");
	getc(intermediate);
	char ch;
	while(ch = getc(intermediate)) {
		if( ch == '\t') {
			break;
		}
	}
	while(1) {
		int col = 0,N = 1, I = 1, X = 0, B = 0, P = 0, E = 0, format = 0;
		current = pc;
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
	                    strncat(label, &ch, 1);
	                    break;
	                case 1:
	                	if(ch == '+') {	// format 4
							E = 1;
							format = 4;
						}
						else {
							strncat(opcode, &ch, 1);
						}
	                    break;
	                case 2:
	                	if(ch == '#') {	// immediate 
							N = 0;
							I = 1;
						}
						else if(ch == '@') {// indirect 
							N = 1;
							I = 0;
						}
	                    else {
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
			fprintf(objcode, "\t%s\t%s\t%s\n", label, opcode, operand);
			printf("\t%s\t%s\t%s\n", label, opcode, operand);
			fprintf(objprog, "%02X%s\n", obj_len, current_add);
			fprintf(objprog, "%s", modify);
			fprintf(objprog, "E%06X\n", start);
			break;
		}
		
	    // read pc
	    char next[MAX_SIZE] = "";
		while(ch = getc(intermediate)) {
			if(ch == '\t' || ch == EOF) {
				break;
			}
			strncat(next, &ch, 1);
		}
		pc = Dec(next);
		// base opcode
		if(strcmp(opcode, "BASE") == 0) {
			int found_Base = find_symtab(operand);
			if(found_Base) {
				base = found_Base;
				printf("\t%s\t%s\n", opcode, operand);
				fprintf(objcode, "\t%s\t%s\n", opcode, operand);
				continue;
			}
			else {
				printf("Error: undefined base" );
				return ;
			}
		}
		LOCCTR = current;
	    int op_code = find_opcode(opcode);
	    int flag = is_find_op(opcode);
		int op_add, obj_code;
		int addr = 0, disp = 0;
		char temp[8];
		if (flag) {
		    if (strcmp(operand, "") == 0) { // operand is null
		        op_add = 0; 
		        format = 3;
		    }
			else{ // if operand
				char *reg1, *reg2;
				if(strstr(operand,",") != NULL){ // two operands
					char cut [MAX_SIZE];
					strcpy(cut, operand );
					reg1 = strtok(cut, ",");
					reg2 = strtok(NULL, ",");
					int r1 = is_find_reg (reg1);
					if(r1 >= 0){// find register1
						int r2 = is_find_reg (reg2);
						format = 2;
						obj_code = format2_ObjectCode(find_opcode(opcode), r1, r2);
					}
					else{ // cant't find register1 
						if(strcmp(reg2, "X") == 0) {
							X = 1;
						}
						op_add = find_symtab(reg1);
						if(op_add){
							disp = op_add - pc;
							if(disp >= -2048 && disp <= 2047) {	//range of PC 
								P = 1;
								format = 3;
							}
							else{
								disp = op_add - base;
								if(disp >= 0 && disp <= 4095) {	//range of base 
									B = 1;
									format = 3;
								}
								else {
									if(E == 1) {// direct address aka format 4
										//format = 4;	
										disp = op_add;
									}
									else {	// for SIC
										N = 0, I = 0;
										format = 3;
									}
									disp = atoi(operand);
								}
							}
						}
						else{ // don't find symtab
							if(N == 0 && I == 1) {	// immediate address (#)
								disp = atoi(operand);
								if(disp >= 0x1000) {
									//format = 4;
								}
								else {
									format = 3;
								}
							}
							else{
								printf("Error: undefined symbol. ");
								return ;
							}
						}
					}
				}
				else{ // one operand
					if(format != 4){
						int reg_op = is_find_reg (operand);
						if(reg_op >= 0){
							format = 2;
							obj_code = format2_ObjectCode(find_opcode(opcode), reg_op, 0);
						}
						else{
							op_add = find_symtab(operand);
							if(op_add){
								disp = op_add - pc;
								if(disp >= -2048 && disp <= 2047) {	//range of PC 
									P = 1;
									format = 3;
								}
								else{
									disp = op_add - base;
									if(disp >= 0 && disp <= 4095) {	//range of base 
										B = 1;
										format = 3;
									}
									else {
										if(E == 1) {// direct address aka format 4
											//format = 4;	
											disp = op_add;
										}
										else {	// for SIC
											N = 0, I = 0;
											format = 3;
										}
									}
								}
							}
							else{ // don't find symtab
								if(N == 0 && I == 1) {	
									disp = atoi(operand);
									if(disp >= 0x1000) {
										//format = 4;
									}
									else {
										format = 3;
									}
								}
								else{
									printf("Error: undefined symbol. ");
									return ;
								}
							}
							
						}
						
					}
					else{
						if(N == 0 && I == 1){
							addr = atoi(operand);
						}
						else{
							addr = find_symtab(operand);
						}
					}
				}
		    }
		    if(format == 3) {
				obj_code = format3_ObjectCode(find_opcode(opcode), N, I, X, B, P, E, disp);
			}
			if(format == 4) {
				obj_code = format4_ObjectCode(find_opcode(opcode), N, I, X, B, P, E, addr);
			}
		}
		else if (strcmp(opcode, "WORD") == 0) {
		    obj_code = atoi(operand);
		    sprintf(temp, "%06X", obj_code);
		    strncat(current_add, temp, 6);
			obj_len += 3;
			//format = 3; 
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
		        fprintf(objcode, "%04X\t%s\t%s\t%-15s%s\n", LOCCTR, label, opcode, operand, hex_op);
				printf("%04X\t%s\t%s\t%-15s%s\n", LOCCTR, label, opcode, operand, hex_op);
		        obj_code = Dec(hex_op);
		        format = -1;
		    }
			else if (operand[0] == 'C') { // character
				obj_code = 0;
		        for (i = 2; operand[i] != '\''; i++) {
		            obj_code = obj_code * 0x100 + operand[i];
		            op_len++;
		        }
		        fprintf(objcode, "%04X\t%s\t%s\t%-15s%X\n", LOCCTR, label, opcode, operand, obj_code);
				printf("%04X\t%s\t%s\t%-15s%X\n", LOCCTR, label, opcode, operand, obj_code);
		        obj_len += op_len; // 1 character = 1 byte
		        //format = op_len;
		        if(op_len == 1){
					sprintf(temp, "%02X", obj_code);
				}
				if(op_len == 2){
					sprintf(temp, "%04X", obj_code);
				}
				if(op_len == 3){
					sprintf(temp, "%06X", obj_code);
				}
		        format = -1;
		    }
			else {
		        printf("Error: invalid byte.\n");
		        return;
		    }
		}
		else if (strcmp(opcode, "RESW") != 0 && strcmp(opcode, "RESB") != 0) {
		    printf("Error: undefined opcode.\n");
		}
		
		if (strcmp(opcode, "CLEAR") == 0 && ad < 2){
			ad++;
		}
		
		if(obj_num == 0) {
			fprintf(objprog, "T%06X", LOCCTR);// T w/ starting address
		}
		obj_num++;
		if(I == 1 && N ==0){
			char t[MAX_SIZE]="#";
			strcat(t, operand);
			strcpy(operand,t);
		}
		if(I == 0 && N ==1){
			char t[MAX_SIZE]="@";
			strcat(t, operand);
			strcpy(operand,t);
		}
		if(E == 1 && I == 1 && N == 1) { // direct address aka format 4	
			char md[10];// append modification record
			sprintf(md, "M%06X05\n", LOCCTR + 1);
			strncat(modify, md, 10);
		}
		if(format>0){
			obj_len+=format;
		}
		if(format == 0) {
			fprintf(objcode, "%04X\t%s\t%s\t%-15s\n", LOCCTR, label, opcode, operand, obj_code);
			printf("%04X\t%s\t%s\t%-15s\n", LOCCTR, label, opcode, operand, obj_code);
			//sprintf(temp, "%02X", obj_code);
		}
		else if(format == 1) {
			fprintf(objcode, "%04X\t%s\t%s\t%-15s%X\n", LOCCTR, label, opcode, operand, obj_code);
			printf("%04X\t%s\t%s\t%-15s%X\n", LOCCTR, label, opcode, operand, obj_code);
			sprintf(temp, "%02X", obj_code);
		}
		else if(format == 2) {
			fprintf(objcode, "%04X\t%s\t%s\t%-15s%04X\n", LOCCTR, label, opcode, operand, obj_code);
			printf("%04X\t%s\t%s\t%-15s%04X\n", LOCCTR, label, opcode, operand, obj_code);
			sprintf(temp, "%04X", obj_code);
		}
		else if(format == 3) {
			fprintf(objcode, "%04X\t%s\t%s\t%-15s%06X\n", LOCCTR, label, opcode, operand, obj_code);
			printf("%04X\t%s\t%s\t%-15s%06X\n", LOCCTR, label, opcode, operand, obj_code);			
			sprintf(temp, "%06X", obj_code);
		}
		else if(format == 4) {
			fprintf(objcode, "%04X\t%s\t%s\t%-15s%08X\n", LOCCTR, label, opcode, operand, obj_code);
			printf("%04X\t%s\t+%s\t%-15s%08X\n", LOCCTR, label, opcode, operand, obj_code);
			sprintf(temp, "%08X", obj_code);	
		}
		strcat(current_add, temp);
		strcpy(temp, "");
		if(obj_num == 9 + ad) {// max object number is 10
			fprintf(objprog, "%02X", obj_len);// length of current object
			fprintf(objprog, "%s\n", current_add);// accumulate address to current object
			obj_num = 0; // reset new line of object number = 0
			obj_len = 0; // reset new line of object length = 0
			strcpy(current_add, ""); // reset clean current address 
		}
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
int is_find_reg(char *operand){
	int i;
	for(i = 0; i < 9; i++) {
		if(strcmp(operand, regi[i].reg_name) == 0) {
			return regi[i].reg_num;
		}
	}
	return -1;
}
int format2_ObjectCode(int opcode, int r1, int r2) {
	return opcode*0x100 + r1*0x10 + r2;
}
int format3_ObjectCode(int opcode, int N, int I, int X, int B, int P, int E, int disp) {
	return opcode*0x10000 + N*0x20000 + I*0x10000 + X*0x8000 + B*0x4000 + P*0x2000 + E*0x1000 + (disp&0x00FFF);
}
int format4_ObjectCode(int opcode, int N, int I, int X, int B, int P, int E, int add) {
	return opcode*0x1000000 + N*0x2000000 + I*0x1000000 + X*0x800000 + B*0x400000 + P*0x200000 + E*0x100000 + add;
}

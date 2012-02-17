/*LC4 Simulator
*Author: Alen Kubati
*CIS 240
*/


#include <stdio.h>
#include <string.h>
//Registers for PennSim

unsigned short R[8];
//unsigned short R0;
//unsigned short R1;
//unsigned short R2;
//unsigned short R3;
//unsigned short R4; 
//unsigned short R5;
//unsigned short R6; 
//unsigned short R7; 
unsigned int PC = 0;
//unsigned int PC = 0x8200;
unsigned int PSR = 0x8002;
char *CC = "Z";

//The Memory Array

unsigned short Memory[65536];

//PennSim commands

char reset_com[] = "reset";
char load_com[] = "load";
char set_com[] = "set";
char Break_com[] = "break";
char step_com[] = "step";
char Continue_com[] = "continue";
char print_com[] = "print";
char script_com[] = "script";
char quit_com[] = "quit";

#define swap(I) ((I) << 8) | ((I) >> 8);

//Loads a binary file into the array
void load(char *arg){
  R[0] = 5;
  R[1] = 3;
  FILE *binFile = fopen(arg, "rb");
  unsigned short num, i, read=1;
  unsigned short header, instruction;
  unsigned short address, nword, line, file_index;
  if (binFile == NULL) {
    printf ("Couldn't open file\n");
    }

  //i = 0;
  //do {
    //num1 = fgetc(binFile);
    //++i;
  //} while (num1 != EOF);

  //rewind (binFile);

  while(read == 1){
     read = fread(&num, 2, 1, binFile);
     header = swap(num);

     //Check if the header equals one of the object headers

     //Takes care of the .CODE part of the object file
     if (header == 0xCADE){
      read = fread(&num, 2, 1, binFile);
      address = swap(num);
      read = fread(&num, 2, 1, binFile);
      nword = swap(num); 
      while (nword > 0){
	fread(&num, 2, 1, binFile);
	instruction = swap(num)
	Memory[address] = instruction;
	address++;
	nword--;
      }
    }

     //Takes care of the .DATA part of the object file
     else if (header ==  0xDADA){
       read = fread(&num, 2, 1, binFile);
       address = swap(num);
       read = fread(&num, 2, 1, binFile);
       nword = swap(num);
       while (nword > 0){
	read = fread(&num, 2, 1, binFile);
	instruction = swap(num);
        Memory[address] = instruction;
	address++;
	nword--;
       }
     }

     //Skips over the Symbol part of the object file
     else if (header == 0xC3B7){
       read = fread(&num, 2, 1, binFile);
       address = swap(num);
       read = fread(&num, 2, 1, binFile);
       nword = swap(num);
       while (nword > 0){
	  read = fread(&num, 1, 1, binFile);
	  nword--;
       }
     }

     //Skips over the File name part of the object file
     else if (header == 0xF17E){
       read = fread(&num, 2, 1, binFile);
       nword = swap(num);
       while (nword > 0){
	 read = fread(&num, 1, 1, binFile);
	 nword--;
       }
     }

     //Skips over the Line number part of the object file
     else if (header ==  0x715E){
       read = fread(&num, 2, 1, binFile);
       address = swap(num);
       read = fread(&num, 2, 1, binFile);
       line = swap(num);
       read = fread(&num, 2, 1, binFile);
       file_index = swap(num);
     }
     else{
     }
     
    // printf ("Before %6d = %04x\n", i, num);
  //   printf ("Integer %6d = %04x\n", i, header);
//     i++;
 }
 
//    for (i=0; i<10; i++){
//	printf("Memory at %5d = %04x\n", i, Memory[i]);
  //   }
}



//prints the values of the registers
void print(){
  printf("[R0: x%04x,R1: x%04x,R2: x%04x,R3: x%04x,R4: x%04x,R5: x%04x,R6: x%04x,R7: x%04x]\n", R[0], R[1], R[2], R[3], R[4], R[5], R[6], R[7]);
  printf("PC =  x%04x\n", PC);
  printf("PSR = x%04x\n", PSR);
  printf("CC = %s\n", CC);
}

//resets every register and memory location to 0
void reset(){
  R[0] = 0;
  R[1] = 0;
  R[2] = 0;
  R[3] = 0;
  R[4] = 0;
  R[5] = 0;
  R[6] = 0;
  R[7] = 0;
  PC = 0x8200;
  PSR = 0x8002;
  if ((PSR) & 4){
    CC = "N";
  }
  else if ((PSR) & 2){
    CC = "Z";
  }
  else{
    CC = "P";
  }
  int i;
  for (i=0; i<65536; i++){
    Memory[i] = 0;
  }
  printf("System reset\n");
}

/*This section is marked for the beginning and the end of the LC4 Instruction Set
 *
 *
 *
 */
#define SEXT(I) (((I) >> 8) & 1);
#define SEXT2(I) ((I) | 0xFE00);
#define SEXT3(I) (((I) >> 6) & 1);
#define SEXT4(I) ((I) | 0xFF80);
#define SEXT5(I) (((I) >> 4) & 1);
#define SEXT6(I) ((I) | 0xFFE0);

void BRn(signed short IMM){
  signed short IMM2 = SEXT(IMM);
  if (IMM2 == 1){
    IMM = SEXT2(IMM);
  }
  if ((PSR & 4) == 4){
    PC = PC + 1 + IMM;
  }
  else{
    PC = PC + 1;
  }
}

void BRnz(signed short IMM){
 
  signed short IMM2 = SEXT(IMM);
  if (IMM2 == 1){
    IMM = SEXT2(IMM);
  }
  if ((PSR & 4) == 4 || (PSR & 2) == 2){
    PC = PC + 1 + IMM;
  }
  else{
    PC++;
  }
}

void BRnp(signed short IMM){

  signed short IMM2 = SEXT(IMM);
  if (IMM2 == 1){
    IMM = SEXT2(IMM);
  }
  if ((PSR & 4) == 4 || (PSR & 1) == 1){
    PC = PC + 1 + IMM;
  }
  else{
    PC++;
  }
}
void BRz(signed short IMM){

  signed short IMM2 = SEXT(IMM);
  if (IMM2 == 1){
    IMM = SEXT2(IMM);
  }
  if ((PSR & 2) == 2){
    PC = PC + 1 + IMM;
  }
  else{
    PC++;
  }
}

void BRzp(signed short IMM){

  signed short IMM2 = SEXT(IMM);
  if (IMM2 == 1){
    IMM = SEXT2(IMM);
  }
  if ((PSR & 2) == 2 || (PSR & 1) == 1){
    PC = PC + 1 + IMM;
  }
  else{
    PC++;
  }
}

void BRp(signed short IMM){

  signed short IMM2 = SEXT(IMM);
  if (IMM2 == 1){
    IMM = SEXT2(IMM);
  }
  if ((PSR & 1)==1){
    PC = PC + 1 + IMM;
  }
  else{
    PC++;
  }
}

void BRnzp(signed short IMM){

  signed short IMM2 = SEXT(IMM);
  if (IMM2 == 1){
    IMM = SEXT2(IMM);
  }
  PC = PC + 1 + IMM;
}

void ADDREG(unsigned short Rd, unsigned short Rs, unsigned short Rt){
  R[Rd] = R[Rs] + R[Rt];
  PC++;
}

void MUL(unsigned short Rd, unsigned short Rs, unsigned short Rt){
  R[Rd] = R[Rs] * R[Rt];
  PC++;
}

void SUB(unsigned short Rd, unsigned short Rs, unsigned short Rt){
  R[Rd] = R[Rs] - R[Rt];
  PC++;
}

void DIV(unsigned short Rd, unsigned short Rs, unsigned short Rt){
  R[Rd] = R[Rs] / R[Rt];
  PC++;
}

void ADDIMM(unsigned short Rd, unsigned short Rs, signed short IMM){
  signed short IMM2 = SEXT5(IMM);
  if (IMM2 == 1){
    IMM = SEXT6(IMM);
  }
  R[Rd] = R[Rs] + IMM;
  PC++;
}

#define PSRSetterN(I) (((I) << 13) & 4) 
#define PSRSetterZ(I) (((I) << 13) & 2) 
#define PSRSetterP(I) (((I) << 13) & 1) 

void CMP(unsigned short Rd, unsigned short Rt){
 unsigned short N = PSRSetterN(PSR);
 unsigned short Z = PSRSetterZ(PSR);
 unsigned short P = PSRSetterP(PSR);
 signed short S = (signed)R[Rd] - (signed)R[Rt];
 if (S<0){
  CC="N";
  if (((PSR >> 15) | 1) == 1){
    PSR = (0 | 0x8004);
  }
  else{
    PSR = (0 | 0x0004);
  }

 }

 else if (S==0){
  CC="Z";
  if (((PSR >> 15) | 1) == 1){
    PSR = (0 | 0x8002);
  }
  else{
    PSR = (0 | 0x0002);
  }
 }
 else{
  CC="P";
  if (((PSR >> 15) | 1) == 1){
    PSR = (0 | 0x8001);
  }
  else{
    PSR = (0 | 0x0001);
  }
 }
  PC++;
}

void CMPU(unsigned short Rd, unsigned short Rt){
 unsigned short N = PSRSetterN(PSR);
 unsigned short Z = PSRSetterZ(PSR);
 unsigned short P = PSRSetterP(PSR);
 unsigned short S = (signed)R[Rd] - (signed)R[Rt];
  
 if (S<0){
  CC="N";
  if (((PSR >> 15) | 1) == 1){
    PSR = (0 | 0x8004);
  }
  else{
    PSR = (0 | 0x0004);
  }

 }

 else if (S==0){
  CC="Z";
  if (((PSR >> 15) | 1) == 1){
    PSR = (0 | 0x8002);
  }
  else{
    PSR = (0 | 0x0002);
  }
 }
 else{
  CC="P";
  if (((PSR >> 15) | 1) == 1){
    PSR = (0 | 0x8001);
  }
  else{
    PSR = (0 | 0x0001);
  }
 }
  PC++;
}

void CMPI(unsigned short Rd, signed short IMM){
 unsigned short N = PSRSetterN(PSR);
 unsigned short Z = PSRSetterZ(PSR);
 unsigned short P = PSRSetterP(PSR);
 signed short IMM2 = SEXT3(IMM);
 if (IMM2==1){
   IMM = SEXT4(IMM);
 }
 signed short S = (signed)R[Rd] - (signed)IMM;
 
 if (S<0){
  CC="N";
  if (((PSR >> 15) | 1) == 1){
    PSR = (0 | 0x8004);
  }
  else{
    PSR = (0 | 0x0004);
  }

 }

 else if (S==0){
  CC="Z";
  if (((PSR >> 15) | 1) == 1){
    PSR = (0 | 0x8002);
  }
  else{
    PSR = (0 | 0x0002);
  }
 }
 else{
  CC="P";
  if (((PSR >> 15) | 1) == 1){
    PSR = (0 | 0x8001);
  }
  else{
    PSR = (0 | 0x0001);
  }
 }
  PC++;
}

void CMPIU(unsigned short Rd, unsigned short IMM){
 unsigned short N = PSRSetterN(PSR);
 unsigned short Z = PSRSetterZ(PSR);
 unsigned short P = PSRSetterP(PSR);
 unsigned short S = R[Rd] - IMM;
 
 if (S<0){
  CC="N";
  if (((PSR >> 15) | 1) == 1){
    PSR = (0 | 0x8004);
  }
  else{
    PSR = (0 | 0x0004);
  }

 }

 else if (S==0){
  CC="Z";
  if (((PSR >> 15) | 1) == 1){
    PSR = (0 | 0x8002);
  }
  else{
    PSR = (0 | 0x0002);
  }
 }
 else{
  CC="P";
  if (((PSR >> 15) | 1) == 1){
    PSR = (0 | 0x8001);
  }
  else{
    PSR = (0 | 0x0001);
  }
 }
  PC++;
}

void JSR(signed short IMM){
  R[7] = PC + 1;
  PC = (PC & 0x8000) | (IMM << 4);
}

void JSRR(unsigned short Rs){
  R[7] = PC +1;
  PC = Rs;
}

void AND(unsigned short Rd, unsigned short Rs, unsigned short Rt){
  R[Rd] = R[Rs] & R[Rt];
  PC++;
}
void NOT(unsigned short Rd, unsigned short Rs){
  R[Rd] = ~R[Rs];
  PC++;
}
void OR(unsigned short Rd, unsigned short Rs, unsigned short Rt){
  R[Rd] = R[Rs] | R[Rt];
  PC++;
}
void XOR(unsigned short Rd, unsigned short Rs, unsigned short Rt){
  R[Rd] = R[Rs] ^ R[Rt];
  PC++;
}


void ANDIMM(unsigned short Rd, unsigned short Rs, signed short IMM){
  signed short IMM2 = SEXT5(IMM);
  if (IMM2 == 1){
    IMM = SEXT6(IMM);
  }
  R[Rd] = R[Rs] & IMM;
  PC++;
}

#define SEXTFOR6(I) (((I) >> 5) & 1);
#define SEXTFOR62(I) ((I) | 0xFFC0);
void LDR(unsigned short Rd, unsigned short Rs, signed short IMM){
  signed short IMM2 = SEXTFOR6(IMM);
  if (IMM2 == 1){
    IMM = SEXTFOR62(IMM);
  }
  R[Rd] = Memory[Rs+IMM];
  PC++;
}

void STR(unsigned short Rd, unsigned short Rs, signed short IMM){
  signed short IMM2 = SEXTFOR6(IMM);
  if (IMM2 == 1){
    IMM = SEXTFOR62(IMM);
  }
  Memory[Rs+IMM] = R[Rd];
  PC++;
}

void RTI(){
  PC = R[7];
  unsigned short Most_Significant = PSR >> 15;
  if (Most_Significant == 1){
    PSR = PSR ^ 0x8000;
  }
}

void CONST(unsigned short Rd, signed short IMM){
  signed short IMM2 = SEXT(IMM);
  if (IMM2 == 1){
   IMM = SEXT(IMM);
  }
  R[Rd] = IMM;
  PC++;
}

void SLL(unsigned short Rd, unsigned short Rs, unsigned short IMM){
  R[Rd] = R[Rs] << IMM;
  PC++;
}

void SRA(unsigned short Rd, unsigned short Rs, unsigned short IMM){
  unsigned short Most_Significant = Rs >> 15;
  R[Rd] = R[Rs] >> IMM;
  unsigned short Second_Most_Significant = R[Rd] >> 15;
  if (Most_Significant != Second_Most_Significant){
    R[Rd] = R[Rd] ^ 0x8000;
  }
  PC++;
}

void SRL(unsigned short Rd, unsigned short Rs, unsigned short IMM){
  R[Rd] = R[Rs] >> IMM;
  PC++;
}

void MOD(unsigned short Rd, unsigned short Rs, unsigned short Rt){
  R[Rd] = R[Rs]%R[Rt];
  PC++;
}

void JMPR(unsigned short Rs){
  PC = Rs;
}

#define SEXTFOR11(I) (((I) >> 10) & 1);
#define SEXTFOR112(I) ((I) | 0xFFC0);
void JMP(signed short IMM){
  IMM = (unsigned)IMM;
  signed short IMM2 = SEXTFOR11(IMM);
  if (IMM2 == 1){
    IMM = SEXTFOR112(IMM);
  }
  PC = PC + 1 +IMM;
}

void HICONST(unsigned short Rd, unsigned char IMM){
  R[Rd] = (Rd & 0xFF) | (IMM << 8);
  PC++;
}

void TRAP(unsigned char IMM){
  R[7] = PC + 1;
  PC = (0x8000|IMM);
  unsigned short Most_Significant = PSR >> 15;
  if (Most_Significant == 0){
    PSR = PSR | 0x8000;
  }
}

//End of LC4 Instuction Set

//Implements the step() function in PennSim
int step(){
  unsigned short current_instruction;
  current_instruction = Memory[PC];
  if (current_instruction == 0x0000){
    return 1;
  }

 #define opcode(I) (((I) >> 9) & 127);
  #define IMM9(I) ((I) & 511);
  unsigned short opcode = opcode(current_instruction);
  signed short IMM9 = IMM9(current_instruction);
  if (opcode == 4){
    BRn(IMM9);
  }
  if (opcode == 6){
    BRnz(IMM9);
  }
  if (opcode == 5){
    BRnp(IMM9);
  }
  if (opcode == 2){
    BRz(IMM9);
  }
  if (opcode == 3){
    BRzp(IMM9);
  }
  if (opcode == 1){
    BRp(IMM9);
  }
  if (opcode == 7){
    BRnzp(IMM9);
  }

  #define opcode1(I) (((I) >> 12) & 15);
  #define subopcode(I) (((I) >> 3) & 7);
  #define Rd(I) (((I) >> 9) & 7);
  #define Rs(I) (((I) >> 6) & 7);
  #define Rt(I) ((I) & 7);
  #define subopcode1(I) (((I) >> 5) & 1);
  #define IMM5(I) ((I) & 31);

  unsigned short opcode1 = opcode1(current_instruction);
  unsigned short subopcode = subopcode(current_instruction);
  unsigned short Rd = Rd(current_instruction);
  unsigned short Rs = Rs(current_instruction);
  unsigned short Rt = Rt(current_instruction);
  unsigned short subopcode1 = subopcode1(current_instruction);
  signed short IMM5 = IMM5(current_instruction);

  if ((opcode1==1) && (subopcode==0)){
    ADDREG(Rd, Rs, Rt);
  }
  if ((opcode1==1) && (subopcode==1)){
    MUL(Rd, Rs, Rt);
  }
  if ((opcode1==1) && (subopcode==2)){
    SUB(Rd, Rs, Rt);
  }
  if ((opcode1==1) && (subopcode==3)){
    DIV(Rd, Rs, Rt);
  }
  if ((opcode1==1) && (subopcode1==1)){
    ADDIMM(Rd, Rs, IMM5);
  }

  #define subopcode2(I) (((I) >> 7) & 3);
  #define IMM7(I) ((I) & 127);

  unsigned short subopcode2 = subopcode2(current_instruction);
  unsigned short IMM7 = IMM7(current_instruction);

  if ((opcode1==2) && (subopcode2==0)){
    CMP(Rd, Rt);
  }
  if ((opcode1==2) && (subopcode2==1)){
    CMPU(Rd, Rt);
  }
  if ((opcode1==2) && (subopcode2==2)){
    CMPI(Rd, (signed)IMM7);
  }
  if ((opcode1==2) && (subopcode2==3)){
    CMPIU(Rd, IMM7);
  }

  #define opcode2(I) (((I) >> 11) & 31);
  #define IMM11(I) ((I) & 2047);

  unsigned short opcode2 = opcode2(current_instruction);
  signed short IMM11 = IMM11(current_instruction);

  if (opcode2==9){
    JSR(IMM11);
  }
  if (opcode2==8){
    JSRR(Rs);
  }

  //Logical Operators
  if ((opcode1==5) && (subopcode==0)){
    AND(Rd, Rs, Rt);
  }
  if ((opcode1==5) && (subopcode==1)){
    NOT(Rd, Rs);
  }
  if ((opcode1==5) && (subopcode==2)){
    OR(Rd, Rs, Rt);
  }
  if ((opcode1==5) && (subopcode==3)){
    XOR(Rd, Rs, Rt);
  }
 if ((opcode1==5) && (subopcode1==1)){
    ANDIMM(Rd, Rs, IMM5);
 }

 #define IMM6(I) ((I) & 63);

 signed short IMM6 = IMM6(current_instruction);
 //Loading and Storing
 if ((opcode1==6)){
    LDR(Rd, Rs, IMM6);
 }
 if ((opcode1==7)){
    STR(Rd, Rs, IMM6);
 }

 //RTI
 if ((opcode1==8)){
    RTI();
 }

 //CONST
 if ((opcode1==9)){
    CONST(Rd, IMM9);
 }

 //The Shift and the MOD
#define IMM4(I) ((I) & 15);
#define subopcode3(I) (((I) >> 4) & 3);

 unsigned short subopcode3 = subopcode3(current_instruction);
 unsigned short IMM4 = IMM4(current_instruction);
 
 if ((opcode1==10) && (subopcode3==0)){
   SLL(Rd, Rs, IMM4);
 }
 if ((opcode1==10) && (subopcode3==1)){
   SRA(Rd, Rs, IMM4);
 }
 if ((opcode1==10) && (subopcode3==2)){
   SRL(Rd, Rs, IMM4);
 }
 if ((opcode1==10) && (subopcode3==3)){
   MOD(Rd, Rs, Rt);
 }

  //JMP and JMPR
  
  if ((opcode2==24)){
    JMPR(Rs);
  }
  if ((opcode2==25)){
    JMP(IMM11);
  }

  //HICONST

  #define IMM8(I) ((I) & 255);

  unsigned char IMM8 = IMM8(current_instruction);
  if (opcode1==13){
    HICONST(Rd, IMM8);
  }

  //TRAP

  if (opcode1==15){
   TRAP(IMM8);
  }
  return 0;
}

//The set method
void set(char *arg1, char *arg2){
  int len = strlen(arg2);
 
  if (strcmp(arg1, "R0") == 0){
    R[0] = *(unsigned short*) arg2;
    R[0] = R[0] - 15*15*15;
  }

  if (strcmp(arg1, "R1") == 0){
    R[1] = *(unsigned short*) arg2;
    R[1] = R[1] - 15*15*15;
  }
	
  if (strcmp(arg1, "R2") == 0){
    R[2] = *(unsigned short*) arg2;
    R[2] = R[2] - 15*15*15;
  }

  if (strcmp(arg1, "R3") == 0){
    R[3] = *(unsigned short*) arg2;
    R[3] = R[3]  - 15*15*15;
  }

  if (strcmp(arg1, "R4") == 0){
    R[4] = *(unsigned short*) arg2;
    R[4] = R[4] - 15*15*15;
  }

  if (strcmp(arg1, "R5") == 0){
    R[5] = *(unsigned short*) arg2;
    R[5] = R[5] - 15*15*15;
  }

  if (strcmp(arg1, "R6") == 0){
    R[6] = *(unsigned short*) arg2;
    R[6] = R[6] - 15*15*15;
  }

  if (strcmp(arg1, "R7") == 0){
    R[7] = *(unsigned short*) arg2;
    R[7] = R[7] - 15*15*15;
  }

  if (strcmp(arg1, "PC") == 0){
    PC = *(unsigned short*) arg2;
    PC = PC - 15*15*15;
  }

  if (strcmp(arg1, "PSR") == 0){
    PSR = *(unsigned short*) arg2;
    PC = PC - 15*15*15;
  }
}



//Method that simulates the whole thing
int main()
{

//Reading the input
while(1){
    char string[255];
    char *command;
    fputs("Please enter a command: ", stdout);
    fflush(stdout);
   // fgets(string, sizeof string, stdin);
  
   //Stripping the new line
   if (fgets(string, sizeof string, stdin) != NULL){
      char *n = strchr(string, '\n');
      if (n != NULL){
       *n = '\0';
      }
    }

    command = strtok(string, " ");
//   printf("%s\n", command);
 //   char *argum = strtok(NULL, " ");
   // printf("%s\n", argum);

   //Parsing the commands

   //Resets the memory and the registers
    if (strcmp(command, reset_com) == 0){
      reset();	    
    }

    //Loads the object file
    else if (strcmp(command, load_com) == 0){
      char *arg = strtok(NULL, " ");
      load(arg);
    }

    else if (strcmp(command, set_com) == 0){
      char *arg1 = strtok(NULL, " ");
      char *arg2 = strtok(NULL, " ");
      set(arg1, arg2);
	 }

    else if (strcmp(command, Break_com) == 0){}

    //Steps through the instructions
    else if (strcmp(command, step_com) == 0){
      if (step() == 1){
       // break;
      }
    }
    else if (strcmp(command, Continue_com) == 0){}
    else if (strcmp(command, print_com) == 0){
      print();	    
    }
    else if (strcmp(command, script_com) == 0){}
    else if (strcmp(command, quit_com) == 0){
	break;
    }
    else{
      printf("Unknown command: %s\n", command);
    }
}
return 0;

}

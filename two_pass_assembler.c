// This program emulates a two-pass assembler for a SIC Machine. It
// requires the presence of an 'assemble.txt' file in C:\CPROGS directory
// containing a program written for said machine. It then 'assembles' the
// code present in the file and outputs the object code into 'objcode.txt',
// also created in the same directory.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define APOS 0x27

// GLOBAL VARIABLES

char OPTAB[18][7] = {"STL","JSUB","LDA","COMP","JEQ","J","STA","LDL","RSUB","LDX","TD","RD","STCH","JLT","STX","LDCH","WD","TIX"};
char OPVAL[18][3] = {"14","48","00","28","30","3C","0C","08","4C","04","E0","D8","54","38","10","50","DC","2C"};

char SYMTAB[20][10];
int SYMVAL[20], numberOfSymbols = 0;

int OPTABVal, SYMTABVal;

char outputString[30];

char gotLine[50][50], OPCODE[20], OPERAND[20], LABEL[20];

int nLines = 0, programLength, startAddress = 0, currentAddress = 0, progress = 0;

FILE *fpIN, *fpOUT;

char startAddressAsString[7] = {'0','0','0','0','\0'};

int processFile() {
    char c;
    int location = 0, endNow = 0;
    if((fpIN = fopen("c:/cprogs/assemble.txt", "r+")) == NULL) {
        printf("Error! File not found!\n" );
        return(1);
    }

    printf("File Opened! Reading contents...\n\n");
    getch();

    c = getc(fpIN);
    while(endNow != 1) {
        if(c != '\n') {
            gotLine[nLines][location] = c;
            location++;
        }
        else {
            gotLine[nLines][location] = '\0';
            location = 0;
            while(gotLine[nLines][location] != '\0') {
                printf("%c", gotLine[nLines][location]);
                location++;
            }
            printf("\n");
            location = 0;
            nLines++;
        }
        if((c = getc(fpIN)) == EOF) {
            endNow = 1;
        }
    }
    printf("File has ended! Contents have been stored in local array. Closing file...");
    getch();
    fclose(fpIN);
    return(0);
}

void getCodesFromLine(int line) {
    int location = 0, nWords = 1, inLoc = 0;
    while(gotLine[line][location] != '\0') {
        if(gotLine[line][location] == ' ') {
            nWords++;
        }
        location++;
    }
    location = 0;
    if(nWords > 2) {
        while(gotLine[line][location] != ' ') {
            LABEL[inLoc] = gotLine[line][location];
            location++;
            inLoc++;
        }
        LABEL[inLoc] = '\0';
        inLoc = 0;
        location++;
        while(gotLine[line][location] != ' ') {
            OPCODE[inLoc] = gotLine[line][location];
            location++;
            inLoc++;
        }
        OPCODE[inLoc] = '\0';
        inLoc = 0;
        location++;
        while(gotLine[line][location] != '\0') {
            OPERAND[inLoc] = gotLine[line][location];
            location++;
            inLoc++;
        }
        OPERAND[inLoc] = '\0';
    }
    else if(nWords > 1) {
        while(gotLine[line][location] != ' ') {
            OPCODE[inLoc] = gotLine[line][location];
            inLoc++;
            location++;
        }
        OPCODE[inLoc] = '\0';
        inLoc = 0;
        location++;
        while(gotLine[line][location] != '\0') {
            OPERAND[inLoc] = gotLine[line][location];
            inLoc++;
            location++;
        }
        OPERAND[inLoc] = '\0';

        LABEL[0] = 'N';
        LABEL[1] = 'U';
        LABEL[2] = 'L';
        LABEL[3] = 'L';
        LABEL[4] = '\0';
    }
    else {
        while(gotLine[line][location] != ' ' && gotLine[line][location] != '\0') {
            OPCODE[inLoc] = gotLine[line][location];
            inLoc++;
            location++;
        }
        OPCODE[inLoc] = '\0';

        OPERAND[0] = 'N';
        OPERAND[1] = 'U';
        OPERAND[2] = 'L';
        OPERAND[3] = 'L';
        OPERAND[4] = '\0';

        LABEL[0] = 'N';
        LABEL[1] = 'U';
        LABEL[2] = 'L';
        LABEL[3] = 'L';
        LABEL[4] = '\0';
    }
}

int opInt() {
    int b = 0, i = 0;
    while(OPERAND[i] != '\0') {
        b = b + OPERAND[i] - 48;
        b*=10;
        i++;
    }
    b/=10;
    return(b);
}

int isDuplicateLabel() {
    int i = 0;
    while(i < 18) {
        if(strcmp(LABEL, SYMTAB[i]) == 0)
            return(1);
        i++;
    }
    return(0);
}

int isOPERANDinSYMTAB() {
    int i = 0;
    while(i < numberOfSymbols) {
        if(strcmp(OPERAND, SYMTAB[i]) == 0) {
            SYMTABVal = SYMVAL[i];
            return(1);
        }
        i++;
    }
    return(0);
}

int isOPCODEValid() {
    int i = 0;
    while(i < 18) {
        if(strcmp(OPCODE, OPTAB[i]) == 0) {
            OPTABVal = i;
            return(1);
        }
        i++;
    }
    return(0);
}

void insertIntoSYMTAB(int location) {
    int i = numberOfSymbols;
    numberOfSymbols++;
    strcpy(SYMTAB[i], LABEL);
    SYMVAL[i] = location;
}

void initOutputFile() {
    char output[50] = {'H','\0'}, start[8], length[8], endline[2] = {'\n', '\0'};
    int nameLength, startLength, lengthLength;

    printf("\n\nInserting header record... Name: %s", LABEL);

    fpOUT = fopen("c:/cprogs/objcode.txt", "w+");

    itoa(startAddress, start, 10);
    itoa(programLength, length, 10);

    printf("\nStarting address: %s\nProgram Length: %s\n", start, length);

    nameLength = strlen(LABEL) + 1;
    startLength = strlen(start) + 1;
    lengthLength = strlen(length) + 1;

    strcat(output, LABEL);
    while(nameLength <= 7) {
        output[nameLength] = ' ';
        nameLength++;
    }
    while(startLength < 7) {
        output[nameLength] = '0';
        startLength++;
        nameLength++;
    }
    output[nameLength] = '\0';
    strcat(output, start);
    nameLength += startLength;
    while(lengthLength < 7) {
        output[nameLength] = '0';
        lengthLength++;
        nameLength++;
    }
    output[nameLength] = '\0';
    strcat(output, length);
    nameLength += lengthLength;

    output[nameLength] = '\0';
    strcat(output, endline);
    fputs(output, fpOUT);
}

void writeToFile(int lengthOfString) {
    char output[50] = {'T','\0'}, start[8], length[3], endline[2] = {'\n', '\0'};
    itoa(lengthOfString, length, 10);
    currentAddress += progress;
    progress = lengthOfString;
    itoa(currentAddress, start, 10);
    strcat(output, start);
    strcat(output, length);
    strcat(output, outputString);
    strcat(output, endline);
    fputs(output, fpOUT);
}

void writeEndRecordAndClose() {
    char output[50] = {'E','0','0','\0'};
    strcat(output, startAddressAsString);
    fputs(output, fpOUT);
    fclose(fpOUT);
}

int main() {
    int fileReadError = 0, LOCCTR, currentLine = 0, digit1, digit2, digit3, digit4, inLoc, constLength, i, j, lengthOfOutputString;

    char operationCode[10], operandAddress[5] = {'0','0','0','0','\0'}, tempOPERAND[20], tempString[10];

    clrscr();

    fileReadError = processFile();

    if(fileReadError == 1) {
        printf("\nPlease ensure that 'assemble.txt' is present in root of C drive.\nPress any key to exit...");
        getch();
        return(1);
    }

    // BEGIN Pass 1

    getCodesFromLine(currentLine);

    if(strcmp(OPCODE, "START") == 0) {
        startAddress = opInt();
        LOCCTR = startAddress;
        currentAddress = startAddress;
        itoa(startAddress, startAddressAsString, 10);
        currentLine++;
        getCodesFromLine(currentLine);
    }
    else
        LOCCTR = 0;

    printf("\n\nSYMTAB is as follows: ");
    while(strcmp(OPCODE, "END") != 0) {	// While OPCODE is not 'END'
        if(gotLine[currentLine][0] != '.') {	// If current line is not a comment line
            if(strcmp(LABEL, "NULL") != 0) {	// If there is a label
                if(isDuplicateLabel() == 1) {	// If there is a duplicate label in SYMTAB
                    printf("\n\nError! Duplicate Label Encountered!\nThe assembly program is invalid. Press any key to exit...");
                    getch();
                    return(1);
                }
                else {	// If there is no duplicate
                    insertIntoSYMTAB(LOCCTR);
                    printf("\n%s - %d", LABEL, LOCCTR);
                }
            }
            if(isOPCODEValid() == 1)
                LOCCTR += 3;
            else if(strcmp(OPCODE, "WORD") == 0)
                LOCCTR += 3;
            else if(strcmp(OPCODE, "RESW") == 0)
                LOCCTR += (3 * opInt());
            else if(strcmp(OPCODE, "RESB") == 0)
                LOCCTR += 1000;
            else if(strcmp(OPCODE, "BYTE") == 0) {
                if(OPERAND[0] == 'X') {
                    LOCCTR++;
                }
                else {
                    LOCCTR += 3; // INCOMPLETE. This simplification is insufficient. Refer algorithm for detail.
                }
            }
            else {
                printf("\n\nError! Invalid Operation Code Encountered!\nThe assembly program is invalid. Press any key to exit...");
                getch();
                return(1);
            }
        }
        currentLine++;
        getCodesFromLine(currentLine);
    }
    programLength = LOCCTR - startAddress;
    getch();

    // END Pass 1

    currentLine = 0;
    outputString[0] = '\0';	// Purge outputString

    // BEGIN Pass 2

    getCodesFromLine(currentLine);

    if(strcmp(OPCODE, "START") == 0) {
        currentLine++;
    }

    initOutputFile();	// write Header record to object program & initialize first Text Record

    getCodesFromLine(currentLine);

    while(strcmp(OPCODE, "END") != 0) {	//while OPCODE is not 'END'
        if(gotLine[currentLine][0] != '.') {	// If current line is not a comment line
            if(isOPCODEValid() == 1) {	// If OPCODE exists
                if(strcmp(OPERAND, "NULL") != 0) {	// If OPERAND is available
                    if(isOPERANDinSYMTAB() == 1) {	// If OPERAND is in SYMTAB
                        itoa(SYMTABVal, operandAddress, 10);	// strore SYMBOL value as Operand address
                    }
                    else {
                        printf("\nError! Undefined Symbol Encountered on Line %d!\nContinuing execution by placing '0000'.", (currentLine + 1));
                    }
                }
                else {	// Operand Address default value is '0000'
                    operandAddress[0] = '0';
                    operandAddress[1] = '0';
                    operandAddress[2] = '0';
                    operandAddress[3] = '0';
                    operandAddress[4] = '\0';
                }
                strcpy(operationCode, OPVAL[OPTABVal]);	// set Operation Code
                strcat(outputString, OPVAL[OPTABVal]);
                strcat(outputString, operandAddress);
            }
            else if(strcmp(OPCODE, "BYTE") == 0 || strcmp(OPCODE, "WORD") == 0) {	// convert constant to object code
                if(strcmp(OPCODE, "WORD") == 0) {
                    i = 0;
                    constLength = strlen(OPERAND);
                    while(constLength < 6) {
                        tempOPERAND[i] = '0';
                        constLength++;
                        i++;
                    }
                    tempOPERAND[i] = '\0';
                    strcat(tempOPERAND, OPERAND);
                    strcat(outputString, tempOPERAND);
                }
                else {
                    i = 2;
                    j = 0;
                    if(OPERAND[0] == 'X') {
                        while(APOS != OPERAND[i]) {
                            tempString[j] = OPERAND[i];
                            j++;
                            i++;
                        }
                        tempString[j] = '\0';
                    }
                    else {
                        tempString[0] = '4';
                        tempString[1] = '5';
                        tempString[2] = '4';
                        tempString[3] = 'F';
                        tempString[4] = '4';
                        tempString[5] = '6';
                        tempString[6] = '\0';
                    }
                    strcat(outputString, tempString);
                }
            }
            if(strcmp(outputString, "000000") != 0) {
                printf("\nObject Code Array: %s", outputString);
                getch();
            }
            lengthOfOutputString = (strlen(outputString) / 2);
            if(lengthOfOutputString >= 12) {	// if object code will not fit into the current text record then
                writeToFile(lengthOfOutputString);	//write Text record to object program
                outputString[0] = '\0';	//initialize new Text record
                printf("\nObject Code Array has reached maximum size. Purging to file...");
                getch();
            }
        }
        currentLine++;
        getCodesFromLine(currentLine);
    }
    printf("\nFinished assembling code...\nPurging remainging object code to file and writing End Record...");
    getch();
    lengthOfOutputString = strlen(outputString);	// write last Text record to object program
    writeToFile(lengthOfOutputString);

    writeEndRecordAndClose();	// write End record to object program

    // END Pass 2

    printf("\nEnd Record has been written. Press any key to exit...");
    getch();

    return(0);
}
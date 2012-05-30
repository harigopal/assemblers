// This program emulates an absolute loader for an SIC Machine. It requires
// the presence of an 'objcode.txt' file in C:\CPROGS directory containing the
// object code produced by an assembler from any given source code. This
// program outputs machine code in the same order as would be expected
//from a real loader and stores the result in 'machcode.txt', also in the same dir.

#include <stdio.h>
#include <stdlib.h>

// GLOBAL VARIABLES

char outputString[30], gotLine[50][50];

int nLines = 0, startAddress = 0;

FILE *fpIN, *fpOUT;

char startAddressAsString[7] = {'0','0','0','0','\0'};

int processFile() {
    char c;
    int location = 0, endNow = 0;
    if((fpIN = fopen("c:/CPROGS/OBJCOD1P.TXT", "r+")) == NULL) {
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
    printf("\nFile has ended! Contents have been stored in local array. Closing file...");
    getch();
    fclose(fpIN);
    return(0);
}

int main() {
    int fileReadError = 0, currentLine = 0, currentLetter = 7, outputLetter = 0, locationCounter = 1, startAddress;

    char programName[10], startAddressAsString[10] = {'0','0','0','0','\0'}, outputString[6], gotLetter;

    clrscr();

    fileReadError = processFile();

    if(fileReadError == 1) {
        printf("\nPlease ensure that 'assemble.txt' is present in root of C drive.\nPress any key to exit...");
        getch();
        return(1);
    }

    if(gotLine[currentLine][0] == 'H') {
        while(gotLine[currentLine][locationCounter] != ' ' && locationCounter <= 6) {
            programName[locationCounter - 1] = gotLine[currentLine][locationCounter];
            locationCounter++;
        }
        programName[locationCounter - 1] = '\0';

        printf("\n\nProgram Name: %s", programName);

        locationCounter = 7;
        while(locationCounter <= 13) {
            startAddressAsString[locationCounter - 7] = gotLine[currentLine][locationCounter];
            locationCounter++;
        }
        startAddressAsString[locationCounter - 7] = '\0';
        startAddress = atoi(startAddressAsString);
        printf("\nStart Address: %d", startAddress);
        getch();
    }

    currentLine++;
    fpOUT = fopen("c:/CPROGS/MACHCODE.TXT", "w+");

    while(gotLine[currentLine][0] != 'E') {
        gotLetter = gotLine[currentLine][currentLetter];
        if(gotLetter == '\0') {
            currentLine++;
            currentLetter = 7;
            startAddressAsString[0] = gotLine[currentLine][1];
            startAddressAsString[1] = gotLine[currentLine][2];
            startAddressAsString[2] = gotLine[currentLine][3];
            startAddressAsString[3] = gotLine[currentLine][4];
            startAddressAsString[4] = '\0';
            startAddress = atoi(startAddressAsString);
        }
        else {
            outputString[outputLetter] = gotLetter;
            outputLetter++;
            if(outputLetter == 4) {

                outputString[4] = '\n';
                outputString[5] = '\0';
                itoa(startAddress, startAddressAsString, 10);
                startAddressAsString[4] = ' ';
                startAddressAsString[5] = '\0';
                fputs(startAddressAsString, fpOUT);
                fputs(outputString, fpOUT);

                startAddress += 2;
                outputLetter = 0;
            }
            currentLetter++;
        }
    }
    if(outputLetter != 0) {
        outputString[outputLetter] = '\n';
        outputString[outputLetter + 1] = '\0';
        itoa(startAddress, startAddressAsString, 10);
        startAddressAsString[4] = ' ';
        startAddressAsString[5] = '\0';
        fputs(startAddressAsString, fpOUT);
        fputs(outputString, fpOUT);
    }

    printf("\n\nLoading procedure completed. Press any key to exit...");
    getch();

    return(0);
}
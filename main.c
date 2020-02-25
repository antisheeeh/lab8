#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 256
#define NUMBER_OF_WORDS_IN_LINE 3
#define SUCCESS 0
#define ERROR 1

typedef struct NOTE2 {
    char* Name;
    char* TELE;
    char* DATE;
} NOTE2;

int countNumberOfLines(FILE* file);
int readBlocknote(char**** fullText, int numberOfLines, FILE* inputFile);
int splitLine(char* line, char*** wordsInLine, char* sep);
void sortFullText(char*** fullText, int numberOfLines);
int fillBlocknote(NOTE2** BLOCKNOTE, char*** fullText, int numberOfLines);
int fillNote(NOTE2** NOTE, char** wordsInLine);
NOTE2* findNote(NOTE2** BLOCKNOTE, char* phoneNumber, int numberOfLines);
void printNote(NOTE2* NOTE);
void freeBlocknoteArray(NOTE2** BLOCKNOTE, int numberOfLines);
void freeFullTextArray(char*** fullText, int numberOfLines);
void freeLine(char** line, int numberOfWords);

int main() {
    NOTE2 **BLOCKNOTE, *NOTE;
    FILE* inputFile;
    char line[MAX_LENGTH], ***fullText;
    int numberOfLines;

    if((inputFile = fopen("input.csv", "r")) == NULL) {
        puts("Cannot open csv file");
    } else {
        numberOfLines = countNumberOfLines(inputFile);

        if(numberOfLines == 0) {
            puts("File is empty");
        } else if((BLOCKNOTE = malloc(sizeof(*BLOCKNOTE) * numberOfLines)) == NULL) {
            puts("Cannot allocate memory");
        } else if(readBlocknote(&fullText, numberOfLines, inputFile) == ERROR) {
            puts("Cannot allocate memory");
        } else {
            sortFullText(fullText, numberOfLines);

            if(fillBlocknote(BLOCKNOTE, fullText, numberOfLines) == ERROR) {
                puts("Cannot allocate memory");
            } else {
                puts("Enter your phone number: ");
                fgets(line, MAX_LENGTH, stdin);
                line[strlen(line) - 1] = '\0';

                if((NOTE = findNote(BLOCKNOTE, line, numberOfLines)) == NULL) {
                    puts("No person found with this phone number");
                } else {
                    printNote(NOTE);
                }

                freeFullTextArray(fullText, numberOfLines);
                freeBlocknoteArray(BLOCKNOTE, numberOfLines);
            }
        }

        if(fclose(inputFile) == EOF) {
            puts("Cannot close csv file");
        }
    }

    return 0;
}

int countNumberOfLines(FILE* file) {
    int numberOfLines = 0;
    char line[MAX_LENGTH];

    while(fgets(line, MAX_LENGTH, file) != NULL) ++numberOfLines;
    rewind(file);

    return numberOfLines;
}

int readBlocknote(char**** fullText, int numberOfLines, FILE* inputFile) {
    char line[MAX_LENGTH], *sep = ";", **wordsInLine;
    int i, code = SUCCESS;

    *fullText = malloc(sizeof(**fullText) * numberOfLines);

    if(fullText == NULL) {
        code == ERROR;
    }

    for(i = 0; i < numberOfLines && code != ERROR; ++i) {
        fgets(line, MAX_LENGTH, inputFile);
        
        if(splitLine(line, &wordsInLine, sep) == ERROR) {
            code = ERROR;
            freeFullTextArray(*fullText, i);
        } else {
            (*fullText)[i] = wordsInLine;
        }
    }

    return code;
}

int splitLine(char* line, char*** wordsInLine, char* sep) {
    int i, code = SUCCESS;
    char *word;

    *wordsInLine = malloc(sizeof(**wordsInLine) * NUMBER_OF_WORDS_IN_LINE);

    if(*wordsInLine == NULL) {
        code = ERROR;
    }

    for(i = 0, word = strtok(line, sep); code != ERROR && word != NULL; ++i) {
        if(i == NUMBER_OF_WORDS_IN_LINE - 1) {
            word[strlen(word) - 1] = '\0';
        } else {
            word[strlen(word)] = '\0';
        }

        (*wordsInLine)[i] = malloc(sizeof(**wordsInLine[i]) * (strlen(word) + 1));

        if((*wordsInLine)[i] == NULL) {
            code == ERROR;
            freeLine(wordsInLine[i], i);
        } else {
            strcpy((*wordsInLine)[i], word);
            word = strtok(NULL, sep);
        }
    }
    
    return code;
}

void sortFullText(char*** fullText, int numberOfLines) {
    int i, j;
    char** tempLine;

    for(i = 1; i < numberOfLines; ++i) {
        tempLine = fullText[i];
        for(j = i - 1; j >= 0 && strcmp(fullText[j][0], tempLine[0]) > 0; --j) fullText[j + 1] = fullText[j];
        fullText[j + 1] = tempLine;
    }
}

int fillBlocknote(NOTE2** BLOCKNOTE, char*** fullText, int numberOfLines) {
    int i, code = SUCCESS;

    for(i = 0; code != ERROR && i < numberOfLines; ++i) {
        if(fillNote(&BLOCKNOTE[i], fullText[i]) == ERROR) {
            code = ERROR;
            freeBlocknoteArray(BLOCKNOTE, i);
        }
    }

    return code;
}

int fillNote(NOTE2** NOTE, char** wordsInLine) {
    int code = SUCCESS;

    *NOTE = malloc(sizeof(**NOTE));

    if(*NOTE == NULL) {
        code = ERROR;
    } else {
        (*NOTE)->Name = wordsInLine[0];
        (*NOTE)->TELE = wordsInLine[1];
        (*NOTE)->DATE = wordsInLine[2];
    }

    return code;
}

NOTE2* findNote(NOTE2** BLOCKNOTE, char* phoneNumber, int numberOfLines) {
    int i;
    NOTE2* NOTE = NULL;

    for(i = 0; i < numberOfLines; ++i) {
        if(strcmp(BLOCKNOTE[i]->TELE, phoneNumber) == 0) {
            NOTE = BLOCKNOTE[i];
        }
    }

    return NOTE;
}

void printNote(NOTE2* NOTE) {
    printf("Surname and initials: %s Phone number: %s Date of Birth: %s\n", NOTE->Name, NOTE->TELE, NOTE->DATE);
}

void freeBlocknoteArray(NOTE2** BLOCKNOTE, int numberOfLines) {
    int i;

    for(i = 0; i < numberOfLines; ++i) {
        free(BLOCKNOTE[i]);
        BLOCKNOTE[i] = NULL;
    }

    free(BLOCKNOTE);
    BLOCKNOTE = NULL;
}

void freeFullTextArray(char*** fullText, int numberOfLines) {
    int i, j;

    for(i = 0; i < numberOfLines; ++i) {
        freeLine(fullText[i], NUMBER_OF_WORDS_IN_LINE);
    }

    free(fullText);
    fullText = NULL;
}

void freeLine(char** line, int numberOfWords) {
    int i;

    for(i = 0; i < numberOfWords; ++i) {
        free(line[i]);
        line[i] = NULL;
    }

    free(line);
    line = NULL;
}
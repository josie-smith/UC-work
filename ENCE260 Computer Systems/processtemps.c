#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_LINE_LENGTH 500
#define MAX_STATION_NAME 30


// returns a pointer to the nth occurance of sharacter c in string s
char* strchrn(char* s, int c, int n)
{
    char* occurence = NULL;
    if (n > 0) {
        int i = 0;
        bool found = 0;
        while (*s != '\0' && !found) {
            if (*s == c) {
                i++;
                if (i == n) {
                    occurence = s;
                    found = 1;
                }
            }
            s++;
        }
    }
    return occurence;
}



// get nth field of a comma separated string
char* nField(const char s[], int n, char result[])
{
    char buffer[MAX_LINE_LENGTH] = {0}; // Line buffer
    char* term = buffer;
    strncpy(buffer, s, MAX_LINE_LENGTH);
    if (n != 1) {
        char* commaPosBefore = strchrn(term, ',', n-1);
        term = commaPosBefore + 1;
    }
    char* commaPosAfter = strchrn(term, ',', n);
    if (commaPosAfter != NULL && commaPosAfter > term) {
        *commaPosAfter = '\0';
    }
    strncpy(result, term, MAX_LINE_LENGTH);
    return result;
}



// Read the station name from the first section, print the headers, and read
// until the first blank line
void printHeaders(FILE* file, double threshold)
{
    char buffer[MAX_LINE_LENGTH] = {0};
    char siteName[MAX_STATION_NAME + 1] = {0};
    int lineNumber = 1;
    char* line = fgets(buffer, MAX_LINE_LENGTH, file);

    // get the third line
    while (lineNumber < 3 && line != NULL) {
        line = fgets(buffer, MAX_LINE_LENGTH, file);
        lineNumber++;
    }
    char* commaPos = strchr(buffer, ',');
    if (commaPos != NULL && commaPos > buffer) {
        *commaPos = '\0';  // null-terminate the siteName
        strncpy(siteName, buffer, MAX_STATION_NAME);
        siteName[MAX_STATION_NAME] = '\0';
        printf("Dates when %.1f C was reached at %s\n\n", threshold, siteName);
        printf("   Date       MaxTemp\n");
    }
    // keep reading until the blank line
    while (*line != '\n' && line != NULL) {
        line = fgets(buffer, MAX_LINE_LENGTH, file);
    }
}



// Prints the date and maxTemp from a line if it exceeds the threshold
void printLine(char buffer[], double threshold)
{
    char tempString[MAX_LINE_LENGTH] = {0};
    nField(buffer, 3, tempString);
    double maxTemp = atof(tempString);

    if (maxTemp >= threshold) {
        char dateString[MAX_LINE_LENGTH] = {0};
        nField(buffer, 2, dateString);
        char* date = dateString;

        char yearString[5] = {0};
        strncpy(yearString, date, 4);
        yearString[4] = '\0';
        int year = atoi(yearString);

        char monthString[3] = {0};
        strncpy(monthString, &date[4], 2);
        monthString[2] = '\0';
        int month = atoi(monthString);

        char dayString[3] = {0};
        strncpy(dayString, &date[6], 2);
        dayString[2] = '\0';
        int day = atoi(dayString);

        printf("%02d/%02d/%d%9.1lf C\n", day, month, year, maxTemp);
    }
}



// skips the first 2 lines of the temp section, then reads each data line in turn
void printTemps(FILE* file, double threshold)
{
    char buffer[MAX_LINE_LENGTH] = {0};
    int lineNumber = 1;
    char* line = fgets(buffer, MAX_LINE_LENGTH, file);
    // skip the first 2 lines of the temp section
    while (lineNumber < 3 && line != NULL) {
        line = fgets(buffer, MAX_LINE_LENGTH, file);
        lineNumber++;
    }
    while (*line != '\n' && line != NULL) {
        printLine(buffer, threshold);
        line = fgets(buffer, MAX_LINE_LENGTH, file);
    }
}



int main(int argc, char* argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: processtemps filename threshold\n");
    } else {
        FILE* inputFile = fopen(argv[1], "r");
        if (inputFile == NULL) {
            fprintf(stderr, "File '%s' not found\n", argv[1]);
        } else {
            double threshold = atof(argv[2]);
            printHeaders(inputFile, threshold);
            printTemps(inputFile, threshold);
        }
    }
}

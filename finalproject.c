#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Function Prototypes
int romanToDecimal(const char *roman);
void decimalToWords(int num, char *result);
void processLine(const char *line, FILE *outputFile);
int isValidRoman(const char *roman);

// Main Function
int main() {
    FILE *inputFile = fopen("Input.txt", "r");
    FILE *outputFile = fopen("Output.txt", "w");

    if (!inputFile || !outputFile) {
        printf("Error: Unable to open file.\n");
        return 1;
    }

    char line[256];
    while (fgets(line, sizeof(line), inputFile)) {
        // Skip empty lines
        if (strlen(line) > 1) {
            processLine(line, outputFile);
        }
    }

    fclose(inputFile);
    fclose(outputFile);

    printf("Processing completed. Check Output.txt for results.\n");
    return 0;
}

// Function to convert Roman numeral to decimal
int romanToDecimal(const char *roman) {
    int values[128] = {0};
    values['I'] = 1;
    values['V'] = 5;
    values['X'] = 10;
    values['L'] = 50;
    values['C'] = 100;
    values['D'] = 500;
    values['M'] = 1000;

    int total = 0, prevValue = 0;

    for (int i = strlen(roman) - 1; i >= 0; i--) {
        char ch = toupper(roman[i]);
        int value = values[ch];

        if (value < prevValue) {
            total -= value;
        } else {
            total += value;
        }

        prevValue = value;
    }

    return total;
}

// Function to validate Roman numeral
int isValidRoman(const char *roman) {
    int values[128] = {0};
    values['I'] = 1;
    values['V'] = 5;
    values['X'] = 10;
    values['L'] = 50;
    values['C'] = 100;
    values['D'] = 500;
    values['M'] = 1000;

    int prevValue = 0;
    int repeatCount = 0;

    for (int i = 0; roman[i] != '\0'; i++) {
        char ch = toupper(roman[i]);
        if (values[ch] == 0) return 0; // Invalid character

        int value = values[ch];

        // Check for valid repeat rules
        if (value == prevValue) {
            repeatCount++;
            if ((value == 5 || value == 50 || value == 500) || repeatCount > 2) {
                return 0; // Invalid repeat for V, L, D or too many repeats
            }
        } else {
            repeatCount = 0;
        }

        // Check for valid subtraction rules
        if (prevValue > 0 && value > prevValue) {
            if (!((prevValue == 1 && (value == 5 || value == 10)) ||
                  (prevValue == 10 && (value == 50 || value == 100)) ||
                  (prevValue == 100 && (value == 500 || value == 1000)))) {
                return 0;
            }
        }

        prevValue = value;
    }

    return 1;
}

// Function to convert decimal number to words
void decimalToWords(int num, char *result) {
    const char *units[] = {"", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine"};
    const char *teens[] = {"Ten", "Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen", "Sixteen", "Seventeen", "Eighteen", "Nineteen"};
    const char *tens[] = {"", "", "Twenty", "Thirty", "Forty", "Fifty", "Sixty", "Seventy", "Eighty", "Ninety"};
    const char *thousands[] = {"", "Thousand", "Million", "Billion"};

    if (num == 0) {
        strcpy(result, "Zero");
        return;
    }

    char temp[256] = "";
    int chunkCount = 0;
    int isNegative = 0;

    // Check for negative number
    if (num < 0) {
        isNegative = 1;
        num = -num; // Convert to positive for word conversion
    }

    while (num > 0) {
        int chunk = num % 1000;
        num /= 1000;

        if (chunk > 0) {
            char chunkWords[256] = "";

            int hundreds = chunk / 100;
            if (hundreds > 0) {
                strcat(chunkWords, units[hundreds]);
                strcat(chunkWords, " Hundred");
            }

            int remainder = chunk % 100;
            if (remainder >= 10 && remainder < 20) {
                if (hundreds > 0) strcat(chunkWords, " ");
                strcat(chunkWords, teens[remainder - 10]);
            } else {
                int tensPlace = remainder / 10;
                int unitsPlace = remainder % 10;

                if (tensPlace > 0) {
                    if (hundreds > 0) strcat(chunkWords, " ");
                    strcat(chunkWords, tens[tensPlace]);
                }

                if (unitsPlace > 0) {
                    if (tensPlace > 0) strcat(chunkWords, " ");
                    strcat(chunkWords, units[unitsPlace]);
                }
            }

            if (thousands[chunkCount][0] != '\0') {
                strcat(chunkWords, " ");
                strcat(chunkWords, thousands[chunkCount]);
            }

            if (temp[0] != '\0') {
                strcat(chunkWords, " ");
                strcat(chunkWords, temp);
            }

            strcpy(temp, chunkWords);
        }

        chunkCount++;
    }

    if (isNegative) {
        strcpy(result, "Negative ");
        strcat(result, temp);
    } else {
        strcpy(result, temp);
    }
}


// Function to process each line of input
void processLine(const char *line, FILE *outputFile) {
    char roman1[64] = "", roman2[64] = "";
    char operator;

    // Parse the input line for two Roman numerals and an operator
    if (sscanf(line, "%s %c %s", roman1, &operator, roman2) != 3) {
        fprintf(outputFile, "Invalid input\n");
        return;
    }

    // Validate Roman numerals
    if (!isValidRoman(roman1) || !isValidRoman(roman2)) {
        fprintf(outputFile, "Invalid Roman numeral\n");
        return;
    }

    // Convert Roman numerals to decimal
    int decimal1 = romanToDecimal(roman1);
    int decimal2 = romanToDecimal(roman2);
    int result;

    // Perform the arithmetic operation
    switch (operator) {
        case '+':
            result = decimal1 + decimal2;
            break;
        case '-':
            result = decimal1 - decimal2;
            break;
        case '*':
            result = decimal1 * decimal2;
            break;
        case '/':
            if (decimal2 == 0) {
                fprintf(outputFile, "Division by zero\n");
                return;
            }
            result = decimal1 / decimal2;
            break;
        default:
            fprintf(outputFile, "Invalid operator\n");
            return;
    }

    // Convert the result to words
    char words[256];
    decimalToWords(result, words);

    // Write the result in words to the output file
    fprintf(outputFile, "%s\n", words);
}

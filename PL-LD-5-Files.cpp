#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024
#define MAX_IP_LENGTH 16

// Structure to represent log entries
typedef struct {
    char ip[MAX_IP_LENGTH];
    int numFiles;
    long totalSize;
} LogEntry;

/**
 * Processes a log file, aggregates data, and writes the results to an output file.
 *
 * @param logFilePath The path to the input log file.
 * @param outputFilePath The path to the output file where results will be written.
 */
void processLogFile(const char* logFilePath, const char* outputFilePath) {
    FILE* logFile = fopen(logFilePath, "r");
    if (logFile == NULL) {
        perror("Error opening log file");
        exit(1);
    }

    FILE* outputFile = fopen(outputFilePath, "w");
    if (outputFile == NULL) {
        perror("Error opening output file");
        fclose(logFile);
        exit(1);
    }

    // Array to store log entries
    LogEntry* entries = NULL;
    int numEntries = 0;
    const char* delimiter = " ";

    // Buffer to store each line of the log file
    char line[MAX_LINE_LENGTH];
    // Read each line of the log file
    while (fgets(line, sizeof(line), logFile)) {
        // Tokenize each line
        char* ip = strtok(line, delimiter);
        char* unused1 = strtok(NULL, delimiter); // Skip the unused fields
        char* unused2 = strtok(NULL, delimiter);
        char* unused3 = strtok(NULL, delimiter);
        char* totalSizeStr = strtok(NULL, delimiter);

        // Check if both IP address and total size are present
        if (ip && totalSizeStr) {
            // Flag to check if the IP address is already in the entries array
            int foundInEntries = 0;
            for (int i = 0; i < numEntries; i++) {
                if (strcmp(entries[i].ip, ip) == 0) {
                    // Update counts for the existing entry
                    entries[i].numFiles++;
                    entries[i].totalSize += atoi(totalSizeStr);
                    foundInEntries = 1;
                    break;
                }
            }

            if (!foundInEntries) {
                numEntries++;
                // Resize the entries array to accommodate the new entry
                entries = (LogEntry*)realloc(entries, numEntries * sizeof(LogEntry));
                // Copy the IP address to the new entry
                strncpy(entries[numEntries - 1].ip, ip, MAX_IP_LENGTH);
                // Initialize counts for the new entry
                entries[numEntries - 1].numFiles = 1;
                entries[numEntries - 1].totalSize = atoi(totalSizeStr);
            }
        }
    }

    // Write the entries to the output file
    for (int i = 0; i < numEntries; i++) {
        fprintf(outputFile, "IP: %s, Files Requested: %d, Total Size: %ld\n",
            entries[i].ip, entries[i].numFiles, entries[i].totalSize);
    }

    // Free dynamically allocated memory for entries
    free(entries);
    fclose(logFile);
    fclose(outputFile);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <log_file> <output_file>\n", argv[0]);
        return 1;
    }

    const char* logFilePath = argv[1];
    const char* outputFilePath = argv[2];

    processLogFile(logFilePath, outputFilePath);

    return 0;
}

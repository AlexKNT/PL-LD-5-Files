#include <cstdio>
#include <cstring>
#include <cstdlib>

constexpr int MAX_LOG_ENTRY_LENGTH = 1024;
constexpr int MAX_IP_LENGTH = 16; // IPv4 addresses have a maximum length of 15 characters

/**
 * Extracts and processes IP addresses from an Apache web server log file.
 *
 * @param inputFilePath Path to the input log file.
 * @param outputFilePath Path to the output file for IP addresses and their stats.
 */
void processApacheLog(const char* inputFilePath, const char* outputFilePath) {
    // Open input log file
    FILE* inputFile;
    if (fopen_s(&inputFile, inputFilePath, "r") != 0) {
        perror("Error opening input file");
        return;
    }

    // Open output file for writing IP addresses and their stats
    FILE* outputFile;
    if (fopen_s(&outputFile, outputFilePath, "w") != 0) {
        perror("Error opening output file");
        fclose(inputFile);
        return;
    }

    char line[MAX_LOG_ENTRY_LENGTH];
    char ipAddress[MAX_IP_LENGTH];
    char previousIp[MAX_IP_LENGTH] = "";

    int numFiles = 0;
    long long totalSize = 0;

    while (fgets(line, MAX_LOG_ENTRY_LENGTH, inputFile) != nullptr) {
        if (sscanf_s(line, "%*s %*s %*s [%*[^]]] \"%*s %*s %*s\" %s", ipAddress, (unsigned)_countof(ipAddress)) == 1) {
            if (strcmp(previousIp, ipAddress) == 0) {
                // Same IP address as the previous entry
                numFiles++;
            }
            else {
                // New IP address encountered
                if (numFiles > 0) {
                    // Write IP address and its stats to the output file
                    fprintf(outputFile, "IP: %s, Files Requested: %d, Total Size: %lld bytes\n", previousIp, numFiles, totalSize);
                }

                // Reset counters for the new IP address
                strcpy_s(previousIp, _countof(previousIp), ipAddress);
                numFiles = 1;
                totalSize = 0;
            }

            long long fileSize;
            if (sscanf_s(line, "%*s %*s %*s [%*[^]]] \"%*s %*s %*s\" %*s %*s %*s %lld", &fileSize) == 1) {
                totalSize += fileSize;
            }
        }
    }

    // Write the last IP address and its stats
    if (numFiles > 0) {
        fprintf(outputFile, "IP: %s, Files Requested: %d, Total Size: %lld bytes\n", previousIp, numFiles, totalSize);
    }

    fclose(inputFile);
    fclose(outputFile);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <inputLogFile> <outputFile>\n", argv[0]);
        return 1;
    }

    const char* inputFilePath = argv[1];
    const char* outputFilePath = argv[2];

    processApacheLog(inputFilePath, outputFilePath);

    return 0;
}

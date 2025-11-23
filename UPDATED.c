#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#define MAX_NAME 100
#define MAX_PROGRAMME 100
#define FILENAME "P4_6-CMS.txt"
#define MAX_RECORDS 100000  // Maximum records limit

// Student record structure with Grade field
typedef struct {
    int ID;
    char Name[MAX_NAME];
    char Programme[MAX_PROGRAMME];
    float Mark;
    char Grade[3];
} StudentRecords;

// Global variables - using dynamic array
StudentRecords *student_records = NULL;
int recordCount = 0;
int capacity = 0;
int isFileOpen = 0;

// Function prototypes
void printDeclaration();
void openDatabase();
void showAll();
void insertRecord(char *input);
void queryRecord(char *input);
void updateRecord(char *input);
void deleteRecord(char *input);
void saveDatabase();
void showSummary();
void sortByID(int ascending);
void sortByMark(int ascending);
void trim(char *str);
void toLowerCase(char *str);
void calculateGrade(float mark, char *grade);

// filtering functions
void filterByProgramme();
void filterByMarkRange();

// Save before exit function
void promptSaveBeforeExit();

// Programme validation function
static int isValidProgramme(const char *programme);

// Helper functions
static void trim_newline(char *s);
static int idExists(int id);
static int ensureCapacity(int want);
static int findIndexById(int id);
static int parseID(const char *str, int *id);
static int parseMark(const char *str, float *mark);

// Security validation functions
static int isValidInputLength(const char *input);
static int containsInvalidChars(const char *str, const char *invalid_chars);
static int isReasonableString(const char *str);
static int isAllWhitespace(const char *str);
static int isValidFieldValue(const char *str);

// SQL injection prevention functions
static int containsSQLInjectionPatterns(const char *str);
static int isSecureFieldValue(const char *str);

// Duplicate record detection
static int isDuplicateRecord(int id, const char *name, const char *programme, float mark);

// Enhanced invalid character checking
static int containsDangerousChars(const char *str);

// Duplicate field detection
static int hasDuplicateFields(const char *input);

// Command injection detection
static int containsCommandInjectionChars(const char *str);
static int isValidFieldContent(const char *str);

int main(void) {
    char input[256];
    char command[256];

    printDeclaration();
    printf("CMS - type HELP for commands.\n");

    while (1) {
        printf("P4_6: ");
        if (!fgets(input, sizeof(input), stdin)) {
            break;  
        }
        
        // Check if input was truncated
        if (strlen(input) == sizeof(input) - 1 && input[sizeof(input) - 2] != '\n') {
            printf("CMS: Input too long. Maximum 255 characters allowed.\n");
            // Clear the input buffer
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            continue;
        }
        
        // Check for command injection in main input
        if (containsCommandInjectionChars(input)) {
            printf("CMS: Security warning: Command injection attempt detected.\n");
            continue;
        }

        // Check for command injection
        if (strchr(input, ';') != NULL) {
            printf("CMS: Invalid character ';' in command.\n");
            continue;
        }

        // Safe SQL injection detection
        if (containsSQLInjectionPatterns(input)) {
            printf("CMS: Security warning: Potential injection attempt detected in command.\n");
            continue;
        }

        trim_newline(input);

        // Validate input length
        if (!isValidInputLength(input)) {
            printf("CMS: Invalid input length.\n");
            continue;
        }

        // Make a lowercase copy for command matching (so commands are case-insensitive)
        strcpy(command, input);
        toLowerCase(command);

        if (strcmp(command, "exit") == 0) {
            promptSaveBeforeExit();  // Ask to save before exit
            printf("CMS: Exiting program. Goodbye!\n");
            free(student_records);
            return 0;
        }
        else if (strcmp(command, "help") == 0) {
            printf("Commands:\n");
            printf("OPEN\n");
            printf("SHOW ALL\n");
            printf("SHOW ALL SORT BY ID [ASCD/DESC]\n");
            printf("SHOW ALL SORT BY MARK [ASCD/DESC]\n");
            printf("SHOW ALL FILTER BY PROGRAMME\n");
            printf("SHOW ALL FILTER BY MARK\n");
            printf("INSERT ID=.. Name=.. Programme=.. Mark=..\n");
            printf("QUERY ID=..\n");
            printf("UPDATE ID=.. Name=.. | Programme=.. | Mark=..\n");
            printf("DELETE ID=..\n");
            printf("SAVE\n");
            printf("SHOW SUMMARY\n");
            printf("EXIT\n");
            printf("\nNote: ID must be exactly 7 digits starting with '2' (e.g., 2123456)\n");
            printf("Note: Programme must contain only letters, spaces, and hyphens\n");
            printf("Note: Names and programmes cannot contain / | & # characters\n");
            
        }
        else if (strcmp(command, "open") == 0) {
            openDatabase();
        }
        else if (strcmp(command, "show all") == 0) {
            showAll();
        }
        else if (strncmp(command, "show all sort by id", 19) == 0) {
        if (strstr(command, "desc") != NULL)
            sortByID(0); // descending
        else if (strstr(command, "ascd") != NULL)
            sortByID(1); // ascending
        else {
            printf("CMS Please specify sort order: 'ascd' or 'desc'.\n");
            continue;
        }
        showAll();
    }

        else if (strncmp(command, "show all sort by mark", 21) == 0) {
        if (strstr(command, "desc") != NULL)
            sortByMark(0); // descending
        else if (strstr(command, "ascd") != NULL)
            sortByMark(1); // ascending
        else {
            printf("CMS Please specify sort order: 'ascd' or 'desc'.\n");
            continue;
        }
        showAll();
    }

        else if (strcmp(command, "show all filter by programme") == 0) {
            filterByProgramme();
        }
        else if (strcmp(command, "show all filter by mark") == 0) {
            filterByMarkRange();
        }
        else if (strncmp(command, "insert", 6) == 0) {
            // Use original input so Name/Programme keep their case
            insertRecord(input);
        }
        else if (strncmp(command, "query", 5) == 0) {
            queryRecord(input);
        }
        else if (strncmp(command, "update", 6) == 0) {
            updateRecord(input);
        }
        else if (strncmp(command, "delete", 6) == 0) {
            deleteRecord(input);
        }
        else if (strcmp(command, "save") == 0) {
            saveDatabase();
        }
        else if (strcmp(command, "show summary") == 0) {
            showSummary();
        }
        else {
            printf("CMS: Invalid command. Please try again.\n");
        }
    }

    return 0;
}

// Check for command injection characters
static int containsCommandInjectionChars(const char *str) {
    if (!str) return 0;
    
    const char *dangerous_chars = "|&`$(){}[]<>!";
    for (int i = 0; str[i]; i++) {
        if (strchr(dangerous_chars, str[i]) != NULL) {
            return 1; // Command injection character found
        }
    }
    return 0; // No command injection characters
}

// Comprehensive field content validation
static int isValidFieldContent(const char *str) {
    if (!str) return 0;
    
    // Check for command injection characters
    if (containsCommandInjectionChars(str)) {
        printf("CMS: Security warning: Command injection characters detected.\n");
        return 0;
    }
    
    // Check for other dangerous characters
    if (containsDangerousChars(str)) {
        printf("CMS: Invalid characters detected. Cannot contain / | & # characters.\n");
        return 0;
    }
    
    // Check for SQL injection
    if (containsSQLInjectionPatterns(str)) {
        printf("CMS: Security warning: Potential SQL injection detected.\n");
        return 0;
    }
    
    return 1;
}

// Validate programme name (only letters, spaces, and hyphens allowed)
static int isValidProgramme(const char *programme) {
    if (!programme || *programme == '\0') {
        return 0;  // Empty string
    }
    
    // Check each character
    for (int i = 0; programme[i]; i++) {
        unsigned char c = programme[i];
        
        // Allow letters (both cases), spaces, and hyphens
        if (!isalpha(c) && c != ' ' && c != '-') {
            return 0;  // Invalid character found
        }
        
        // Don't allow consecutive spaces or hyphens at start/end
        if (i == 0 && (c == ' ' || c == '-')) {
            return 0;  // Cannot start with space or hyphen
        }
        if (programme[i + 1] == '\0' && (c == ' ' || c == '-')) {
            return 0;  // Cannot end with space or hyphen
        }
        if (i > 0 && (c == ' ' || c == '-') && (programme[i-1] == ' ' || programme[i-1] == '-')) {
            return 0;  // No consecutive spaces or hyphens
        }
    }
    
    // Must contain at least one letter
    int has_letter = 0;
    for (int i = 0; programme[i]; i++) {
        if (isalpha((unsigned char)programme[i])) {
            has_letter = 1;
            break;
        }
    }
    
    return has_letter;
}

// Prompt user to save before exiting
void promptSaveBeforeExit() {
    if (!isFileOpen || recordCount == 0) {
        printf("CMS: No changes to save.\n");
        return;
    }
    
    printf("CMS: Do you want to save changes before exiting? (Y/N): ");
    char response;
    if (scanf(" %c", &response) == 1) {
        // Clear input buffer
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
        
        if (response == 'Y' || response == 'y') {
            saveDatabase();
            printf("CMS: Changes saved successfully.\n");
        } else {
            printf("CMS: Exiting without saving changes.\n");
        }
    } else {
        printf("CMS: Invalid input. Exiting without saving.\n");
    }
}

// Check for duplicate records (all fields)
static int isDuplicateRecord(int id, const char *name, const char *programme, float mark) {
    for (int i = 0; i < recordCount; i++) {
        // Check if all fields match
        if (student_records[i].ID == id &&
            strcmp(student_records[i].Name, name) == 0 &&
            strcmp(student_records[i].Programme, programme) == 0 &&
            student_records[i].Mark == mark) {
            return 1; // Exact duplicate found
        }
    }
    return 0; // No duplicate found
}

// Check for dangerous characters (/ | & #)
static int containsDangerousChars(const char *str) {
    if (!str) return 0;
    
    const char *dangerous_chars = "/|&#";
    for (int i = 0; str[i]; i++) {
        if (strchr(dangerous_chars, str[i]) != NULL) {
            return 1; // Dangerous character found
        }
    }
    return 0; // No dangerous characters
}

// Check for duplicate fields in input
static int hasDuplicateFields(const char *input) {
    if (!input) return 0;
    
    char buffer[500];
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    
    // Convert to lowercase for case-insensitive comparison
    toLowerCase(buffer);
    
    // Count occurrences of each field
    int id_count = 0, name_count = 0, programme_count = 0, mark_count = 0;
    
    char *token = strtok(buffer, " ");
    while (token != NULL) {
        // Check if token starts with a field name
        if (strncmp(token, "id=", 3) == 0) {
            id_count++;
        }
        else if (strncmp(token, "name=", 5) == 0) {
            name_count++;
        }
        else if (strncmp(token, "programme=", 10) == 0) {
            programme_count++;
        }
        else if (strncmp(token, "mark=", 5) == 0) {
            mark_count++;
        }
        
        token = strtok(NULL, " ");
    }
    
    // Check if any field appears more than once
    if (id_count > 1) {
        printf("CMS: Error - Multiple ID fields detected.\n");
        return 1;
    }
    if (name_count > 1) {
        printf("CMS: Error - Multiple Name fields detected.\n");
        return 1;
    }
    if (programme_count > 1) {
        printf("CMS: Error - Multiple Programme fields detected.\n");
        return 1;
    }
    if (mark_count > 1) {
        printf("CMS: Error - Multiple Mark fields detected.\n");
        return 1;
    }
    
    return 0; // No duplicate fields
}

// Safe SQL injection detection - no false positives
static int containsSQLInjectionPatterns(const char *str) {
    if (!str) return 0;
    
    char lowerStr[256];
    strncpy(lowerStr, str, sizeof(lowerStr) - 1);
    lowerStr[sizeof(lowerStr) - 1] = '\0';
    toLowerCase(lowerStr);
    
    // Only check for actual dangerous SQL patterns
    const char *sqlPatterns[] = {
        // SQL comment syntax (definitely dangerous)
        "--", "/*", "*/",
        // Command separators
        ";",
        // Actual SQL commands with spaces to avoid false positives
        " union ", " select ", " drop ", " create ", " alter ",
        " insert ", " update ", " delete ", " exec ", " execute ",
        // Dangerous SQL patterns with spaces
        " 1=1", "1=1 ", "'1'='1"
    };
    
    int patternCount = sizeof(sqlPatterns) / sizeof(sqlPatterns[0]);
    for (int i = 0; i < patternCount; i++) {
        if (strstr(lowerStr, sqlPatterns[i]) != NULL) {
            return 1; // SQL injection pattern detected
        }
    }
    
    return 0; // No SQL injection patterns found
}

// Enhanced field validation with SQL injection detection and invalid character check
static int isSecureFieldValue(const char *str) {
    if (!str) return 0;
    
    // Check basic validity first
    if (!isValidFieldValue(str)) {
        return 0;
    }
    
    // Check for SQL injection patterns
    if (containsSQLInjectionPatterns(str)) {
        printf("CMS: Security warning: Potential injection attempt detected.\n");
        return 0;
    }
    
    // Check for invalid characters / | & #
    if (containsDangerousChars(str)) {
        printf("CMS: Invalid characters detected. Cannot contain / | & # characters.\n");
        return 0;
    }
    
    // Check for command injection characters
    if (containsCommandInjectionChars(str)) {
        printf("CMS: Security warning: Command injection characters detected.\n");
        return 0;
    }
    
    // Additional security: limit consecutive special characters
    int specialCount = 0;
    for (int i = 0; str[i]; i++) {
        if (!isalnum((unsigned char)str[i]) && !isspace((unsigned char)str[i])) {
            specialCount++;
            if (specialCount > 3) { // Allow some punctuation but not many
                return 0;
            }
        } else {
            specialCount = 0;
        }
    }
    
    return 1;
}

void printDeclaration() {
    printf("\n========================================\n");
    printf(" Declaration\n");
    printf("========================================\n\n");
    printf("SIT's policy on copying does not allow the students to copy source code as well as\n");
    printf("assessment solutions from another person AI or other places. It is the students'\n");
    printf("responsibility to guarantee that their assessment solutions are their own work.\n");
    printf("Meanwhile, the students must also ensure that their work is not accessible by others.\n");
    printf("Where such plagiarism is detected, both of the assessments involved will receive ZERO mark.\n\n");
    printf("We hereby declare that:\n");
    printf("• We fully understand and agree to the abovementioned plagiarism policy.\n");
    printf("• We did not copy any code from others or from other places.\n");
    printf("• We did not share our codes with others or upload to any other places for public access\n");
    printf("  and will not do that in the future.\n");
    printf("• We agree that our project will receive Zero mark if there is any plagiarism detected.\n");
    printf("• We agree that we will not disclose any information or material of the group project\n");
    printf("  to others or upload to any other places for public access.\n");
    printf("• We agree that we did not copy any code directly from AI generated sources.\n\n");
    printf("Declared by: Group P4_6\n");
    printf("Team members:\n");
    printf("1. Goh Jia Xin\n");
    printf("2. Chourasia Anushka\n");
    printf("3. Chye Zhi Hao\n");
    printf("4. Goh Jason Fadil\n");
    printf("5. Goh Li Xuan\n");
    printf("Date: November 25, 2025\n");
    printf("========================================\n\n");
}

void openDatabase() {
    // Free existing data before loading new
    if (student_records != NULL) {
        free(student_records);
        student_records = NULL;
    }
    capacity = 0;
    recordCount = 0;
    
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("CMS: Database file \"%s\" not found. Creating new database.\n", FILENAME);
        isFileOpen = 1;
        recordCount = 0;
        return;
    }

    char line[300];
    int lineNum = 0;

    while (fgets(line, sizeof(line), file)) {
        lineNum++;
        
        if (!ensureCapacity(recordCount + 1)) {
            printf("CMS: Error - Unable to allocate memory.\n");
            fclose(file);
            return;
        }

        int id;
        char name[MAX_NAME];
        char programme[MAX_PROGRAMME];
        float mark;
        char grade[3];

        // Try reading with grade first
        int items = sscanf(line, "%d\t%99[^\t]\t%99[^\t]\t%f\t%2s",
                           &id, name, programme, &mark, grade);
        
        if (items == 5) {
            // Has grade field - validate data
            if (id <= 0) {
                printf("CMS: Warning - Line %d: Invalid ID (%d), skipping.\n", lineNum, id);
                continue;
            }
            if (strlen(name) == 0 || isAllWhitespace(name)) {
                printf("CMS: Warning - Line %d: Empty name (ID=%d), skipping.\n", lineNum, id);
                continue;
            }
            if (strlen(programme) == 0 || isAllWhitespace(programme)) {
                printf("CMS: Warning - Line %d: Empty programme (ID=%d), skipping.\n", lineNum, id);
                continue;
            }
            // Validate programme format from file
            if (!isValidProgramme(programme)) {
                printf("CMS: Warning - Line %d: Invalid programme format '%s' (ID=%d), skipping.\n", lineNum, programme, id);
                continue;
            }
            if (mark < 0 || mark > 100) {
                printf("CMS: Warning - Line %d: Invalid mark %.1f (ID=%d), skipping.\n", lineNum, mark, id);
                continue;
            }
            if (idExists(id)) {
                printf("CMS: Warning - Line %d: Duplicate ID %d, skipping.\n", lineNum, id);
                continue;
            }
            
            // Check for invalid characters in name and programme
            if (containsDangerousChars(name)) {
                printf("CMS: Warning - Line %d: Name contains invalid characters / | & # (ID=%d), skipping.\n", lineNum, id);
                continue;
            }
            if (containsDangerousChars(programme)) {
                printf("CMS: Warning - Line %d: Programme contains invalid characters / | & # (ID=%d), skipping.\n", lineNum, id);
                continue;
            }
            
            // Check for command injection characters
            if (containsCommandInjectionChars(name)) {
                printf("CMS: Warning - Line %d: Name contains command injection characters (ID=%d), skipping.\n", lineNum, id);
                continue;
            }
            if (containsCommandInjectionChars(programme)) {
                printf("CMS: Warning - Line %d: Programme contains command injection characters (ID=%d), skipping.\n", lineNum, id);
                continue;
            }
            
            student_records[recordCount].ID = id;
            strcpy(student_records[recordCount].Name, name);
            strcpy(student_records[recordCount].Programme, programme);
            student_records[recordCount].Mark = mark;
            strcpy(student_records[recordCount].Grade, grade);
            recordCount++;
            
        } else if (items == 4) {
            // No grade field, calculate it - validate data
            if (id <= 0) {
                printf("CMS: Warning - Line %d: Invalid ID (%d), skipping.\n", lineNum, id);
                continue;
            }
            if (strlen(name) == 0 || isAllWhitespace(name)) {
                printf("CMS: Warning - Line %d: Empty name (ID=%d), skipping.\n", lineNum, id);
                continue;
            }
            if (strlen(programme) == 0 || isAllWhitespace(programme)) {
                printf("CMS: Warning - Line %d: Empty programme (ID=%d), skipping.\n", lineNum, id);
                continue;
            }
            // Validate programme format from file
            if (!isValidProgramme(programme)) {
                printf("CMS: Warning - Line %d: Invalid programme format '%s' (ID=%d), skipping.\n", lineNum, programme, id);
                continue;
            }
            if (mark < 0 || mark > 100) {
                printf("CMS: Warning - Line %d: Invalid mark %.1f (ID=%d), skipping.\n", lineNum, mark, id);
                continue;
            }
            if (idExists(id)) {
                printf("CMS: Warning - Line %d: Duplicate ID %d, skipping.\n", lineNum, id);
                continue;
            }
            
            // Check for invalid characters in name and programme
            if (containsDangerousChars(name)) {
                printf("CMS: Warning - Line %d: Name contains invalid characters / | & # (ID=%d), skipping.\n", lineNum, id);
                continue;
            }
            if (containsDangerousChars(programme)) {
                printf("CMS: Warning - Line %d: Programme contains invalid characters / | & # (ID=%d), skipping.\n", lineNum, id);
                continue;
            }
            
            // Check for command injection characters
            if (containsCommandInjectionChars(name)) {
                printf("CMS: Warning - Line %d: Name contains command injection characters (ID=%d), skipping.\n", lineNum, id);
                continue;
            }
            if (containsCommandInjectionChars(programme)) {
                printf("CMS: Warning - Line %d: Programme contains command injection characters (ID=%d), skipping.\n", lineNum, id);
                continue;
            }
            
            // Data is valid
            student_records[recordCount].ID = id;
            strcpy(student_records[recordCount].Name, name);
            strcpy(student_records[recordCount].Programme, programme);
            student_records[recordCount].Mark = mark;
            calculateGrade(mark, student_records[recordCount].Grade);
            recordCount++;
            
        } else {
            // Skip invalid lines
            printf("CMS: Warning - Line %d: Invalid format, skipping.\n", lineNum);
            continue;
        }
    }

    fclose(file);
    isFileOpen = 1;
    printf("CMS: The database file \"%s\" is successfully opened.\n", FILENAME);
    printf("CMS: Loaded %d valid records.\n", recordCount);
}

void showAll() {
    if (!isFileOpen) {
        printf("CMS: Please open the database first using OPEN command.\n");
        return;
    }

    if (recordCount == 0) {
        printf("CMS: No records found in the database.\n");
        return;
    }

    printf("CMS: Here are all the records found in the table \"StudentRecords\".\n");
    printf("%-10s %-25s %-30s %-10s\n", "ID", "Name", "Programme", "Mark");
    for (int i = 0; i < recordCount; i++) {
        printf("%-10d %-25s %-30s %-10.1f\n",
               student_records[i].ID,
               student_records[i].Name,
               student_records[i].Programme,
               student_records[i].Mark);
    }
}

void insertRecord(char *input) {
    if (!isFileOpen) {
        printf("CMS: Please open the database first using OPEN command.\n");
        return;
    }

    // Check for command injection in the entire input first
    if (containsCommandInjectionChars(input)) {
        printf("CMS: Security warning: Command injection attempt detected in input.\n");
        return;
    }

    // Check for duplicate fields first
    if (hasDuplicateFields(input)) {
        return;
    }

    int id = -1;
    char name[MAX_NAME] = "";
    char programme[MAX_PROGRAMME] = "";
    float mark = -1.0f;

    // Work with original input to preserve spacing
    char buffer[500];
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    // Skip "INSERT" keyword (case-insensitive)
    char *parseStart = buffer;
    char bufferLower[500];
    strncpy(bufferLower, buffer, sizeof(bufferLower) - 1);
    bufferLower[sizeof(bufferLower) - 1] = '\0';
    toLowerCase(bufferLower);
    
    if (strncmp(bufferLower, "insert ", 7) == 0) {
        parseStart = buffer + 7;
        // Skip spaces after INSERT
        while (*parseStart == ' ') parseStart++;
    }

    // Enhanced parsing with command injection checks
    
    // Find ID=
    char *idPos = strstr(parseStart, "ID=");
    if (!idPos) idPos = strstr(parseStart, "id=");
    if (!idPos) idPos = strstr(parseStart, "Id=");
    
    if (idPos) {
        char *idValue = idPos + 3;
        char idStr[20] = "";
        int i = 0;
        // Read until space or end
        while (idValue[i] && !isspace((unsigned char)idValue[i]) && i < 19) {
            idStr[i] = idValue[i];
            i++;
        }
        idStr[i] = '\0';
        
        // Check ID for command injection
        if (containsCommandInjectionChars(idStr)) {
            printf("CMS: Security warning: Command injection attempt detected in ID field.\n");
            return;
        }
        
        if (!parseID(idStr, &id)) {
            printf("CMS: Invalid ID format. ID must be 7 digits starting with 2\n");
            return;
        }
    }

    // Find Name=
    char *namePos = strstr(parseStart, "Name=");
    if (!namePos) namePos = strstr(parseStart, "name=");
    if (!namePos) namePos = strstr(parseStart, "NAME=");
    
    if (namePos) {
        char *nameStart = namePos + 5;
        
        // Find where Name ends - look for the next field (Programme= or Mark=)
        char *progPos = strstr(nameStart, "Programme=");
        if (!progPos) progPos = strstr(nameStart, "programme=");
        if (!progPos) progPos = strstr(nameStart, "PROGRAMME=");
        
        char *markPos = strstr(nameStart, "Mark=");
        if (!markPos) markPos = strstr(nameStart, "mark=");
        if (!markPos) markPos = strstr(nameStart, "MARK=");
        
        // Use whichever comes first
        char *nameEnd = progPos;
        if (!nameEnd || (markPos && markPos < nameEnd)) {
            nameEnd = markPos;
        }
        
        if (nameEnd) {
            // Copy name from nameStart to nameEnd
            int len = nameEnd - nameStart;
            if (len >= MAX_NAME) {
                printf("CMS: Name too long (max %d characters).\n", MAX_NAME - 1);
                return;
            }
            strncpy(name, nameStart, len);
            name[len] = '\0';
            
            // Trim trailing spaces
            trim(name);
            
            if (strlen(name) == 0 || isAllWhitespace(name)) {
                printf("CMS: Name cannot be empty.\n");
                return;
            }
            
            // ✅ FIXED: Check for dangerous characters in Name
            if (containsDangerousChars(name)) {
                printf("CMS: Name contains invalid characters. Cannot contain / | & # characters.\n");
                return;
            }
            
            // ✅ NEW: Check for command injection in Name
            if (containsCommandInjectionChars(name)) {
                printf("CMS: Security warning: Command injection attempt detected in Name field.\n");
                return;
            }
            
            if (!isSecureFieldValue(name)) {
                printf("CMS: Name contains invalid or potentially dangerous characters.\n");
                return;
            }
        }
    }

    // Find Programme=
    char *progPos = strstr(parseStart, "Programme=");
    if (!progPos) progPos = strstr(parseStart, "programme=");
    if (!progPos) progPos = strstr(parseStart, "PROGRAMME=");
    
    if (progPos) {
        char *progStart = progPos + 10;
        
        // Find where Programme ends - look for Mark=
        char *markPos = strstr(progStart, "Mark=");
        if (!markPos) markPos = strstr(progStart, "mark=");
        if (!markPos) markPos = strstr(progStart, "MARK=");
        
        if (markPos) {
            // Copy programme from progStart to markPos
            int len = markPos - progStart;
            if (len >= MAX_PROGRAMME) {
                printf("CMS: Programme name too long (max %d characters).\n", MAX_PROGRAMME - 1);
                return;
            }
            strncpy(programme, progStart, len);
            programme[len] = '\0';
            
            // Trim trailing spaces
            trim(programme);
            
            if (strlen(programme) == 0 || isAllWhitespace(programme)) {
                printf("CMS: Programme cannot be empty.\n");
                return;
            }
            
            // Check for dangerous characters in Programme
            if (containsDangerousChars(programme)) {
                printf("CMS: Programme contains invalid characters. Cannot contain / | & # characters.\n");
                return;
            }
            
            // Check for command injection in Programme
            if (containsCommandInjectionChars(programme)) {
                printf("CMS: Security warning: Command injection attempt detected in Programme field.\n");
                return;
            }
            
            if (!isSecureFieldValue(programme)) {
                printf("CMS: Programme contains invalid or potentially dangerous characters.\n");
                return;
            }
            
            if (!isValidProgramme(programme)) {
                printf("CMS: Invalid programme format. Programme must contain only letters, spaces, and hyphens.\n");
                printf("CMS: Examples: 'Computer Science', 'Software-Engineering', 'Information Technology'\n");
                return;
            }
        }
    }

    // Find Mark=
    char *markPos = strstr(parseStart, "Mark=");
    if (!markPos) markPos = strstr(parseStart, "mark=");
    if (!markPos) markPos = strstr(parseStart, "MARK=");
    
    if (markPos) {
        char *markValue = markPos + 5;
        char markStr[20] = "";
        int i = 0;
        // Read until space or end
        while (markValue[i] && !isspace((unsigned char)markValue[i]) && i < 19) {
            markStr[i] = markValue[i];
            i++;
        }
        markStr[i] = '\0';
        
        // Check Mark for command injection
        if (containsCommandInjectionChars(markStr)) {
            printf("CMS: Security warning: Command injection attempt detected in Mark field.\n");
            return;
        }
        
        if (!parseMark(markStr, &mark)) {
            printf("CMS: Invalid mark format. Mark must be a number between 0 and 100.\n");
            return;
        }
    }

    // Check if only ID is provided (to check existence)
    if (id != -1 && strlen(name) == 0 && strlen(programme) == 0 && mark < 0) {
        if (idExists(id)) {
            printf("CMS: The record with ID=%d already exists.\n", id);
        } else {
            printf("CMS: Please provide all fields: ID, Name, Programme, and Mark.\n");
        }
        return;
    }

    // Validate all fields are provided
    if (id == -1 || strlen(name) == 0 || strlen(programme) == 0 || mark < 0) {
        printf("CMS: Invalid input. Please provide all fields: ID, Name, Programme, and Mark.\n");
        return;
    }

    if (idExists(id)) {
        printf("CMS: The record with ID=%d already exists.\n", id);
        return;
    }

    // NEW: Check for duplicate record (all fields match)
    if (isDuplicateRecord(id, name, programme, mark)) {
        printf("CMS: Error - Duplicate record detected. A record with the same ID, Name, Programme, and Mark already exists.\n");
        return;
    }

    if (mark < 0 || mark > 100) {
        printf("CMS: Invalid mark. Mark should be between 0 and 100.\n");
        return;
    }

    if (!ensureCapacity(recordCount + 1)) {
        printf("CMS: Error - Unable to allocate memory for new record.\n");
        return;
    }

    student_records[recordCount].ID = id;
    strcpy(student_records[recordCount].Name, name);
    strcpy(student_records[recordCount].Programme, programme);
    student_records[recordCount].Mark = mark;
    calculateGrade(mark, student_records[recordCount].Grade);
    recordCount++;

    printf("CMS: A new record with ID=%d is successfully inserted.\n", id);
}

// ... (rest of the functions remain the same as previous version)

void queryRecord(char *input) {
    if (!isFileOpen) {
        printf("CMS: Please open the database first using OPEN command.\n");
        return;
    }

    // Check for command injection in QUERY
    if (containsCommandInjectionChars(input)) {
        printf("CMS: Security warning: Command injection attempt detected in query.\n");
        return;
    }

    // Check for duplicate fields in QUERY
    if (hasDuplicateFields(input)) {
        return;
    }

    int id = -1;

    // Work on a copy because strtok modifies the string
    char buffer[256];
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    // Parse: QUERY ID=2401234 (now case-insensitive for "ID=")
    char *token = strtok(buffer, " ");
    while (token != NULL) {
        char tokenLower[256];
        strncpy(tokenLower, token, sizeof(tokenLower) - 1);
        tokenLower[sizeof(tokenLower) - 1] = '\0';

        toLowerCase(tokenLower);  // make copy lowercase

        if (strncmp(tokenLower, "id=", 3) == 0) {
            // Check for the ID
            if (!parseID(token + 3, &id)) {
                printf("CMS: Invalid ID format. ID must be exactly 7 digits starting with 2.\n");
                return;
            }
            break;
        }

        token = strtok(NULL, " ");
    }

    if (id == -1) {
        printf("CMS: Invalid input. Please provide ID in format: QUERY ID=xxxxx\n");
        return;
    }

    int index = findIndexById(id);
    if (index == -1) {
        printf("CMS: The record with ID=%d does not exist.\n", id);
        return;
    }

    printf("CMS: The record with ID=%d is found in the data table.\n", id);
    printf("%-10s %-25s %-30s %-10s\n", "ID", "Name", "Programme", "Mark");
    printf("%-10d %-25s %-30s %-10.1f\n",
           student_records[index].ID,
           student_records[index].Name,
           student_records[index].Programme,
           student_records[index].Mark);
}
void updateRecord(char *input) {
    if (!isFileOpen) {
        printf("CMS: Please open the database first using OPEN command.\n");
        return;
    }

    // Check for command injection in UPDATE
    if (containsCommandInjectionChars(input)) {
        printf("CMS: Security warning: Command injection attempt detected in update.\n");
        return;
    }

    // Check for duplicate fields in UPDATE
    if (hasDuplicateFields(input)) {
        return;
    }

    int id = -1;
    char name[MAX_NAME] = "";
    char programme[MAX_PROGRAMME] = "";
    float mark = -1.0f;
    int has_name = 0, has_programme = 0, has_mark = 0;

    // Work with original input to preserve spacing
    char buffer[500];
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    // Skip "UPDATE" keyword (case-insensitive)
    char *parseStart = buffer;
    char bufferLower[500];
    strncpy(bufferLower, buffer, sizeof(bufferLower) - 1);
    bufferLower[sizeof(bufferLower) - 1] = '\0';
    toLowerCase(bufferLower);
    
    if (strncmp(bufferLower, "update ", 7) == 0) {
        parseStart = buffer + 7;
        // Skip spaces after UPDATE
        while (*parseStart == ' ') parseStart++;
    }

    // Find ID=
    char *idPos = strstr(parseStart, "ID=");
    if (!idPos) idPos = strstr(parseStart, "id=");
    if (!idPos) idPos = strstr(parseStart, "Id=");
    
    if (idPos) {
        char *idValue = idPos + 3;
        char idStr[20] = "";
        int i = 0;
        while (idValue[i] && !isspace((unsigned char)idValue[i]) && i < 19) {
            idStr[i] = idValue[i];
            i++;
        }
        idStr[i] = '\0';
        
        if (containsCommandInjectionChars(idStr)) {
            printf("CMS: Security warning: Command injection attempt detected in ID field.\n");
            return;
        }
        
        if (!parseID(idStr, &id)) {
            printf("CMS: Invalid ID format. ID must be 7 digits starting with 2\n");
            return;
        }
    }

    if (id == -1) {
        printf("CMS: Invalid input. Please provide ID in format: UPDATE ID=xxxxxxx field=value\n");
        return;
    }

    // Find record
    int index = findIndexById(id);
    if (index == -1) {
        printf("CMS: The record with ID=%d does not exist.\n", id);
        return;
    }

    // Find Name=
    char *namePos = strstr(parseStart, "Name=");
    if (!namePos) namePos = strstr(parseStart, "name=");
    if (!namePos) namePos = strstr(parseStart, "NAME=");
    
    if (namePos) {
        char *nameStart = namePos + 5;
        
        // Find where Name ends
        char *progPos = strstr(nameStart, "Programme=");
        if (!progPos) progPos = strstr(nameStart, "programme=");
        if (!progPos) progPos = strstr(nameStart, "PROGRAMME=");
        
        char *markPos = strstr(nameStart, "Mark=");
        if (!markPos) markPos = strstr(nameStart, "mark=");
        if (!markPos) markPos = strstr(nameStart, "MARK=");
        
        char *nameEnd = progPos;
        if (!nameEnd || (markPos && markPos < nameEnd)) {
            nameEnd = markPos;
        }
        
        if (nameEnd) {
            int len = nameEnd - nameStart;
            if (len >= MAX_NAME) {
                printf("CMS: Name too long (max %d characters).\n", MAX_NAME - 1);
                return;
            }
            strncpy(name, nameStart, len);
            name[len] = '\0';
            trim(name);
            has_name = 1;
        } else {
            // Name is at the end
            strncpy(name, nameStart, sizeof(name) - 1);
            name[sizeof(name) - 1] = '\0';
            trim(name);
            has_name = 1;
        }
    }

    // Find Programme=
    char *progPos = strstr(parseStart, "Programme=");
    if (!progPos) progPos = strstr(parseStart, "programme=");
    if (!progPos) progPos = strstr(parseStart, "PROGRAMME=");
    
    if (progPos) {
        char *progStart = progPos + 10;
        
        char *markPos = strstr(progStart, "Mark=");
        if (!markPos) markPos = strstr(progStart, "mark=");
        if (!markPos) markPos = strstr(progStart, "MARK=");
        
        char *namePos2 = strstr(progStart, "Name=");
        if (!namePos2) namePos2 = strstr(progStart, "name=");
        if (!namePos2) namePos2 = strstr(progStart, "NAME=");
        
        char *progEnd = markPos;
        if (!progEnd || (namePos2 && namePos2 < progEnd)) {
            progEnd = namePos2;
        }
        
        if (progEnd) {
            int len = progEnd - progStart;
            if (len >= MAX_PROGRAMME) {
                printf("CMS: Programme name too long (max %d characters).\n", MAX_PROGRAMME - 1);
                return;
            }
            strncpy(programme, progStart, len);
            programme[len] = '\0';
            trim(programme);
            has_programme = 1;
        } else {
            // Programme is at the end
            strncpy(programme, progStart, sizeof(programme) - 1);
            programme[sizeof(programme) - 1] = '\0';
            trim(programme);
            has_programme = 1;
        }
    }

    // Find Mark=
    char *markPos = strstr(parseStart, "Mark=");
    if (!markPos) markPos = strstr(parseStart, "mark=");
    if (!markPos) markPos = strstr(parseStart, "MARK=");
    
    if (markPos) {
        char *markValue = markPos + 5;
        char markStr[20] = "";
        int i = 0;
        while (markValue[i] && !isspace((unsigned char)markValue[i]) && i < 19) {
            markStr[i] = markValue[i];
            i++;
        }
        markStr[i] = '\0';
        
        if (containsCommandInjectionChars(markStr)) {
            printf("CMS: Security warning: Command injection attempt detected in Mark field.\n");
            return;
        }
        
        if (!parseMark(markStr, &mark)) {
            printf("CMS: Invalid mark format. Mark must be a number between 0 and 100.\n");
            return;
        }
        has_mark = 1;
    }

    // Check if at least one field to update was provided
    if (!has_name && !has_programme && !has_mark) {
        printf("CMS: Invalid UPDATE format.\n");
        printf("CMS: Example: UPDATE ID=2123456 Programme=Applied AI\n");
        printf("CMS: Supported fields: Name, Programme, Mark\n");
        return;
    }

    StudentRecords *rec = &student_records[index];

    // Update Name if provided
    if (has_name) {
        if (strlen(name) == 0 || isAllWhitespace(name)) {
            printf("CMS: Name cannot be empty.\n");
            return;
        }
        if (containsDangerousChars(name)) {
            printf("CMS: Name contains invalid characters. Cannot contain / | & # characters.\n");
            return;
        }
        if (containsCommandInjectionChars(name)) {
            printf("CMS: Security warning: Command injection attempt detected in Name field.\n");
            return;
        }
        if (!isSecureFieldValue(name)) {
            printf("CMS: Name contains invalid or potentially dangerous characters.\n");
            return;
        }
        strncpy(rec->Name, name, sizeof(rec->Name) - 1);
        rec->Name[sizeof(rec->Name) - 1] = '\0';
    }

    // Update Programme if provided
    if (has_programme) {
        if (strlen(programme) == 0 || isAllWhitespace(programme)) {
            printf("CMS: Programme cannot be empty.\n");
            return;
        }
        if (containsDangerousChars(programme)) {
            printf("CMS: Programme contains invalid characters. Cannot contain / | & # characters.\n");
            return;
        }
        if (containsCommandInjectionChars(programme)) {
            printf("CMS: Security warning: Command injection attempt detected in Programme field.\n");
            return;
        }
        if (!isSecureFieldValue(programme)) {
            printf("CMS: Programme contains invalid or potentially dangerous characters.\n");
            return;
        }
        if (!isValidProgramme(programme)) {
            printf("CMS: Invalid programme format. Programme must contain only letters, spaces, and hyphens.\n");
            printf("CMS: Examples: 'Computer Science', 'Software-Engineering', 'Information Technology'\n");
            return;
        }
        strncpy(rec->Programme, programme, sizeof(rec->Programme) - 1);
        rec->Programme[sizeof(rec->Programme) - 1] = '\0';
    }

    // Update Mark if provided
    if (has_mark) {
        rec->Mark = mark;
        calculateGrade(mark, rec->Grade);
    }

    printf("CMS: The record with ID=%d is successfully updated.\n", id);
}
void deleteRecord(char *input) {
    if (!isFileOpen) {
        printf("CMS: Please open the database first using OPEN command.\n");
        return;
    }

    // Check for command injection in DELETE
    if (containsCommandInjectionChars(input)) {
        printf("CMS: Security warning: Command injection attempt detected in delete.\n");
        return;
    }

    // Check for duplicate fields in DELETE
    if (hasDuplicateFields(input)) {
        return;
    }

    int id = -1;

    // Work on a copy because strtok modifies the string
    char buffer[256];
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    // Parse: DELETE ID=2401234 (now case-insensitive)
    char *token = strtok(buffer, " ");
    while (token != NULL) {
        // Convert token to lowercase for case-insensitive comparison
        char tokenLower[256];
        strncpy(tokenLower, token, sizeof(tokenLower) - 1);
        tokenLower[sizeof(tokenLower) - 1] = '\0';
        toLowerCase(tokenLower);

        if (strncmp(tokenLower, "id=", 3) == 0) {
            // Check for ID - use original token to preserve case for parsing
            if (!parseID(token + 3, &id)) {
                printf("CMS: Invalid ID format. ID must be exactly 7 digits starting with '2'.\n");
                return;
            }
            break;
        }
        token = strtok(NULL, " ");
    }

    if (id == -1) {
        printf("CMS: Invalid input. Please provide ID in format: DELETE ID=xxxxx\n");
        return;
    }

    int index = findIndexById(id);
    if (index == -1) {
        printf("CMS: The record with ID=%d does not exist.\n", id);
        return;
    }

    char confirm;
    printf("CMS: Are you sure you want to delete record with ID=%d? Type \"Y\" to Confirm or type \"N\" to cancel.\n", id);
    printf("P4_6: ");
    if (scanf(" %c", &confirm) != 1) {
        printf("CMS: Invalid confirmation input.\n");
        // Clear leftover
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
        return;
    }
    // Clear leftover newline
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);

    if (confirm == 'Y' || confirm == 'y') {
        for (int i = index; i < recordCount - 1; i++) {
            student_records[i] = student_records[i + 1];
        }
        recordCount--;
        printf("CMS: The record with ID=%d is successfully deleted.\n", id);
    } else {
        printf("CMS: The deletion is cancelled.\n");
    }
}

void saveDatabase() {
    if (!isFileOpen) {
        printf("CMS: Please open the database first using OPEN command.\n");
        return;
    }

    FILE *file = fopen(FILENAME, "w");
    if (file == NULL) {
        printf("CMS: Error - Unable to save to file \"%s\".\n", FILENAME);
        return;
    }

    for (int i = 0; i < recordCount; i++) {
        fprintf(file, "%d\t%s\t%s\t%.1f\t%s\n",
                student_records[i].ID,
                student_records[i].Name,
                student_records[i].Programme,
                student_records[i].Mark,
                student_records[i].Grade);
    }

    fclose(file);
    printf("CMS: The database file \"%s\" is successfully saved.\n", FILENAME);
}

void showSummary() {
    if (!isFileOpen) {
        printf("CMS: Please open the database first using OPEN command.\n");
        return;
    }

    if (recordCount == 0) {
        printf("CMS: No records found in the database.\n");
        return;
    }

    float sum = 0.0f;
    float highest = student_records[0].Mark;
    float lowest = student_records[0].Mark;
    int highIndex = 0;
    int lowIndex = 0;

    for (int i = 0; i < recordCount; i++) {
        sum += student_records[i].Mark;
        if (student_records[i].Mark > highest) {
            highest = student_records[i].Mark;
            highIndex = i;
        }
        if (student_records[i].Mark < lowest) {
            lowest = student_records[i].Mark;
            lowIndex = i;
        }
    }

    float average = sum / recordCount;
    printf("\n=== Summary Statistics ===\n");
    printf("Total number of students: %d\n", recordCount);
    printf("Average mark: %.2f\n", average);
    printf("Highest mark: %.1f (Student: %s, Grade: %s)\n",
           highest, student_records[highIndex].Name, student_records[highIndex].Grade);
    printf("Lowest mark: %.1f (Student: %s, Grade: %s)\n",
           lowest, student_records[lowIndex].Name, student_records[lowIndex].Grade);
    printf("==========================\n\n");
}

void sortByID(int ascending) {
    if (!isFileOpen || recordCount == 0) return;

    for (int i = 0; i < recordCount - 1; i++) {
        for (int j = 0; j < recordCount - i - 1; j++) {
            int swap = ascending ?
                (student_records[j].ID > student_records[j + 1].ID) :
                (student_records[j].ID < student_records[j + 1].ID);
            if (swap) {
                StudentRecords temp = student_records[j];
                student_records[j] = student_records[j + 1];
                student_records[j + 1] = temp;
            }
        }
    }

    printf("CMS: Records sorted by ID (%s).\n", ascending ? "ascending" : "descending");
}

void sortByMark(int ascending) {
    if (!isFileOpen || recordCount == 0) return;

    for (int i = 0; i < recordCount - 1; i++) {
        for (int j = 0; j < recordCount - i - 1; j++) {
            int swap = ascending ?
                (student_records[j].Mark > student_records[j + 1].Mark) :
                (student_records[j].Mark < student_records[j + 1].Mark);
            if (swap) {
                StudentRecords temp = student_records[j];
                student_records[j] = student_records[j + 1];
                student_records[j + 1] = temp;
            }
        }
    }

    printf("CMS: Records sorted by Mark (%s).\n", ascending ? "ascending" : "descending");
}

void calculateGrade(float mark, char *grade) {
    if (mark >= 85) strcpy(grade, "A");
    else if (mark >= 70) strcpy(grade, "B");
    else if (mark >= 60) strcpy(grade, "C");
    else if (mark >= 50) strcpy(grade, "D");
    else strcpy(grade, "F");
}

void trim(char *str) {
    char *start = str;
    char *end;

    while (isspace((unsigned char)*start)) start++;
    if (*start == 0) {
        *str = 0;
        return;
    }

    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;
    *(end + 1) = 0;

    memmove(str, start, end - start + 2);
}

void toLowerCase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = (char)tolower((unsigned char)str[i]);
    }
}

//Filter by Programme
void filterByProgramme() {
    if (!isFileOpen) {
        printf("CMS: Please open the database first using OPEN command.\n");
        return;
    }

    if (recordCount == 0) {
        printf("CMS: No records found in the database.\n");
        return;
    }

    // List unique programmes
    printf("CMS: Available programmes in the database:\n");

    int printedCount = 0;
    for (int i = 0; i < recordCount; i++) {
        int alreadyPrinted = 0;
        for (int j = 0; j < i; j++) {
            // Case-insensitive comparison for uniqueness
            char prog1Lower[MAX_PROGRAMME];
            char prog2Lower[MAX_PROGRAMME];
            
            strcpy(prog1Lower, student_records[i].Programme);
            strcpy(prog2Lower, student_records[j].Programme);
            
            toLowerCase(prog1Lower);
            toLowerCase(prog2Lower);
            
            if (strcmp(prog1Lower, prog2Lower) == 0) {
                alreadyPrinted = 1;
                break;
            }
        }
        if (!alreadyPrinted) {
            printf("  - %s\n", student_records[i].Programme);
            printedCount++;
        }
    }

    if (printedCount == 0) {
        printf("CMS: No programmes found.\n");
        return;
    }

    // Prompt user to select programme
    char selected[MAX_PROGRAMME];
    printf("CMS: Please type the programme name (case insensitive):\n");
    printf("P4_6: ");
    if (!fgets(selected, sizeof(selected), stdin)) {
        printf("CMS: Input error.\n");
        return;
    }
    trim_newline(selected);
    trim(selected);  // Trim whitespace

    if (strlen(selected) == 0) {
        printf("CMS: Invalid input. Programme name cannot be empty.\n");
        return;
    }

    //  Convert input to lowercase for comparison
    char selectedLower[MAX_PROGRAMME];
    strcpy(selectedLower, selected);
    toLowerCase(selectedLower);

    // Validate that the programme exists (case-insensitive)
    int exists = 0;
    char matchedProgramme[MAX_PROGRAMME] = "";  // Store the actual programme name
    
    for (int i = 0; i < recordCount; i++) {
        char progLower[MAX_PROGRAMME];
        strcpy(progLower, student_records[i].Programme);
        toLowerCase(progLower);

        if (strcmp(progLower, selectedLower) == 0) {
            exists = 1;
            // Store the first matched programme name for display
            if (strlen(matchedProgramme) == 0) {
                strcpy(matchedProgramme, student_records[i].Programme);
            }
            break;
        }
    }

    if (!exists) {
        printf("CMS: Invalid programme. Please choose a programme from the list.\n");
        printf("CMS: Exiting Filter..\n");
        return;
    }

    // Print filtered table using the matched programme name
    printf("CMS: Records for programme \"%s\":\n", matchedProgramme);
    printf("%-10s %-25s %-30s %-10s\n", "ID", "Name", "Programme", "Mark");
    int found = 0;
    
    for (int i = 0; i < recordCount; i++) {
        char progLower[MAX_PROGRAMME];
        strcpy(progLower, student_records[i].Programme);
        toLowerCase(progLower);

        if (strcmp(progLower, selectedLower) == 0) {
            printf("%-10d %-25s %-30s %-10.1f\n",
                   student_records[i].ID,
                   student_records[i].Name,
                   student_records[i].Programme,
                   student_records[i].Mark);
            found = 1;
        }
    }

    if (!found) {
        // Should not happen if 'exists' is true, but safety check
        printf("CMS: No records found for the selected programme.\n");
    }
}

// Filter by Mark Range
void filterByMarkRange() {
    if (!isFileOpen) {
        printf("CMS: Please open the database first using OPEN command.\n");
        return;
    }

    if (recordCount == 0) {
        printf("CMS: No records found in the database.\n");
        return;
    }

    char input[200];
    float low, high;

    printf("CMS: Please enter mark range in format XX - XX (e.g. 50 - 80):\n");
    printf("P4_6: ");
    if (!fgets(input, sizeof(input), stdin)) {
        printf("CMS: Input error.\n");
        return;
    }
    trim_newline(input);
    trim(input);  //  Trim whitespace

    //  Check for empty input
    if (strlen(input) == 0) {
        printf("CMS: Input cannot be empty.\n");
        return;
    }

    //  More flexible parsing - try different formats
    int parsed = 0;
    
    // Try format: "XX - XX" (with spaces around dash)
    if (sscanf(input, "%f - %f", &low, &high) == 2) {
        parsed = 1;
    }
    // Try format: "XX-XX" (no spaces)
    else if (sscanf(input, "%f-%f", &low, &high) == 2) {
        parsed = 1;
    }
    // Try format: "XX  -  XX" (multiple spaces)
    else {
        // Manual parsing for more flexibility
        char *dash = strchr(input, '-');
        if (dash != NULL) {
            *dash = '\0';  // Split at dash
            char *lowStr = input;
            char *highStr = dash + 1;
            
            // Trim both parts
            trim(lowStr);
            trim(highStr);
            
            // Try to parse
            if (parseMark(lowStr, &low) && parseMark(highStr, &high)) {
                parsed = 1;
            }
        }
    }

    if (!parsed) {
        printf("CMS: Invalid input format. Please use: XX - XX (e.g. 50 - 80).\n");
        printf("CMS: You can also use: XX-XX or XX  -  XX\n");
        return;
    }

    //  Validate range
    if (low < 0 || high > 100) {
        printf("CMS: Invalid range. Both marks must be between 0 and 100.\n");
        return;
    }
    
    if (low > high) {
        printf("CMS: Invalid range. Lower bound (%.1f) cannot be greater than upper bound (%.1f).\n", low, high);
        return;
    }

    // Print filtered table
    printf("CMS: Records with marks between %.1f and %.1f (inclusive):\n", low, high);
    printf("%-10s %-25s %-30s %-10s\n", "ID", "Name", "Programme", "Mark");
    int found = 0;

    for (int i = 0; i < recordCount; i++) {
        if (student_records[i].Mark >= low && student_records[i].Mark <= high) {
            printf("%-10d %-25s %-30s %-10.1f\n",
                   student_records[i].ID,
                   student_records[i].Name,
                   student_records[i].Programme,
                   student_records[i].Mark);
            found = 1;
        }
    }

    if (!found) {
        printf("CMS: No records found within the specified mark range.\n");
    }
}


static void trim_newline(char *s) {
    if (!s) return;
    size_t n = strlen(s);
    if (n && (s[n - 1] == '\n' || s[n - 1] == '\r')) s[n - 1] = '\0';
}

static int idExists(int id) {
    for (int i = 0; i < recordCount; i++) {
        if (student_records[i].ID == id) return 1;
    }
    return 0;
}

static int ensureCapacity(int want) {
    // Add maximum record limit to prevent memory exhaustion attacks
    if (want > MAX_RECORDS) {
        printf("CMS: Maximum record limit (%d) reached.\n", MAX_RECORDS);
        return 0;
    }
    
    if (capacity >= want) return 1;
    int newCap = (capacity > 0) ? capacity : 16;
    while (newCap < want) newCap *= 2;
    StudentRecords *tmp = realloc(student_records, newCap * sizeof(*tmp));
    if (!tmp) {
        perror("realloc failed");
        return 0;
    }
    student_records = tmp;
    capacity = newCap;
    return 1;
}

static int findIndexById(int id) {
    for (int i = 0; i < recordCount; i++) {
        if (student_records[i].ID == id) return i;
    }
    return -1;
}

// ID validation
static int parseID(const char *str, int *id) {
    if (!str || *str == '\0') {
        return 0;  // Empty string
    }
    
    // Check for spaces in ID
    for (int i = 0; str[i]; i++) {
        if (isspace((unsigned char)str[i])) {
            return 0;
        }
    }
    
    // Check if starts with '2'
    if (*str != '2') {
        return 0;  // Must start with 2
    }
    
    // Check total length is exactly 7 digits
    if (strlen(str) != 7) {
        return 0;  // Must be exactly 7 digits
    }
    
    // Check all characters are digits
    for (int i = 0; i < 7; i++) {
        if (!isdigit((unsigned char)str[i])) {
            return 0;  // Contains non-digit characters
        }
    }
    
    char *endptr;
    errno = 0;
    long val = strtol(str, &endptr, 10);
    
    // Check for various error conditions
    if (errno == ERANGE) {
        return 0;  // Overflow/underflow
    }
    if (*endptr != '\0') {
        return 0;  // Contains non-numeric characters
    }
    if (str == endptr) {
        return 0;  // No conversion performed
    }
    if (val < 2000000 || val > 2999999) {
        return 0;  // Out of valid range (must be 2000000-2999999)
    }
    
    *id = (int)val;
    return 1;  // Success
}

// Robust mark parsing function
static int parseMark(const char *str, float *mark) {
    if (!str || *str == '\0') {
        return 0;  // Empty string
    }
    
    // Check if starts with a digit, negative sign, or decimal point
    if (!isdigit((unsigned char)*str) && *str != '-' && *str != '+' && *str != '.') {
        return 0;  // Invalid start character
    }
    
    char *endptr;
    errno = 0;
    float val = strtof(str, &endptr);
    
    // Check for various error conditions
    if (errno == ERANGE) {
        return 0;  // Overflow/underflow
    }
    if (*endptr != '\0') {
        // Check if remaining characters are just whitespace
        while (*endptr && isspace((unsigned char)*endptr)) {
            endptr++;
        }
        if (*endptr != '\0') {
            return 0;  // Contains non-numeric characters
        }
    }
    if (str == endptr) {
        return 0;  // No conversion performed
    }
    if (val < 0.0f || val > 100.0f) {
        return 0;  // Out of valid range
    }
    
    *mark = val;
    return 1;  // Success
}


static int isValidInputLength(const char *input) {
    return (input != NULL && strlen(input) < 256); // Match your buffer size
}

static int containsInvalidChars(const char *str, const char *invalid_chars) {
    if (!str) return 0;
    for (int i = 0; str[i]; i++) {
        if (strchr(invalid_chars, str[i]) != NULL) {
            return 1;
        }
    }
    return 0;
}

static int isReasonableString(const char *str) {
    if (!str) return 0;
    
    // Check length
    if (strlen(str) > 100) return 0;
    
    // Check for reasonable character distribution
    int alpha_count = 0, space_count = 0, other_count = 0;
    for (int i = 0; str[i]; i++) {
        if (isalpha((unsigned char)str[i])) alpha_count++;
        else if (isspace((unsigned char)str[i])) space_count++;
        else if (isprint((unsigned char)str[i])) other_count++;
        else return 0; // Non-printable character
    }
    
    // Names should be mostly alphabetic
    if (alpha_count < strlen(str) * 0.6) return 0;
    
    return 1;
}

static int isAllWhitespace(const char *str) {
    if (!str) return 1;
    while (*str) {
        if (!isspace((unsigned char)*str)) {
            return 0;
        }
        str++;
    }
    return 1;
}

static int isValidFieldValue(const char *str) {
    if (!str) return 0;
    
    // Disallow tabs and other problematic characters
    const char *invalid_chars = "\t\n\r";
    if (containsInvalidChars(str, invalid_chars)) {
        return 0;
    }
    
    // restrict to printable ASCII
    for (int i = 0; str[i]; i++) {
        if ((unsigned char)str[i] < 32 || (unsigned char)str[i] > 126) {
            return 0;
        }
    }
    
    return 1;
}

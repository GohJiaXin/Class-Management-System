#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME 100
#define MAX_PROGRAMME 100
#define MAX_INPUT 500
#define FILENAME "P4_6-CMS.txt"

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
void sortRecords(char *input);
void sortByID(int ascending);
void sortByMark(int ascending);
void sortByName(int ascending);
void sortByProgramme(int ascending);
void trim(char *str);
void toLowerCase(char *str);
void calculateGrade(float mark, char *grade);
int sanitizeInput(char *input);
int validateID(int id);
int validateMark(float mark);
int sanitizeName(char *name);
int sanitizeProgramme(char *programme);

// Helper functions
static void trim_newline(char *s);
static int idExists(int id);
static int ensureCapacity(int want);
static int findIndexById(int id);
static int findRecordByID(int id);
static int isAlphaOnly(const char *s);


// Return index of record with given id, or -1 if not found 
static int findRecordByID(int id) {
    for (int i = 0; i < recordCount; i++) {
        if (student_records[i].ID == id) {
            return i;
        }
    }
    return -1;
}

// Return 1 if string contains only letters and spaces, else 0 
static int isAlphaOnly(const char *s) {
    for (; *s != '\0'; s++) {
        if (!isalpha((unsigned char)*s) && !isspace((unsigned char)*s)) {
            return 0;
        }
    }
    return 1;
}



int main() {
    char input[MAX_INPUT];
    char command[MAX_INPUT];
    
    printDeclaration();
    
    printf("\n=== Class Management System ===\n");
    printf("Available commands:\n");
    printf("  OPEN, SHOW ALL, INSERT, QUERY, UPDATE, DELETE, SAVE, SHOW SUMMARY, EXIT\n");
    printf("  SORT BY [ID|NAME|PROGRAMME|MARK] [ASC|DESC]\n\n");
    
    while (1) {
        printf("\nP4_6: ");
        if (!fgets(input, sizeof(input), stdin)) {
            continue;
        }
        
        // Input sanitization
        if (!sanitizeInput(input)) {
            printf("CMS: Invalid input detected. Please try again.\n");
            continue;
        }
        
        strcpy(command, input);
        toLowerCase(command);
        trim(command);
        
        if (strcmp(command, "exit") == 0) {
            printf("CMS: Exiting program. Goodbye!\n");
            free(student_records);
            break;
        }
        else if (strcmp(command, "open") == 0) {
            openDatabase();
        }
        else if (strcmp(command, "show all") == 0) {
            showAll();
        }
        else if (strncmp(command, "sort by", 7) == 0) {
            sortRecords(input);
        }
        else if (strncmp(command, "insert", 6) == 0) {
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

void printDeclaration() {
    printf("\n========================================\n");
    printf("         Declaration\n");
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

// Enhanced input sanitization
int sanitizeInput(char *input) {
    if (!input) return 0;
    
    // Remove newline
    input[strcspn(input, "\n")] = 0;
    input[strcspn(input, "\r")] = 0;
    
    // Check for empty input
    trim(input);
    if (strlen(input) == 0) return 0;
    
    // Check for dangerous characters or patterns
    if (strstr(input, "..") != NULL) return 0;  // Path traversal
    if (strchr(input, '\0') != input + strlen(input)) return 0;  // Null byte injection
    
    // Limit input length
    if (strlen(input) >= MAX_INPUT - 1) {
        input[MAX_INPUT - 1] = '\0';
    }
    
    return 1;
}

int validateID(int id) {
    if (id <= 0 || id > 99999999) {
        printf("CMS: Invalid ID. ID must be a positive number (1-99999999).\n");
        return 0;
    }
    return 1;
}

int validateMark(float mark) {
    if (mark < 0 || mark > 100) {
        printf("CMS: Invalid mark. Mark must be between 0 and 100.\n");
        return 0;
    }
    return 1;
}

int sanitizeName(char *name) {
    if (!name || strlen(name) == 0) {
        printf("CMS: Name cannot be empty.\n");
        return 0;
    }
    
    trim(name);
    
    if (strlen(name) >= MAX_NAME) {
        printf("CMS: Name too long. Maximum %d characters.\n", MAX_NAME - 1);
        return 0;
    }
    
    // Check for invalid characters
    for (int i = 0; name[i]; i++) {
        if (!isalnum(name[i]) && name[i] != ' ' && name[i] != '-' && name[i] != '\'') {
            printf("CMS: Name contains invalid characters.\n");
            return 0;
        }
    }
    
    return 1;
}

int sanitizeProgramme(char *programme) {
    if (!programme || strlen(programme) == 0) {
        printf("CMS: Programme cannot be empty.\n");
        return 0;
    }
    
    trim(programme);
    
    if (strlen(programme) >= MAX_PROGRAMME) {
        printf("CMS: Programme name too long. Maximum %d characters.\n", MAX_PROGRAMME - 1);
        return 0;
    }
    
    return 1;
}

void sortRecords(char *input) {
    if (!isFileOpen) {
        printf("CMS: Please open the database first using OPEN command.\n");
        return;
    }
    
    if (recordCount == 0) {
        printf("CMS: No records to sort.\n");
        return;
    }
    
    char command[MAX_INPUT];
    strcpy(command, input);
    toLowerCase(command);
    
    int ascending = 1;  // Default to ascending
    
    // Check for ASC or DESC
    if (strstr(command, "desc") != NULL) {
        ascending = 0;
    }
    
    // Determine sort field
    if (strstr(command, "sort by id") != NULL) {
        sortByID(ascending);
        showAll();
    }
    else if (strstr(command, "sort by name") != NULL) {
        sortByName(ascending);
        showAll();
    }
    else if (strstr(command, "sort by programme") != NULL) {
        sortByProgramme(ascending);
        showAll();
    }
    else if (strstr(command, "sort by mark") != NULL) {
        sortByMark(ascending);
        showAll();
    }
    else {
        printf("CMS: Invalid sort command. Use: SORT BY [ID|NAME|PROGRAMME|MARK] [ASC|DESC]\n");
    }
}

void sortByID(int ascending) {
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

void sortByName(int ascending) {
    for (int i = 0; i < recordCount - 1; i++) {
        for (int j = 0; j < recordCount - i - 1; j++) {
            int cmp = strcmp(student_records[j].Name, student_records[j + 1].Name);
            int swap = ascending ? (cmp > 0) : (cmp < 0);
            
            if (swap) {
                StudentRecords temp = student_records[j];
                student_records[j] = student_records[j + 1];
                student_records[j + 1] = temp;
            }
        }
    }
    
    printf("CMS: Records sorted by Name (%s).\n", ascending ? "ascending" : "descending");
}

void sortByProgramme(int ascending) {
    for (int i = 0; i < recordCount - 1; i++) {
        for (int j = 0; j < recordCount - i - 1; j++) {
            int cmp = strcmp(student_records[j].Programme, student_records[j + 1].Programme);
            int swap = ascending ? (cmp > 0) : (cmp < 0);
            
            if (swap) {
                StudentRecords temp = student_records[j];
                student_records[j] = student_records[j + 1];
                student_records[j + 1] = temp;
            }
        }
    }
    
    printf("CMS: Records sorted by Programme (%s).\n", ascending ? "ascending" : "descending");
}

void openDatabase() {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("CMS: Database file \"%s\" not found. Creating new database.\n", FILENAME);
        isFileOpen = 1;
        recordCount = 0;
        return;
    }
    
    recordCount = 0;
    char line[MAX_INPUT];
    
    while (fgets(line, sizeof(line), file)) {
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
            student_records[recordCount].ID = id;
            strcpy(student_records[recordCount].Name, name);
            strcpy(student_records[recordCount].Programme, programme);
            student_records[recordCount].Mark = mark;
            strcpy(student_records[recordCount].Grade, grade);
        } else if (items == 4) {
            student_records[recordCount].ID = id;
            strcpy(student_records[recordCount].Name, name);
            strcpy(student_records[recordCount].Programme, programme);
            student_records[recordCount].Mark = mark;
            calculateGrade(mark, student_records[recordCount].Grade);
        } else {
            continue;
        }
        
        recordCount++;
    }
    
    fclose(file);
    isFileOpen = 1;
    printf("CMS: The database file \"%s\" is successfully opened.\n", FILENAME);
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
    printf("%-10s %-25s %-30s %-10s %-6s\n", "ID", "Name", "Programme", "Mark", "Grade");
    printf("----------------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < recordCount; i++) {
        printf("%-10d %-25s %-30s %-10.1f %-6s\n",
               student_records[i].ID,
               student_records[i].Name,
               student_records[i].Programme,
               student_records[i].Mark,
               student_records[i].Grade);
    }
}

void insertRecord(char *input) {
    // Check if database is open
    if (!isFileOpen) {
        printf("CMS: Please open the database first using OPEN command.\n");
        return;
    }

    int id;
    char name[MAX_NAME];
    char programme[MAX_PROGRAMME];
    float mark;

    char line[512];
    // copy original command line
    strncpy(line, input, sizeof(line) - 1);

    // ensure null termination by adding '\0' at the end
    line[sizeof(line) - 1] = '\0';

    // remove whitespace characters from both ends
    trim(line);

    // check for empty command
    if (line[0] == '\0') {
        printf("CMS: Command cannot be empty.\n");
        return;
    }

    // if theres spaces in front of the input, auto move pointer to first non-space character
    char *p = line;
    // skip leading spaces
    while (isspace((unsigned char)*p)) p++;

    // check for "INSERT" command
    if (strncmp(p, "INSERT", 6) != 0) {
        printf("CMS: Command must start with \"INSERT\".\n");
        return;
    }
    // move pointer past "INSERT"
    p += 6;

    // search for fields and returns their positions
    char *idPos   = strstr(p, "ID=");
    char *namePos = strstr(p, "Name=");
    char *progPos = strstr(p, "Programme=");
    char *markPos = strstr(p, "Mark=");

    // check for missing fields
    if (!idPos || !namePos || !progPos || !markPos) {
        printf("CMS: Missing one or more fields. Please include ID=, Name=, Programme= and Mark=.\n");
        return;
    }

    // ensure fields are in correct order
    if (!(idPos < namePos && namePos < progPos && progPos < markPos)) {
        printf("CMS: Fields must appear in order: ID, Name, Programme, Mark.\n");
        return;
    }

    // parse and validate ID
    if (sscanf(idPos + 3, "%d", &id) != 1) {
        printf("CMS: Invalid ID. Please enter a valid integer.\n");
        return;
    }
    
    // make sure ID in valid range
    if (id < 2000000 || id > 9999999) {
        printf("CMS: Invalid ID. It must be exactly 7 digits.\n");
        return;
    }

    // check for duplicate ID
    if (findRecordByID(id) != -1) {
        printf("CMS: The record with ID=%d already exists.\n", id);
        return;
    }

    // extract Name
    {   
        // Points to the character after "Name=" (Skip 5 characters)
        char *nameStart = namePos + 5;

        // find how many letters to copy before "Programme="
        size_t len = (size_t)(progPos - nameStart);
        
        // Prevent buffer overflow
        if (len >= sizeof(name)) len = sizeof(name) - 1;
        // copy len characters from nameStart to name array
        memcpy(name, nameStart, len);
        // putting fullstop so C knows the name ends here
        name[len] = '\0';
        trim(name);
    }

    if (name[0] == '\0') {
        printf("CMS: Name cannot be empty.\n");
        return;
    }
    // check name contains only alphabets and spaces
    if (!isAlphaOnly(name)) {
        printf("CMS: Invalid name. Only alphabets and spaces are allowed.\n");
        return;
    }
    if (strlen(name) > 20) {
        printf("CMS: Name cannot exceed 20 characters.\n");
        return;
    }

    // extract Programme
    {
        char *progStart = progPos + 10;
        size_t len = (size_t)(markPos - progStart);
        if (len >= sizeof(programme)) len = sizeof(programme) - 1;
        memcpy(programme, progStart, len);
        programme[len] = '\0';
        trim(programme);
    }

    if (programme[0] == '\0') {
        printf("CMS: Programme cannot be empty.\n");
        return;
    }
    if (!isAlphaOnly(programme)) {
        printf("CMS: Invalid programme. Only alphabets and spaces are allowed.\n");
        return;
    }
    if (strlen(programme) > 25) {
        printf("CMS: Programme cannot exceed 25 characters.\n");
        return;
    }

    // parse Mark
    if (sscanf(markPos + 5, "%f", &mark) != 1) {
        printf("CMS: Invalid mark. Please enter a number.\n");
        return;
    }
    if (mark < 0.0f || mark > 100.0f) {
        printf("CMS: Invalid mark. Mark should be between 0 and 100.\n");
        return;
    }

    // ensure capacity and insert
    if (!ensureCapacity(recordCount + 1)) {
        printf("CMS: Error - Unable to allocate memory.\n");
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


void queryRecord(char *input) {
    if (!isFileOpen) {
        printf("CMS: Please open the database first using OPEN command.\n");
        return;
    }
    
    int id = -1;
    char *token = strtok(input, " ");
    while (token != NULL) {
        if (strncmp(token, "ID=", 3) == 0) {
            id = atoi(token + 3);
            break;
        }
        token = strtok(NULL, " ");
    }
    
    if (id == -1) {
        printf("CMS: Invalid input. Please provide ID in format: QUERY ID=xxxxx\n");
        return;
    }
    
    if (!validateID(id)) return;
    
    int index = findIndexById(id);
    if (index == -1) {
        printf("CMS: The record with ID=%d does not exist.\n", id);
        return;
    }
    
    printf("CMS: The record with ID=%d is found in the data table.\n", id);
    printf("%-10s %-25s %-30s %-10s %-6s\n", "ID", "Name", "Programme", "Mark", "Grade");
    printf("----------------------------------------------------------------------------------------\n");
    printf("%-10d %-25s %-30s %-10.1f %-6s\n",
           student_records[index].ID,
           student_records[index].Name,
           student_records[index].Programme,
           student_records[index].Mark,
           student_records[index].Grade);
}

void updateRecord(char *input) {
    if (!isFileOpen) {
        printf("CMS: Please open the database first using OPEN command.\n");
        return;
    }
    
    int id = -1;
    char name[MAX_NAME] = "";
    char programme[MAX_PROGRAMME] = "";
    float mark = -1;
    int hasName = 0, hasProgramme = 0, hasMark = 0;
    
    char *token = strtok(input, " ");
    
    while (token != NULL) {
        if (strncmp(token, "ID=", 3) == 0) {
            id = atoi(token + 3);
        }
        else if (strncmp(token, "Mark=", 5) == 0) {
            mark = atof(token + 5);
            hasMark = 1;
        }
        else if (strncmp(token, "Name=", 5) == 0) {
            hasName = 1;
            char *nameStart = token + 5;
            strcpy(name, nameStart);
            token = strtok(NULL, " ");
            while (token != NULL && strncmp(token, "Programme=", 10) != 0 && strncmp(token, "Mark=", 5) != 0) {
                strcat(name, " ");
                strcat(name, token);
                token = strtok(NULL, " ");
            }
            continue;
        }
        else if (strncmp(token, "Programme=", 10) == 0) {
            hasProgramme = 1;
            char *progStart = token + 10;
            strcpy(programme, progStart);
            token = strtok(NULL, " ");
            while (token != NULL && strncmp(token, "Mark=", 5) != 0) {
                strcat(programme, " ");
                strcat(programme, token);
                token = strtok(NULL, " ");
            }
            continue;
        }
        token = strtok(NULL, " ");
    }
    
    if (id == -1) {
        printf("CMS: Invalid input. Please provide ID.\n");
        return;
    }
    
    if (!validateID(id)) return;
    
    int index = findIndexById(id);
    if (index == -1) {
        printf("CMS: The record with ID=%d does not exist.\n", id);
        return;
    }
    
    if (hasName) {
        if (!sanitizeName(name)) return;
        strcpy(student_records[index].Name, name);
    }
    
    if (hasProgramme) {
        if (!sanitizeProgramme(programme)) return;
        strcpy(student_records[index].Programme, programme);
    }
    
    if (hasMark) {
        if (!validateMark(mark)) return;
        student_records[index].Mark = mark;
        calculateGrade(mark, student_records[index].Grade);
    }
    
    printf("CMS: The record with ID=%d is successfully updated.\n", id);
}

void deleteRecord(char *input) {
    if (!isFileOpen) {
        printf("CMS: Please open the database first using OPEN command.\n");
        return;
    }
    
    int id = -1;
    char *token = strtok(input, " ");
    while (token != NULL) {
        if (strncmp(token, "ID=", 3) == 0) {
            id = atoi(token + 3);
            break;
        }
        token = strtok(NULL, " ");
    }
    
    if (id == -1) {
        printf("CMS: Invalid input. Please provide ID in format: DELETE ID=xxxxx\n");
        return;
    }
    
    if (!validateID(id)) return;
    
    int index = findIndexById(id);
    if (index == -1) {
        printf("CMS: The record with ID=%d does not exist.\n", id);
        return;
    }
    
    char confirm[10];
    printf("CMS: Are you sure you want to delete record with ID=%d? Type \"Y\" to Confirm or \"N\" to cancel.\n", id);
    printf("P4_6: ");
    
    if (!fgets(confirm, sizeof(confirm), stdin)) {
        printf("CMS: Error reading input. Deletion cancelled.\n");
        return;
    }
    
    trim(confirm);
    
    if (strcmp(confirm, "Y") == 0 || strcmp(confirm, "y") == 0) {
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
    
    float sum = 0, highest = student_records[0].Mark, lowest = student_records[0].Mark;
    int highIndex = 0, lowIndex = 0;
    
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

void calculateGrade(float mark, char *grade) {
    if (mark >= 85) strcpy(grade, "A");
    else if (mark >= 70) strcpy(grade, "B");
    else if (mark >= 60) strcpy(grade, "C");
    else if (mark >= 50) strcpy(grade, "D");
    else strcpy(grade, "F");
}

void trim(char *str) {
    if (!str) return;
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
    if (!str) return;
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

static void trim_newline(char *s) {
    if (!s) return;
    size_t n = strlen(s);
    if (n && (s[n-1] == '\n' || s[n-1] == '\r')) s[n-1] = '\0';
}

static int idExists(int id) {
    for (int i = 0; i < recordCount; i++) {
        if (student_records[i].ID == id) return 1;
    }
    return 0;
}

static int ensureCapacity(int want) {
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
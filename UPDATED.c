#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_RECORDS 100
#define MAX_NAME 50
#define MAX_PROGRAMME 50
#define FILENAME "P1_1-CMS.txt"

// Student record structure
typedef struct {
    int id;
    char name[MAX_NAME];
    char programme[MAX_PROGRAMME];
    float mark;
} Student;

// Global variables
Student records[MAX_RECORDS];
int recordCount = 0;
int isFileOpen = 0;

// Function prototypes
void printDeclaration();
void openDatabase();
void showAll();
void insertRecord();
void queryRecord();
void updateRecord();
void deleteRecord();
void saveDatabase();
void showSummary();
void sortByID(int ascending);
void sortByMark(int ascending);
int findRecordByID(int id);
void trim(char *str);
void toLowerCase(char *str);

int main() {
    char input[200];
    char command[50];
    
    printDeclaration();
    
    printf("\n=== Class Management System ===\n");
    printf("Available commands: OPEN, SHOW ALL, INSERT, QUERY, UPDATE, DELETE, SAVE, SHOW SUMMARY, EXIT\n");
    printf("Enhancement commands: SHOW ALL SORT BY ID ASC/DESC, SHOW ALL SORT BY MARK ASC/DESC\n\n");
    
    while (1) {
        printf("P1_1: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0; // Remove newline
        
        // Copy for processing
        strcpy(command, input);
        toLowerCase(command);
        
        if (strcmp(command, "exit") == 0) {
            printf("CMS: Exiting program. Goodbye!\n");
            break;
        }
        else if (strcmp(command, "open") == 0) {
            openDatabase();
        }
        else if (strcmp(command, "show all") == 0) {
            showAll();
        }
        else if (strncmp(command, "show all sort by id", 19) == 0) {
            if (strstr(command, "desc") != NULL) {
                sortByID(0);
            } else {
                sortByID(1);
            }
            showAll();
        }
        else if (strncmp(command, "show all sort by mark", 21) == 0) {
            if (strstr(command, "desc") != NULL) {
                sortByMark(0);
            } else {
                sortByMark(1);
            }
            showAll();
        }
        else if (strncmp(command, "insert", 6) == 0) {
            insertRecord();
        }
        else if (strncmp(command, "query", 5) == 0) {
            queryRecord();
        }
        else if (strncmp(command, "update", 6) == 0) {
            updateRecord();
        }
        else if (strncmp(command, "delete", 6) == 0) {
            deleteRecord();
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
    printf("Declared by: Group P1_1\n");
    printf("Team members:\n");
    printf("1. Member Name 1\n");
    printf("2. Member Name 2\n");
    printf("3. Member Name 3\n");
    printf("4. Member Name 4\n");
    printf("5. Member Name 5\n");
    printf("Date: November 25, 2025\n");
    printf("========================================\n\n");
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
    while (fscanf(file, "%d %49[^\t\n] %49[^\t\n] %f\n", 
                  &records[recordCount].id,
                  records[recordCount].name,
                  records[recordCount].programme,
                  &records[recordCount].mark) == 4) {
        recordCount++;
        if (recordCount >= MAX_RECORDS) {
            printf("CMS: Warning - Maximum record limit reached.\n");
            break;
        }
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
    printf("%-10s %-25s %-30s %-10s\n", "ID", "Name", "Programme", "Mark");
    printf("--------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < recordCount; i++) {
        printf("%-10d %-25s %-30s %-10.1f\n",
               records[i].id,
               records[i].name,
               records[i].programme,
               records[i].mark);
    }
}

void insertRecord() {
    if (!isFileOpen) {
        printf("CMS: Please open the database first using OPEN command.\n");
        return;
    }
    
    int id;
    char name[MAX_NAME];
    char programme[MAX_PROGRAMME];
    float mark;
    
    printf("CMS: Enter Student ID: ");
    scanf("%d", &id);
    getchar(); // Clear newline
    
    // Check if ID already exists
    if (findRecordByID(id) != -1) {
        printf("CMS: The record with ID=%d already exists.\n", id);
        return;
    }
    
    printf("CMS: Enter Name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;
    trim(name);
    
    printf("CMS: Enter Programme: ");
    fgets(programme, sizeof(programme), stdin);
    programme[strcspn(programme, "\n")] = 0;
    trim(programme);
    
    printf("CMS: Enter Mark: ");
    scanf("%f", &mark);
    getchar(); // Clear newline
    
    // Validate mark
    if (mark < 0 || mark > 100) {
        printf("CMS: Invalid mark. Mark should be between 0 and 100.\n");
        return;
    }
    
    // Add new record
    records[recordCount].id = id;
    strcpy(records[recordCount].name, name);
    strcpy(records[recordCount].programme, programme);
    records[recordCount].mark = mark;
    recordCount++;
    
    printf("CMS: A new record with ID=%d is successfully inserted.\n", id);
}

void queryRecord() {
    if (!isFileOpen) {
        printf("CMS: Please open the database first using OPEN command.\n");
        return;
    }
    
    int id;
    printf("CMS: Enter Student ID to query: ");
    scanf("%d", &id);
    getchar(); // Clear newline
    
    int index = findRecordByID(id);
    if (index == -1) {
        printf("CMS: The record with ID=%d does not exist.\n", id);
        return;
    }
    
    printf("CMS: The record with ID=%d is found in the data table.\n", id);
    printf("%-10s %-25s %-30s %-10s\n", "ID", "Name", "Programme", "Mark");
    printf("--------------------------------------------------------------------------------\n");
    printf("%-10d %-25s %-30s %-10.1f\n",
           records[index].id,
           records[index].name,
           records[index].programme,
           records[index].mark);
}

void updateRecord() {
    if (!isFileOpen) {
        printf("CMS: Please open the database first using OPEN command.\n");
        return;
    }
    
    int id;
    printf("CMS: Enter Student ID to update: ");
    scanf("%d", &id);
    getchar(); // Clear newline
    
    int index = findRecordByID(id);
    if (index == -1) {
        printf("CMS: The record with ID=%d does not exist.\n", id);
        return;
    }
    
    char choice[10];
    printf("CMS: What would you like to update? (Name/Programme/Mark/All): ");
    fgets(choice, sizeof(choice), stdin);
    choice[strcspn(choice, "\n")] = 0;
    toLowerCase(choice);
    
    if (strcmp(choice, "name") == 0 || strcmp(choice, "all") == 0) {
        printf("CMS: Enter new Name: ");
        fgets(records[index].name, sizeof(records[index].name), stdin);
        records[index].name[strcspn(records[index].name, "\n")] = 0;
        trim(records[index].name);
    }
    
    if (strcmp(choice, "programme") == 0 || strcmp(choice, "all") == 0) {
        printf("CMS: Enter new Programme: ");
        fgets(records[index].programme, sizeof(records[index].programme), stdin);
        records[index].programme[strcspn(records[index].programme, "\n")] = 0;
        trim(records[index].programme);
    }
    
    if (strcmp(choice, "mark") == 0 || strcmp(choice, "all") == 0) {
        printf("CMS: Enter new Mark: ");
        scanf("%f", &records[index].mark);
        getchar(); // Clear newline
        
        if (records[index].mark < 0 || records[index].mark > 100) {
            printf("CMS: Invalid mark. Update cancelled.\n");
            return;
        }
    }
    
    printf("CMS: The record with ID=%d is successfully updated.\n", id);
}

void deleteRecord() {
    if (!isFileOpen) {
        printf("CMS: Please open the database first using OPEN command.\n");
        return;
    }
    
    int id;
    printf("CMS: Enter Student ID to delete: ");
    scanf("%d", &id);
    getchar(); // Clear newline
    
    int index = findRecordByID(id);
    if (index == -1) {
        printf("CMS: The record with ID=%d does not exist.\n", id);
        return;
    }
    
    char confirm;
    printf("CMS: Are you sure you want to delete record with ID=%d? Type \"Y\" to Confirm or type \"N\" to cancel.\n", id);
    printf("P1_1: ");
    scanf("%c", &confirm);
    getchar(); // Clear newline
    
    if (confirm == 'Y' || confirm == 'y') {
        // Shift all records after the deleted one
        for (int i = index; i < recordCount - 1; i++) {
            records[i] = records[i + 1];
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
        fprintf(file, "%d\t%s\t%s\t%.1f\n",
                records[i].id,
                records[i].name,
                records[i].programme,
                records[i].mark);
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
    
    float sum = 0, highest = records[0].mark, lowest = records[0].mark;
    int highIndex = 0, lowIndex = 0;
    
    for (int i = 0; i < recordCount; i++) {
        sum += records[i].mark;
        if (records[i].mark > highest) {
            highest = records[i].mark;
            highIndex = i;
        }
        if (records[i].mark < lowest) {
            lowest = records[i].mark;
            lowIndex = i;
        }
    }
    
    float average = sum / recordCount;
    
    printf("\n=== Summary Statistics ===\n");
    printf("Total number of students: %d\n", recordCount);
    printf("Average mark: %.2f\n", average);
    printf("Highest mark: %.1f (Student: %s)\n", highest, records[highIndex].name);
    printf("Lowest mark: %.1f (Student: %s)\n", lowest, records[lowIndex].name);
    printf("==========================\n\n");
}

void sortByID(int ascending) {
    if (!isFileOpen || recordCount == 0) return;
    
    for (int i = 0; i < recordCount - 1; i++) {
        for (int j = 0; j < recordCount - i - 1; j++) {
            int swap = ascending ? 
                (records[j].id > records[j + 1].id) : 
                (records[j].id < records[j + 1].id);
            
            if (swap) {
                Student temp = records[j];
                records[j] = records[j + 1];
                records[j + 1] = temp;
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
                (records[j].mark > records[j + 1].mark) : 
                (records[j].mark < records[j + 1].mark);
            
            if (swap) {
                Student temp = records[j];
                records[j] = records[j + 1];
                records[j + 1] = temp;
            }
        }
    }
    
    printf("CMS: Records sorted by Mark (%s).\n", ascending ? "ascending" : "descending");
}

int findRecordByID(int id) {
    for (int i = 0; i < recordCount; i++) {
        if (records[i].id == id) {
            return i;
        }
    }
    return -1;
}

void trim(char *str) {
    char *start = str;
    char *end;
    
    // Trim leading spaces
    while (isspace((unsigned char)*start)) start++;
    
    // All spaces?
    if (*start == 0) {
        *str = 0;
        return;
    }
    
    // Trim trailing spaces
    end = start + strlen(start) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;
    
    // Write new null terminator
    *(end + 1) = 0;
    
    // Move trimmed string to start
    memmove(str, start, end - start + 2);
}

void toLowerCase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

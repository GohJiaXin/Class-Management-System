#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME 100
#define MAX_PROGRAMME 100
#define FILENAME "P4_6-CMS.txt"

//Functions that need to be modified
// 1. Insert function
// 2. Query function
// 3. Delete function

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
void insertRecord();
void queryRecord();
void updateRecord();
void deleteRecord();
void saveDatabase();
void showSummary();
void sortByID(int ascending);
void sortByMark(int ascending);
void trim(char *str);
void toLowerCase(char *str);
void calculateGrade(float mark, char *grade);

// Helper functions
static void trim_newline(char *s);
static int idExists(int id);
static int ensureCapacity(int want);
static int findIndexById(int id);

int main() {
    char input[200];
    char command[200];
    
    printDeclaration();
    
    printf("\n=== Class Management System ===\n");
    printf("Available commands: OPEN, SHOW ALL, INSERT, QUERY, UPDATE, DELETE, SAVE, SHOW SUMMARY, EXIT\n");
    printf("Enhancement commands: SHOW ALL SORT BY ID ASC/DESC, SHOW ALL SORT BY MARK ASC/DESC\n\n");
    
    while (1) {
        printf("P4_6: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        
        strcpy(command, input);
        toLowerCase(command);
        
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
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("CMS: Database file \"%s\" not found. Creating new database.\n", FILENAME);
        isFileOpen = 1;
        recordCount = 0;
        return;
    }
    
    recordCount = 0;
    char line[300];
    
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
            // Has grade field
            student_records[recordCount].ID = id;
            strcpy(student_records[recordCount].Name, name);
            strcpy(student_records[recordCount].Programme, programme);
            student_records[recordCount].Mark = mark;
            strcpy(student_records[recordCount].Grade, grade);
        } else if (items == 4) {
            // No grade field, calculate it
            student_records[recordCount].ID = id;
            strcpy(student_records[recordCount].Name, name);
            strcpy(student_records[recordCount].Programme, programme);
            student_records[recordCount].Mark = mark;
            calculateGrade(mark, student_records[recordCount].Grade);
        } else {
            continue; // Skip invalid lines
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
    getchar();
    
    if (idExists(id)) {
        printf("CMS: The record with ID=%d already exists.\n", id);
        return;
    }
    
    printf("CMS: Enter Name: ");
    fgets(name, sizeof(name), stdin);
    trim_newline(name);
    trim(name);
    
    printf("CMS: Enter Programme: ");
    fgets(programme, sizeof(programme), stdin);
    trim_newline(programme);
    trim(programme);
    
    printf("CMS: Enter Mark: ");
    scanf("%f", &mark);
    getchar();
    
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

void queryRecord() {
    if (!isFileOpen) {
        printf("CMS: Please open the database first using OPEN command.\n");
        return;
    }
    
    int id;
    printf("CMS: Enter Student ID to query: ");
    scanf("%d", &id);
    getchar();
    
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

void updateRecord() {
    if (!isFileOpen) {
        printf("CMS: Please open the database first using OPEN command.\n");
        return;
    }
    
    int id;
    printf("CMS: Enter Student ID to update: ");
    scanf("%d", &id);
    getchar();
    
    int index = findIndexById(id);
    if (index == -1) {
        printf("CMS: The record with ID=%d does not exist.\n", id);
        return;
    }
    
    char choice[10];
    printf("CMS: What would you like to update? (Name/Programme/Mark/All): ");
    fgets(choice, sizeof(choice), stdin);
    trim_newline(choice);
    toLowerCase(choice);
    
    if (strcmp(choice, "name") == 0 || strcmp(choice, "all") == 0) {
        printf("CMS: Enter new Name: ");
        fgets(student_records[index].Name, MAX_NAME, stdin);
        trim_newline(student_records[index].Name);
        trim(student_records[index].Name);
    }
    
    if (strcmp(choice, "programme") == 0 || strcmp(choice, "all") == 0) {
        printf("CMS: Enter new Programme: ");
        fgets(student_records[index].Programme, MAX_PROGRAMME, stdin);
        trim_newline(student_records[index].Programme);
        trim(student_records[index].Programme);
    }
    
    if (strcmp(choice, "mark") == 0 || strcmp(choice, "all") == 0) {
        printf("CMS: Enter new Mark: ");
        scanf("%f", &student_records[index].Mark);
        getchar();
        
        if (student_records[index].Mark < 0 || student_records[index].Mark > 100) {
            printf("CMS: Invalid mark. Update cancelled.\n");
            return;
        }
        calculateGrade(student_records[index].Mark, student_records[index].Grade);
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
    getchar();
    
    int index = findIndexById(id);
    if (index == -1) {
        printf("CMS: The record with ID=%d does not exist.\n", id);
        return;
    }
    
    char confirm;
    printf("CMS: Are you sure you want to delete record with ID=%d? Type \"Y\" to Confirm or type \"N\" to cancel.\n", id);
    printf("P4_6: ");
    scanf("%c", &confirm);
    getchar();
    
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
        str[i] = tolower(str[i]);
    }
}

// Helper function implementations
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME 100
#define MAX_PROGRAMME 100
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
void sortByID(int ascending);
void sortByMark(int ascending);
void trim(char *str);
void toLowerCase(char *str);
void calculateGrade(float mark, char *grade);

// NEW filtering functions
void filterByProgramme();
void filterByMarkRange();

// Helper functions
static void trim_newline(char *s);
static int idExists(int id);
static int ensureCapacity(int want);
static int findIndexById(int id);

int main(void) {
    char input[256];
    char command[256];

    printDeclaration();
    printf("CMS - type HELP for commands.\n");

    while (1) {
        printf("P4_6: ");
        if (!fgets(input, sizeof(input), stdin)) {
            break;  // EOF or error
        }
        trim_newline(input);

        // Make a lowercase copy for command matching (so commands are case-insensitive)
        strcpy(command, input);
        toLowerCase(command);

        if (strcmp(command, "exit") == 0) {
            printf("CMS: Exiting program. Goodbye!\n");
            free(student_records);
            return 0;
        }
        else if (strcmp(command, "help") == 0) {
            printf("Commands:\n");
            printf("OPEN\n");
            printf("SHOW ALL\n");
            printf("SHOW ALL SORT BY ID [DESC]\n");
            printf("SHOW ALL SORT BY MARK [DESC]\n");
            printf("SHOW ALL FILTER BY PROGRAMME\n");
            printf("SHOW ALL FILTER BY MARK\n");
            printf("INSERT ID=.. Name=.. Programme=.. Mark=..\n");
            printf("QUERY ID=..\n");
            printf("UPDATE ID=.. Name=.. | Programme=.. | Mark=..\n");
            printf("DELETE ID=..\n");
            printf("SAVE\n");
            printf("SHOW SUMMARY\n");
            printf("EXIT\n");
        }
        else if (strcmp(command, "open") == 0) {
            openDatabase();
        }
        else if (strcmp(command, "show all") == 0) {
            showAll();
        }
        else if (strncmp(command, "show all sort by id", 19) == 0) {
            if (strstr(command, "desc") != NULL) {
                sortByID(0);  // descending
            } else {
                sortByID(1);  // ascending
            }
            showAll();
        }
        else if (strncmp(command, "show all sort by mark", 21) == 0) {
            if (strstr(command, "desc") != NULL) {
                sortByMark(0);  // descending
            } else {
                sortByMark(1);  // ascending
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
            recordCount++;
        } else if (items == 4) {
            // No grade field, calculate it
            student_records[recordCount].ID = id;
            strcpy(student_records[recordCount].Name, name);
            strcpy(student_records[recordCount].Programme, programme);
            student_records[recordCount].Mark = mark;
            calculateGrade(mark, student_records[recordCount].Grade);
            recordCount++;
        } else {
            // Skip invalid lines
            continue;
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

    int id = -1;
    char name[MAX_NAME] = "";
    char programme[MAX_PROGRAMME] = "";
    float mark = -1.0f;

    // Work on a copy because strtok modifies the string
    char buffer[500];
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    // Parse the input: INSERT ID=2401234 Name=Michelle Lee Programme=Information Security Mark=73.2
    char *token = strtok(buffer, " ");
    while (token != NULL) {
        if (strncmp(token, "ID=", 3) == 0) {
            id = atoi(token + 3);
        }
        else if (strncmp(token, "Mark=", 5) == 0) {
            mark = (float)atof(token + 5);
        }
        else if (strncmp(token, "Name=", 5) == 0) {
            char *nameStart = token + 5;
            strcpy(name, nameStart);
            // Continue reading until we hit Programme= or Mark=
            token = strtok(NULL, " ");
            while (token != NULL &&
                   strncmp(token, "Programme=", 10) != 0 &&
                   strncmp(token, "Mark=", 5) != 0) {
                strcat(name, " ");
                strcat(name, token);
                token = strtok(NULL, " ");
            }
            continue;
        }
        else if (strncmp(token, "Programme=", 10) == 0) {
            char *progStart = token + 10;
            strcpy(programme, progStart);
            // Continue reading until we hit Mark=
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

void queryRecord(char *input) {
    if (!isFileOpen) {
        printf("CMS: Please open the database first using OPEN command.\n");
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
            // use original token to get the number
            id = atoi(token + 3);
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

    // Clear leftover input
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);

    char input[256];
    printf("P4_6: ");
    fflush(stdout);

    if (!fgets(input, sizeof(input), stdin)) {
        printf("CMS: Input error.\n");
        return;
    }

    input[strcspn(input, "\n")] = '\0';

    /* Convert whole input to uppercase for consistent parsing */
    char upperInput[256];
    for (int i = 0; input[i]; i++)
        upperInput[i] = toupper(input[i]);
    upperInput[strlen(input)] = '\0';

    /* Extract ID and field=value using a more flexible pattern */
    int id;
    char field[50], value[100];

    int parsed = sscanf(upperInput, "UPDATE ID=%d %49[^=]=%99[^\n]", &id, field, value);

    if (parsed != 3) {
        printf("CMS: Invalid UPDATE format.\n");
        return;
    }

    /* Convert field back to lowercase to compare */
    for (int i = 0; field[i]; i++)
        field[i] = tolower(field[i]);

    /* Restore original-case value (upperInput lost original spacing) */
    char *equalSign = strchr(input, '=');
    if (equalSign) {
        strncpy(value, equalSign + 1, sizeof(value) - 1);
        value[sizeof(value) - 1] = '\0';
    }

    /* ---- FIND RECORD ---- */
    int index = -1;
    for (int i = 0; i < recordCount; i++) {
        if (student_records[i].ID == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("CMS: The record with ID=%d does not exist.\n", id);
        return;
    }

    StudentRecords *rec = &student_records[index];

    /* ---- APPLY UPDATE ---- */
    if (strcmp(field, "name") == 0) {
        if (strlen(value) > 99) {
            printf("CMS: Name too long.\n");
            return;
        }
        strcpy(rec->Name, value);
    }
    else if (strcmp(field, "programme") == 0) {
        if (strlen(value) > 99) {
            printf("CMS: Programme too long.\n");
            return;
        }
        strcpy(rec->Programme, value);
    }
    else if (strcmp(field, "mark") == 0) {
        float newMark;
        if (sscanf(value, "%f", &newMark) != 1 || newMark < 0 || newMark > 100) {
            printf("CMS: Invalid mark. Must be between 0 and 100.\n");
            return;
        }
        rec->Mark = newMark;
    }
    else {
        printf("CMS: Unsupported field '%s'.\n", field);
        return;
    }

    /* ---- WRITE TO TEMP FILE (SAFER) ---- */
    FILE *fp = fopen("Sample-CMS.tmp", "w");
    if (!fp) {
        printf("CMS: File update failed.\n");
        return;
    }

    fprintf(fp, "ID\tName\tProgramme\tMark\n");
    for (int i = 0; i < recordCount; i++) {
        fprintf(
            fp, "%d\t%s\t%s\t%.2f\n",
            student_records[i].ID,
            student_records[i].Name,
            student_records[i].Programme,
            student_records[i].Mark
        );
    }
    fclose(fp);

    // Replace old file
    remove("Sample-CMS.txt");
    rename("Sample-CMS.tmp", "Sample-CMS.txt");

    printf("CMS: The record with ID=%d is successfully updated.\n", id);
}

void deleteRecord(char *input) {
    if (!isFileOpen) {
        printf("CMS: Please open the database first using OPEN command.\n");
        return;
    }

    int id = -1;

    // Work on a copy because strtok modifies the string
    char buffer[256];
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    // Parse: DELETE ID=2401234
    char *token = strtok(buffer, " ");
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

// NEW: Filter by Programme
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
            if (strcmp(student_records[i].Programme, student_records[j].Programme) == 0) {
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
    printf("CMS: Please type the programme exactly as shown above:\n");
    printf("P4_6: ");
    if (!fgets(selected, sizeof(selected), stdin)) {
        printf("CMS: Input error.\n");
        return;
    }
    trim_newline(selected);

    if (strlen(selected) == 0) {
        printf("CMS: Invalid input. Programme name cannot be empty.\n");
        return;
    }

    // Validate that the programme exists (case-insensitive)
    int exists = 0;
    for (int i = 0; i < recordCount; i++) {
        char progLower[MAX_PROGRAMME];
        char inputLower[MAX_PROGRAMME];

        strcpy(progLower, student_records[i].Programme);
        strcpy(inputLower, selected);

        toLowerCase(progLower);
        toLowerCase(inputLower);

        if (strcmp(progLower, inputLower) == 0) {
            exists = 1;
            break;
        }
    }

    if (!exists) {
        printf("CMS: Invalid programme. Please choose a programme from the list. Exiting Filter..\n");
        return;
    }

    // Print filtered table
    printf("CMS: Records for programme \"%s\":\n", selected);
    printf("%-10s %-25s %-30s %-10s\n", "ID", "Name", "Programme", "Mark");
    int found = 0;
    for (int i = 0; i < recordCount; i++) {
        char progLower[MAX_PROGRAMME];
        char inputLower[MAX_PROGRAMME];

        strcpy(progLower, student_records[i].Programme);
        strcpy(inputLower, selected);

        toLowerCase(progLower);
        toLowerCase(inputLower);

        if (strcmp(progLower, inputLower) == 0) {
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

// NEW: Filter by Mark Range
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

    // Strictly enforce "XX - XX" style (spaces around '-'), but allow extra spaces
    if (sscanf(input, "%f - %f", &low, &high) != 2) {
        printf("CMS: Invalid input format. Please use: XX - XX (e.g. 50 - 80).\n");
        return;
    }

    if (low < 0 || high > 100 || low > high) {
        printf("CMS: Invalid range. Marks must be between 0 and 100 and lower <= upper.\n");
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

// Helper function implementations

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

/*

!!!!!!!!!!!Important Information!!!!!!!!

========CORE FUNCTIONS===========
ZhiHao          void ShowAllRecords(void)       COMPLETED
ZhiHao          void InsertNewRecord(void)      COMPLETED
Jason           void Query(void)                COMPLETED
Chef Anushka    void UpdateRecord(void)         COMPLETED
Zhi Hao         void DeleteRecord(void)         COMPLETED
Zhi Hao         void Save(void)                 COMPLETED
Jason           void Exit(void)                 COMPLETED

========ENHANCEMENT FEATURES============
Jia Xin         Data cleaning and validation    COMPLETED
Jason           Summary statistics              COMPLETED 
Zhi Hao         Grade Distribution              COMPLETED - merged into summary statistics
Anushka         Attendance and grading system   COMPLETED - Not in use
JiaXin          Sorting Function                COMPLETED
                
========ADDITIONAL FEATURES============
JiaXin SearchByNameandProgramme                     COMPLETED
JiaXin SearchByMarksRange                           COMPLETED

========Remarks/Meeting Agenda============
**** UPDATED ****
test for bugs/edge cases and create new test cases - do note down in the report ******IMPORTANT******* 1-2 per person
ensure consistent formatting across all functions
ensure ENTIRE CORE/ENHANCEMENT features is working VERY VERY VERY IMPORTANT
possible to add validation for user to check if user has saved before exiting
can work on merged SORT/FILTER function if have extra time but not important


**** ERRORS/TROUBLESHOOT **** (Please note down if fixed thanks, DONT REMOVE as we can use it as test case)
1. Anytime when user enters an input, system should allow the user to retry before returning to menu, possible to 
    make it such that user can leave blank to return to menu( is this needed? need your opinions on this - jason )
2. When inserting new record, currently inputting name does not validate if the user input ONLY has whitespace in the input for name and programme
3. When inserting new record, inputing name and programme has no character limitations. need to set such that it wont go beyond the system formatting.
4. same thing as 3 but for Update Record 
5. Edit Sorting function such that it shows whether it is sorted numerically or alphabetically (SOLVED by Jason)
6. Filtering Function should not have name search as it is already covered by Query, and for programming, can consider to list the existing programmes in
    in the database then ask for input when user chooses to search by programme
7. Sorting Function does not straight away return invalid when choosing an invalid Category choice 
8. Filter Function No error returned when user enters for eg. 30 40 for minimum mark then 50 for maximum mark
9. When user using the Insert, Query, Delete and Update function, when user enter invalid value1 (eg. string instead of integer for ID),
    system should allow user to retry instead of returning to main menu immediately (solved)
10. In IsAlpha function, string without any alphabets but only spaces is considered valid, 
    fixed by adding a flag to check if there is at least one alphabet (solved)


***** OUTDATED ****
User Interface not needed as per project requirements but can be considered as a unique feature? - to consult prof (NOT APPROVED)
Interactive Prompt need to elaborate in what sense - to be discussed
Proper Response formatting - to be discussed
Unique/Extra Features - to be disccused
- Attendance and grading system (abit redundant need to look into this again)
- Advanced search & visualizations (NOT APPROVED)
- Smart Analytics Suite (NOT APPROVED)
- User Interface (NOT APPROVED)
Add new code to "bring back to menu"? as current code will constantly bring up menu after choosing option
Create Test Cases as per project requirements
summary statistics and grade distribution can merge


========Timeline==========
check telegram for updated
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Prototypes */
int option(void);
void readFile(void);
void ShowAllRecords(void);
int CheckRecord(void);
void InsertNewRecord(void);
void Query(void);
void UpdateRecord(void);
void DeleteRecord(void);
void Save(void);
void SummaryStats(void);
void Sorting(void);
void sortByID(int order);
void sortByName(int order);
void sortByProgramme(int order);
void sortByMarks(int order);
void Filtering(void);
//Additional feature: Grade Distribution
//Data structure for students records


//Data structure for students records
typedef struct {
    int ID;
    char Name[100];
    char Programme[100];
    float Mark;
    int Attendance;      // Added to match AttendanceAndGrading use
    char Grade[3];       // Added to hold grades like "A", "B", etc.
} StudentRecords;

//Global variables to share data between functions
StudentRecords *student_records = NULL;
int recordCount = 0;
int capacity = 0;

// Removes trailing newline or carriage return from string
static void trim_newline(char *s) {
    // if string is NULL, exit immediately
    if (!s) return;
    //Get string length
    size_t n = strlen(s);
    // Check if string is not empty AND last character is newline (\n) or carriage return (\r)
    if (n && (s[n-1] == '\n' || s[n-1] == '\r')) s[n-1] = '\0';
}

// Checks if a student ID already exists in the records
static int idExists(int id) {
    for (int i = 0; i < recordCount; i++) {
        if (student_records[i].ID == id) return 1;
    }
    return 0;
}

// Ensures there is enough capacity in the student_records array
static int ensureCapacity(int want) {
    
    if (capacity >= want) return 1;                     // If have enough space, return success
    
    int newCap = (capacity > 0) ? capacity : 16;        // Otherwise, increase capacity
    
    while (newCap < want) newCap *= 2;                  // Double capacity until enough

    StudentRecords *tmp = realloc(student_records, newCap * sizeof(*tmp)); // Reallocate memory
    // Error handling
    if (!tmp) {
        perror("realloc failed");
        return 0;
    }
    student_records = tmp;
    capacity = newCap;
    return 1;
}

// searches for a student by ID and returns the index, or -1 if not found
static int findIndexById(int id) {
    for (int i = 0; i < recordCount; i++) {
        if (student_records[i].ID == id) return i;
    }
    return -1;
}

int ReturnMainMenu(void)
{
    char choice;
    printf("Return to main menu? (Y/N): ");
    fflush(stdout);  // make sure the prompt prints immediately

    if (scanf(" %c", &choice) != 1) {   // notice the space before %c to skip newlines
        printf("Invalid input.\n");
        while ((choice = getchar()) != '\n' && choice != EOF);
        return 0;   // stay here if input fails
    }

    while ((choice = getchar()) != '\n' && choice != EOF);  // clear once

    // convert to uppercase so it works with 'y' or 'Y'
    choice = toupper(choice);

    if (choice == 'Y') {
        return 1;   // go back to main menu
    } else if (choice == 'N') {
        printf("Goodbye!\n");
        exit(0);    // end program
    } else {
        printf("Invalid choice. Returning to main menu by default.\n");
        return 1;
    }
}

int main(void)
{
    int choice;

    for (;;) {
        printf("\n**********************************************************\n");
        printf("\tWelcome to Class Management System\n");
        printf("**********************************************************\n\n");

        choice = option();

        if (choice == 11) {              // Exit chosen: no ReturnMainMenu prompt
            printf("Goodbye!\n");
            break;
        }

        switch (choice) {
            case 1:  readFile(); break;
            case 2:  ShowAllRecords(); break;
            case 3:  InsertNewRecord(); break;
            case 4:  Query(); break;
            case 5:  UpdateRecord(); break;
            case 6:  DeleteRecord(); break;
            case 7:  Save(); break;
            case 8:  Sorting(); break;
            case 9:  Filtering(); break;
            case 10: SummaryStats(); break;
            default:
                // option() should already print an error; just continue
                continue;
        }

        printf("\n");

        // Only ask to return after a non-exit action
        if (!ReturnMainMenu()) {          
            printf("Goodbye!\n");
            break;
        }
    }

    if (student_records) {
        free(student_records);
        student_records = NULL;
    }
    return 0;
}

int option(void)
{
    int choice;

    printf("Please choose from the following options:\n");
    printf("1.  Read File\n");
    printf("2.  Show All Records\n");
    printf("3.  Insert New Record\n");
    printf("4.  Query\n");
    printf("5.  Update Record\n");
    printf("6.  Delete Record\n");
    printf("7.  Save\n");
    printf("8.  Sorting\n");
    printf("9.  Filtering\n");
    printf("10. Summary Stats\n");
    printf("11. Exit\n\n");
    printf("Your choice: ");

    if (scanf("%d", &choice) != 1) {
        //Clear input buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        printf("Invalid input.\n");
        return -1;
    }
    
    // Clear input buffer after successful read
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    return choice;
}

// Check if there is records
int CheckRecord(void)
{
    if (recordCount == 0) {
        printf("No records available. Please load a file first.\n");
        return 0;   // tell caller to stop
    }
    return 1;       // OK to proceed
}

void readFile(void) 
{
    FILE *pFile = fopen("Sample-CMS.txt", "r");
    if (!pFile) {
        perror("Could not open the file");
        return;
    }

    // Free previous data if any
    if (student_records != NULL) {
        free(student_records);
        student_records = NULL;
        recordCount = 0;
        capacity = 0;
    }

    capacity = 100;
    //Calculate total bytes needed
    student_records = malloc(capacity * sizeof(StudentRecords));
    if (!student_records) {
        perror("Memory allocation failed");
        fclose(pFile);
        return;
    }

    char line[256];
    int dataStarted = 0;

    while (fgets(line, sizeof(line), pFile)) {
        line[strcspn(line, "\n")] = 0; // Remove newline

        // Detect and skip header or empty lines
        if (strlen(line) == 0) continue;
        if (strstr(line, "ID") && strstr(line, "Name")) {
            dataStarted = 1;
            continue;
        }
        if (!dataStarted) continue;

        // Expand storage if needed
        if (recordCount >= capacity) {
            capacity *= 2;
            StudentRecords *temp = realloc(student_records, capacity * sizeof(StudentRecords));
            if (!temp) {
                printf("Memory reallocation failed\n");
                break;
            }
            student_records = temp;
        }

        int id;
        char name[100];
        char programme[100];
        float mark;

        // Try parsing as tab-separated
        int parsed = sscanf(line, "%d\t%99[^\t]\t%99[^\t]\t%f", 
                            &id, name, programme, &mark);

        // Fallback if spaces were used instead of tabs
        if (parsed != 4) {
            parsed = sscanf(line, "%d %99[^\t] %99[^\t] %f", 
                            &id, name, programme, &mark);
        }

        if (parsed == 4) {
            student_records[recordCount].ID = id;
            strncpy(student_records[recordCount].Name, name, sizeof(student_records[recordCount].Name));
            strncpy(student_records[recordCount].Programme, programme, sizeof(student_records[recordCount].Programme));
            student_records[recordCount].Mark = mark;

            recordCount++;
        } else {
            printf("Skipping invalid line: %s (parsed=%d)\n", line, parsed);
        }
    }

    fclose(pFile);
    printf("Total records loaded: %d\n", recordCount);
}

void ShowAllRecords(void)
{ 
    if (!CheckRecord()) return;

    //Display the results in pretty table format
    printf("\n%-10s %-20s %-27s %s\n", "ID", "Name", "Programme", "Mark");
    printf("------------------------------------------------------------------\n");
    
    for (int i = 0; i < recordCount; i++) {
        printf("%-10d %-20s %-27s %.2f\n", 
               student_records[i].ID, 
               student_records[i].Name, 
               student_records[i].Programme, 
               student_records[i].Mark);
    }
    printf("Total records: %d\n", recordCount);
    //Bottom Border
    printf("==================================================================\n");
    printf("Total records loaded: %d\n", recordCount);
}

// helper: check if string contains only alphabets and spaces
int isAlphaOnly(const char *str) {
    int hasLetter = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (isalpha((unsigned char)str[i])) {
            hasLetter = 1;
        } else if (str[i] != ' ') {
            return 0;  // Invalid character
        }
    }
    
    return hasLetter;  // Must have at least one letter
}

void InsertNewRecord(void)
{
    if (!CheckRecord()) return;

    int id;
    char buf[256];

    if (!ensureCapacity(recordCount + 1)) {
        printf("Memory allocation error. Aborting insert.\n");
        return;
    }

    /* ===== 1) GET VALID ID ===== */
    while (1) {
        printf("Insert ID (7 digits): ");
        fflush(stdout);

        if (!fgets(buf, sizeof(buf), stdin)) {
            printf("Input error.\n");
            return;   // real I/O error – okay to bail
        }

        if (sscanf(buf, "%d", &id) != 1) {
            printf("Invalid ID. Please enter a valid integer.\n");
            continue;  // ask again
        }

        if (id < 1000000 || id > 9999999) {
            printf("Invalid ID. It must be exactly 7 digits.\n");
            continue;  // ask again
        }

        if (idExists(id)) {
            printf("The record with ID=%d already exists. Please enter another ID.\n", id);
            continue;   
        }

        break; // valid ID
    }

    /* ===== 2) GET VALID NAME ===== */
    char name[100];
    while (1) {
        printf("Name: ");
        fflush(stdout);

        if (!fgets(name, sizeof(name), stdin)) {
            printf("Input error.\n");
            return;
        }
        trim_newline(name);

        if (name[0] == '\0') {
            printf("Name cannot be empty.\n");
            continue;
        }

        if (!isAlphaOnly(name)) {
            printf("Invalid name. Only alphabets and spaces are allowed.\n");
            continue;
        }

        break; // valid name
    }

    /* ===== 3) GET VALID PROGRAMME ===== */
    char programme[100];
    while (1) {
        printf("Programme: ");
        fflush(stdout);

        if (!fgets(programme, sizeof(programme), stdin)) {
            printf("Input error.\n");
            return;
        }
        trim_newline(programme);

        if (programme[0] == '\0') {
            printf("Programme cannot be empty.\n");
            continue;
        }

        if (!isAlphaOnly(programme)) {
            printf("Invalid programme. Only alphabets and spaces are allowed.\n");
            continue;
        }

        break; // valid programme
    }

    /* ===== 4) GET VALID MARK ===== */
    float mark;
    while (1) {
        printf("Mark: ");
        fflush(stdout);

        if (!fgets(buf, sizeof(buf), stdin)) {
            printf("Input error.\n");
            return;
        }

        if (sscanf(buf, "%f", &mark) != 1) {
            printf("Invalid mark. Please enter a number.\n");
            continue;
        }

        if (mark < 1.0f || mark > 100.0f) {
            printf("Invalid mark. Please enter a value between 1.0 and 100.0.\n");
            continue;
        }

        break; // valid mark
    }

    /* ===== 5) APPEND TO FILE ===== */
    FILE *fp = fopen("Sample-CMS.txt", "a");
    if (!fp) {
        perror("Could not open Sample-CMS.txt for appending");
        return;
    }

    if (fseek(fp, -1, SEEK_END) == 0) {
        int last = fgetc(fp);
        if (last != '\n') {
            fputc('\n', fp);
        }
    } else {
        fseek(fp, 0, SEEK_END);
    }

    if (fprintf(fp, "%d\t%s\t%s\t%.2f\n", id, name, programme, mark) < 0) {
        perror("Write failed");
        fclose(fp);
        return;
    }
    fclose(fp);

    /* ===== 6) ADD TO MEMORY ===== */
    student_records[recordCount].ID = id;

    strncpy(student_records[recordCount].Name, name,
            sizeof(student_records[recordCount].Name) - 1);
    student_records[recordCount].Name[sizeof(student_records[recordCount].Name) - 1] = '\0';

    strncpy(student_records[recordCount].Programme, programme,
            sizeof(student_records[recordCount].Programme) - 1);
    student_records[recordCount].Programme[sizeof(student_records[recordCount].Programme) - 1] = '\0';

    student_records[recordCount].Mark = mark;
    student_records[recordCount].Attendance = 0;
    student_records[recordCount].Grade[0] = '\0';

    recordCount++;

    printf("Record inserted: ID=%d, Name=\"%s\", Programme=\"%s\", Mark=%.2f\n",
           id, name, programme, mark);
}


void Query(void)
{
    if (!CheckRecord()) return;

    int id;
    char buf[256];
    int found = -1;

    while (1)
    {
        printf("Enter the student ID to query: ");
        fflush(stdout);

        if (!fgets(buf, sizeof(buf), stdin)) {
            printf("Input error.\n");
            return;
        }

        if (sscanf(buf, "%d", &id) != 1) {
            printf("Invalid ID. Please enter an integer.\n");
            continue; // ask again
        }

        if (id < 1000000 || id > 9999999) {
            printf("Invalid ID. It must be exactly 7 digits.\n");
            continue; // ask again
        }

        // Search for ID in student_records
        found = -1;
        for (int i = 0; i < recordCount; i++) {
            if (student_records[i].ID == id) {
                found = i;
                break;
            }
        }

        if (found == -1) {
            printf("CMS: The record with ID=%d does not exist. Please try again.\n", id);
            continue; // loop back to ask again
        }

        // If we reach here, we found a valid record → break out of loop
        break;
    }

    //Display the results in pretty table format
    printf("\nCMS: The record with ID= %d is found in the data table.\n", id);
    printf("------------------------------------------------------------------");
    printf("\n%-10s %-20s %-27s %s\n", "ID", "Name", "Programme", "Mark");
    printf("------------------------------------------------------------------\n");
    printf("%-10d %-20s %-27s %.2f\n", 
        student_records[found].ID, 
        student_records[found].Name, 
        student_records[found].Programme, 
        student_records[found].Mark);
    //Bottom Border
    printf("------------------------------------------------------------------\n");
}

// Update Record
void UpdateRecord(void)
{
    if (!CheckRecord()) return;

    int id, found = -1;
    char buf[256];

    /* ===== 1) Ask for a valid, existing ID ===== */
    while (1) {
        printf("\nEnter the student ID to update: ");
        fflush(stdout);

        if (!fgets(buf, sizeof(buf), stdin)) {
            printf("Input error.\n");
            return;
        }

        if (sscanf(buf, "%d", &id) != 1) {
            printf("Invalid ID. Please enter an integer.\n");
            continue;
        }

        if (id < 1000000 || id > 9999999) {
            printf("Invalid ID. It must be exactly 7 digits.\n");
            continue;
        }

        found = -1;
        for (int i = 0; i < recordCount; i++) {
            if (student_records[i].ID == id) {
                found = i;
                break;
            }
        }
        if (found == -1) {
            printf("CMS: The record with ID=%d does not exist. Please try again.\n", id);
            continue;   // ask for ID again
        }
        break; // valid & existing
    }

    StudentRecords *rec = &student_records[found];

    printf("Record found:\n");
    printf("ID: %d\nName: %s\nProgramme: %s\nMark: %.2f\n",
           rec->ID, rec->Name, rec->Programme, rec->Mark);

    /* ===== 2) Name (blank = keep) ===== */
    while (1) {
        char input[100];
        printf("\nEnter new name (leave blank to keep current): ");
        fflush(stdout);
        if (!fgets(input, sizeof(input), stdin)) {
            printf("Input error.\n");
            return;
        }
        input[strcspn(input, "\n")] = '\0';

        if (input[0] == '\0') break;                  // keep current
        if (!isAlphaOnly(input)) {
            printf("Invalid name. Only alphabets and spaces are allowed.\n");
            continue;
        }
        strncpy(rec->Name, input, sizeof(rec->Name) - 1);
        rec->Name[sizeof(rec->Name) - 1] = '\0';
        break;
    }

    /* ===== 3) Programme (blank = keep) ===== */
    while (1) {
        char input[100];
        printf("Enter new programme (leave blank to keep current): ");
        fflush(stdout);
        if (!fgets(input, sizeof(input), stdin)) {
            printf("Input error.\n");
            return;
        }
        input[strcspn(input, "\n")] = '\0';

        if (input[0] == '\0') break;                  // keep current
        if (!isAlphaOnly(input)) {
            printf("Invalid programme. Only alphabets and spaces are allowed.\n");
            continue;
        }
        strncpy(rec->Programme, input, sizeof(rec->Programme) - 1);
        rec->Programme[sizeof(rec->Programme) - 1] = '\0';
        break;
    }

    /* ===== 4) Mark (-1 = keep) ===== */
    while (1) {
        float mark;
        printf("Enter new mark (1.0~100.0, or -1 to keep current): ");
        fflush(stdout);

        if (!fgets(buf, sizeof(buf), stdin)) {
            printf("Input error.\n");
            return;
        }
        if (sscanf(buf, "%f", &mark) != 1) {
            printf("Invalid mark. Please enter a number.\n");
            continue;
        }
        if (mark == -1.0f) {
            break;                     // keep current
        }
        if (mark < 1.0f || mark > 100.0f) {
            printf("Invalid mark. Please enter a value between 1.0 and 100.0, or -1 to keep.\n");
            continue;
        }
        rec->Mark = mark;
        break;
    }

    printf("CMS: The record with ID=%d is successfully updated.\n", rec->ID);
}
void DeleteRecord(void)
{
    if (!CheckRecord()) return;

    int id, index;
    char buf[256];

    /* ===== 1) Get valid ID ===== */
    while (1) {
        printf("Delete ID (7 digits): ");
        fflush(stdout);

        if (!fgets(buf, sizeof(buf), stdin)) { 
            printf("Input error.\n"); 
            return; 
        }

        if (sscanf(buf, "%d", &id) != 1) { 
            printf("Invalid ID. Please enter an integer.\n"); 
            continue; 
        }

        if (id < 1000000 || id > 9999999) {
            printf("Invalid ID. It must be exactly 7 digits.\n");
            continue;
        }

        index = findIndexById(id);
        if (index < 0) {
            printf("The record with ID=%d does not exist. Please try again.\n", id);
            continue;
        }

        break; // valid & exists
    }

    /* ===== 2) Remove from in-memory array ===== */
    if (index < recordCount - 1) {
        memmove(&student_records[index],
                &student_records[index + 1],
                (size_t)(recordCount - index - 1) * sizeof(*student_records));
    }
    recordCount--;

    /* ===== 3) Safely update file ===== */
    const char *final_path = "Sample-CMS.txt";
    const char *tmp_path   = "Sample-CMS.tmp";

    FILE *fp = fopen(tmp_path, "wb");   // write binary avoids newline issues on Windows
    if (!fp) {
        perror("Could not open temporary file for writing");
        printf("In-memory record deleted, but file not updated. You may need to Save manually.\n");
        return;
    }

    // write header
    if (fprintf(fp, "ID\tName\tProgramme\tMark\n") < 0) {
        perror("Write failed (header)");
        fclose(fp);
        remove(tmp_path);
        return;
    }

    // write all remaining records
    for (int i = 0; i < recordCount; i++) {
        if (fprintf(fp, "%d\t%s\t%s\t%.2f\n",
                    student_records[i].ID,
                    student_records[i].Name,
                    student_records[i].Programme,
                    student_records[i].Mark) < 0) {
            perror("Write failed (row)");
            fclose(fp);
            remove(tmp_path);
            return;
        }
    }

    // close safely
    if (fclose(fp) != 0) {
        perror("Close failed for temporary file");
        remove(tmp_path);
        return;
    }

    // replace original file
    if (remove(final_path) != 0) {
        // it's fine if the file doesn’t exist — just warn
        perror("Warning: could not remove old file");
    }

    if (rename(tmp_path, final_path) != 0) {
        perror("Failed to replace the original file");
        printf("File update not completed. Kept '%s' with the latest data.\n", tmp_path);
        return;
    }

    printf("The record with ID=%d is successfully deleted. Remaining: %d\n", id, recordCount);
}



void Save(void)
{
    if (!CheckRecord()) return;

    FILE *fp = fopen("Sample-CMS.txt", "w");
    if (!fp) {
        perror("Could not open Sample-CMS.txt for writing");
        return;
    }

    // Check if there's enough space to write
    if (fprintf(fp, "ID\tName\tProgramme\tMark\n") < 0) {
        perror("Write failed (header)");
        fclose(fp);
        return;
    }

    // Write all records to file
    for (int i = 0; i < recordCount; i++) {
        if (fprintf(fp, "%d\t%s\t%s\t%.2f\n",
                    student_records[i].ID,
                    student_records[i].Name,
                    student_records[i].Programme,
                    student_records[i].Mark) < 0) {
            perror("Write failed (row)");
            fclose(fp);
            return;
        }
    }

    if (fclose(fp) != 0) {
        perror("Close failed for Sample-CMS.txt");
        return;
    }
    printf("All records successfully saved to Sample-CMS.txt\n");
}



/*Extra Features*/

/* Attendance and grading */
void AttendanceAndGrading(void)
{
    if (!CheckRecord()) return;CheckRecord();
    printf("\n--- Attendance and Grading System ---\n");

    for (int i = 0; i < recordCount; i++) {
        printf("\nUpdating %s (%d)\n", student_records[i].Name, student_records[i].ID);
        printf("Enter attendance percentage (0–100): ");
        int att;
        if (scanf("%d", &att) != 1 || att < 0 || att > 100) {
            printf("Invalid input, skipping.\n");
            while (getchar() != '\n');
            continue;
        }
        student_records[i].Attendance = att;

        float mark = student_records[i].Mark;
        char grade[3];

        if (mark >= 85 && att >= 75) strcpy(grade, "A");
        else if (mark >= 70 && att >= 70) strcpy(grade, "B");
        else if (mark >= 55 && att >= 60) strcpy(grade, "C");
        else if (mark >= 40 && att >= 50) strcpy(grade, "D");
        else strcpy(grade, "F");

        strcpy(student_records[i].Grade, grade);
    }

    printf("\nCMS: Attendance and grading update complete.\n");
}


/* Summary Statistics*/
void SummaryStats(void)
{
    if (!CheckRecord()) return;

    float sum = 0.0f;
    int highestIndex = 0, lowestIndex = 0;
    int gradeA = 0, gradeB = 0, gradeC = 0, gradeD = 0, gradeF = 0;

    for (int i = 0; i < recordCount; i++) {
        float mark = student_records[i].Mark;
        sum += mark;

        if (mark > student_records[highestIndex].Mark) highestIndex = i;
        if (mark < student_records[lowestIndex].Mark) lowestIndex = i;

        if (mark >= 80.0 && mark <= 100.0) gradeA++;
        else if (mark >= 70.0 && mark < 80.0) gradeB++;
        else if (mark >= 60.0 && mark < 70.0) gradeC++;
        else if (mark >= 50.0 && mark < 60.0) gradeD++;
        else if (mark >= 0.0 && mark < 50.0) gradeF++;
    }

    float average = sum / recordCount;
    float gradeApercent = (float)gradeA / recordCount * 100;
    float gradeBpercent = (float)gradeB / recordCount * 100;
    float gradeCpercent = (float)gradeC / recordCount * 100;
    float gradeDpercent = (float)gradeD / recordCount * 100;
    float gradeFpercent = (float)gradeF / recordCount * 100;

    // Summary Output
    printf("\n --- Summary Statistics ---\n");
    printf("Total number of Students = %d\n", recordCount);
    printf("Average Mark = %.2f\n", average);
    printf("Highest mark: %.2f by %s\n", student_records[highestIndex].Mark, student_records[highestIndex].Name);
    printf("Lowest mark: %.2f by %s\n", student_records[lowestIndex].Mark, student_records[lowestIndex].Name);

    // Grade Distribution Output
    printf("\n --- Grade Distribution ---\n");
    printf("==================================================\n");
    printf("%-8s %-12s %-10s %s\n", "Grade", "Range", "Count", "Percentage");
    printf("--------------------------------------------------\n");
    printf("%-8s %-12s %-10d %.1f%%\n", "A", "80-100", gradeA, gradeApercent);
    printf("%-8s %-12s %-10d %.1f%%\n", "B", "70-79", gradeB, gradeBpercent);
    printf("%-8s %-12s %-10d %.1f%%\n", "C", "60-69", gradeC, gradeCpercent);
    printf("%-8s %-12s %-10d %.1f%%\n", "D", "50-59", gradeD, gradeDpercent);
    printf("%-8s %-12s %-10d %.1f%%\n", "F", "0-49", gradeF, gradeFpercent);
    printf("--------------------------------------------------\n");
    printf("%-8s %-12s %-10d %.1f%%\n", "Total", "", recordCount, 100.0);
    printf("==================================================\n");
}



void Sorting(void)
{

    // Check if there are records
    if (!CheckRecord()) return;

    int sort_category;
    int order;

    do {
        printf("\nPlease choose one category to sort by:\n");
        printf("1. Sort Numerically by ID\n");
        printf("2. Sort Alphabetically by Name\n");
        printf("3. Sort Alphabetically by Programme\n");
        printf("4. Sort Numerically by Marks\n");
        printf("5. Exit Sorting\n\n");
        printf("Enter your choice: ");

        if (scanf("%d", &sort_category) != 1) {
            printf("Invalid input.\n");
            while (getchar() != '\n');
            continue;
        }

        if (sort_category == 5) {
            printf("Returning to main menu...\n");
            break;
        }

        printf("\nSelect order:\n1. Ascending\n2. Descending\nEnter your choice: ");
        if (scanf("%d", &order) != 1 || (order != 1 && order != 2)) {
            printf("Invalid order choice.\n");
            while (getchar() != '\n');
            continue;
        }

        // Clear buffer
        while (getchar() != '\n');

        // Call appropriate sort function
        switch (sort_category) {
            case 1: sortByID(order); break;
            case 2: sortByName(order); break;
            case 3: sortByProgramme(order); break;
            case 4: sortByMarks(order); break;
            default:
                printf("Invalid category choice.\n");
                break;
        }

       //Display the results in pretty table format
    printf("\nSorted Results:\n");
    printf("\n%-10s %-20s %-27s %s\n", "ID", "Name", "Programme", "Mark");
    printf("===============================================================\n");
    
    for (int i = 0; i < recordCount; i++) {
        printf("%-10d %-20s %-27s %.2f\n", 
               student_records[i].ID, 
               student_records[i].Name, 
               student_records[i].Programme, 
               student_records[i].Mark);
    }
    //printf("Total records: %d\n", recordCount);
    //Bottom Border
    printf("===============================================================\n");

    } while (sort_category != 5);

}
void sortByID(int order)
{
    for (int i = 0; i < recordCount - 1; i++) {
        for (int j = 0; j < recordCount - i - 1; j++) {
            int condition = (order == 1) ? 
                (student_records[j].ID > student_records[j + 1].ID) :
                (student_records[j].ID < student_records[j + 1].ID);
            if (condition) {
                StudentRecords temp = student_records[j];
                student_records[j] = student_records[j + 1];
                student_records[j + 1] = temp;
            }
        }
    }
}

void sortByName(int order)
{
    for (int i = 0; i < recordCount - 1; i++) {
        for (int j = 0; j < recordCount - i - 1; j++) {
            int cmp = strcmp(student_records[j].Name, student_records[j + 1].Name);
            int condition = (order == 1) ? (cmp > 0) : (cmp < 0);
            if (condition) {
                StudentRecords temp = student_records[j];
                student_records[j] = student_records[j + 1];
                student_records[j + 1] = temp;
            }
        }
    }
}

void sortByProgramme(int order)
{
    for (int i = 0; i < recordCount - 1; i++) {
        for (int j = 0; j < recordCount - i - 1; j++) {
            int cmp = strcmp(student_records[j].Programme, student_records[j + 1].Programme);
            int condition = (order == 1) ? (cmp > 0) : (cmp < 0);
            if (condition) {
                StudentRecords temp = student_records[j];
                student_records[j] = student_records[j + 1];
                student_records[j + 1] = temp;
            }
        }
    }
}

void sortByMarks(int order)
{
    for (int i = 0; i < recordCount - 1; i++) {
        for (int j = 0; j < recordCount - i - 1; j++) {
            int condition = (order == 1) ? 
                (student_records[j].Mark > student_records[j + 1].Mark) :
                (student_records[j].Mark < student_records[j + 1].Mark);
            if (condition) {
                StudentRecords temp = student_records[j];
                student_records[j] = student_records[j + 1];
                student_records[j + 1] = temp;
            }
        }
    }
}

void Filtering(void)
{
    if (!CheckRecord()) return;

    int choice;
    char buf[256];

    printf("\n--- Advanced Search Menu ---\n");
    printf("1. Search by Name or Programme\n");
    printf("2. Search by Marks Range\n");
    printf("Enter your choice: ");

    if (!fgets(buf, sizeof(buf), stdin) || sscanf(buf, "%d", &choice) != 1) {
        printf("Invalid input.\n");
        return;
    }

    switch (choice) {
        case 1: {
            char keyword[100];
            printf("Enter keyword to search by Name or Programme: ");
            fflush(stdout);
            if (!fgets(keyword, sizeof(keyword), stdin)) {
                printf("Input error.\n");
                return;
            }
            keyword[strcspn(keyword, "\n")] = 0;
            if (strlen(keyword) == 0) {
                printf("Keyword cannot be empty.\n");
                return;
            }
            for (int i = 0; keyword[i]; i++) keyword[i] = tolower(keyword[i]);

            printf("\nMatching Records:\n");
            printf("%-10s %-20s %-20s %s\n", "ID", "Name", "Programme", "Mark");
            printf("------------------------------------------------------------\n");

            int found = 0;
            for (int i = 0; i < recordCount; i++) {
                char nameLower[100], progLower[100];
                strncpy(nameLower, student_records[i].Name, sizeof(nameLower));
                nameLower[sizeof(nameLower) - 1] = '\0';
                strncpy(progLower, student_records[i].Programme, sizeof(progLower));
                progLower[sizeof(progLower) - 1] = '\0';
                for (int j = 0; nameLower[j]; j++) nameLower[j] = tolower(nameLower[j]);
                for (int j = 0; progLower[j]; j++) progLower[j] = tolower(progLower[j]);

                if (strstr(nameLower, keyword) || strstr(progLower, keyword)) {
                    printf("%-10d %-20s %-20s %.2f\n",
                           student_records[i].ID,
                           student_records[i].Name,
                           student_records[i].Programme,
                           student_records[i].Mark);
                    found++;
                }
            }

            if (found == 0) {
                printf("No matching records found for \"%s\".\n", keyword);
            } else {
                printf("------------------------------------------------------------\n");
                printf("Total matches: %d\n", found);
            }
            break;
        }

        case 2: {
            float minMark, maxMark;
            printf("Enter minimum mark: ");
            fflush(stdout);
            if (!fgets(buf, sizeof(buf), stdin) || sscanf(buf, "%f", &minMark) != 1 || minMark < 0 || minMark > 100) {
                printf("Invalid minimum mark.\n");
                return;
            }

            printf("Enter maximum mark: ");
            fflush(stdout);
            if (!fgets(buf, sizeof(buf), stdin) || sscanf(buf, "%f", &maxMark) != 1 || maxMark < 0 || maxMark > 100 || maxMark < minMark) {
                printf("Invalid maximum mark.\n");
                return;
            }

            printf("\nStudents with marks between %.2f and %.2f:\n", minMark, maxMark);
            printf("%-10s %-20s %-20s %s\n", "ID", "Name", "Programme", "Mark");
            printf("------------------------------------------------------------\n");

            int found = 0;
            for (int i = 0; i < recordCount; i++) {
                float mark = student_records[i].Mark;
                if (mark >= minMark && mark <= maxMark) {
                    printf("%-10d %-20s %-20s %.2f\n",
                           student_records[i].ID,
                           student_records[i].Name,
                           student_records[i].Programme,
                           mark);
                    found++;
                }
            }

            if (found == 0) {
                printf("No students found in the specified range.\n");
            } else {
                printf("------------------------------------------------------------\n");
                printf("Total matches: %d\n", found);
            }
            break;
        }

        default:
            printf("Invalid choice. Please select 1 or 2.\n");
    }
}



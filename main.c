/*

!!!!!!!!!!!Important Information!!!!!!!!

========CORE FUNCTIONS===========
ZhiHao          void ShowAllRecords(void)       COMPLETED
ZhiHao          void InsertNewRecord(void)      COMPLETED
Jason           void Query(void)                COMPLETED
Chef Anushka    void UpdateRecord(void)         COMPLETED
Li Xuan         void DeleteRecord(void) 
Li Xuan         void Save(void)
TBA             void Exit(void)                 COMPLETED

========ENHANCEMENT FEATURES============
Jia Xin         Data cleaning and validation    COMPLETED
Jason           Summary statistics 
Jia Xin         Attendance and grading system   COMPLETED
TBA             Sorting Function
                
========Remarks/Meeting Agenda============
User Interface not needed as per project requirements but can be considered as a unique feature? - to consult prof
Interactive Prompt need to elaborate in what sense - to be discussed
Proper Response formatting - to be discussed
Unique/Extra Features - to be disccused
- Attendance and grading system 
- Advanced search & visualizations
- Smart Analytics Suite
- Backup & Recovery
- User Interface
Add new code to "bring back to menu"? as current code will constantly bring up menu after choosing option


========Timeline==========
Complete Indiv Parts    Wednesday, 5 Nov 2359HRS 
Group Meeting           Thursday, 6 Nov ?? - ??
Final Submission        Tuesday, 25 Nov 2359HRS
Peer Evalutation        Wednesday, 26 Nov 2359HRS
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Prototypes */
int option(void);
void readFile(void);
void ShowAllRecords(void);
void InsertNewRecord(void);
void Query(void);
void UpdateRecord(void);
void DeleteRecord(void);
void Save(void);

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

int main(void)
{   
    int choice;
    
    do {
        printf("**********************************************************\n");
        printf("\tWelcome to Class Management System\n");
        printf("**********************************************************\n\n");

        choice = option();
        switch (choice) {   
            case 1: readFile(); break;
            case 2: ShowAllRecords(); break;
            case 3: InsertNewRecord(); break;
            case 4: Query(); break;
            case 5: UpdateRecord(); break;
            case 6: DeleteRecord(); break;
            case 7: Save(); break;
            case 8: printf("Goodbye!\n"); break;
            default: printf("Invalid choice: %d\n", choice); break;
        }
        printf("\n");
    } while (choice != 8);
    
    //Free memory before exiting
    if (student_records != NULL) {
        free(student_records);
        student_records = NULL;
    }
    
    return 0;
}

int option(void)
{
    int choice;

    printf("Please choose from the following options:\n");
    printf("1. Open File\n");
    printf("2. Show All Records\n");
    printf("3. Insert New Record\n");
    printf("4. Query\n");
    printf("5. Update Record\n");
    printf("6. Delete Record\n");
    printf("7. Save\n");
    printf("8. Exit\n\n");
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
    //Check if there is records 
    if (recordCount == 0) {
        printf("No records available. Please load a file first.\n");
        return;
    }
    //Display the results in pretty table format
    printf("\n%-10s %-20s %-20s %s\n", "ID", "Name", "Programme", "Mark");
    printf("------------------------------------------------------------\n");
    
    for (int i = 0; i < recordCount; i++) {
        printf("%-10d %-20s %-20s %.2f\n", 
               student_records[i].ID, 
               student_records[i].Name, 
               student_records[i].Programme, 
               student_records[i].Mark);
    }
    printf("Total records: %d\n", recordCount);
    //Bottom Border
    printf("===============================================================\n");
    printf("Total records loaded: %d\n", recordCount);
}


void InsertNewRecord(void)  { /* TODO */ }



void Query(void)          
{
    //Check if there is records 
    if (recordCount == 0) {
        printf("No records available. Please load a file first.\n");
        return;
    }
    

    // ID Input & Validation & ID Match Student Record 
    int id;
    printf("Enter the student ID to query: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid input.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    int found = -1;
    for (int i = 0; i < recordCount; i++) {
        if (student_records[i].ID == id) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        printf("CMS: The record with ID=%d does not exist.\n", id);
        return;
    }



    //Display the results in pretty table format
    printf("\nCMS: The record with ID= %d is found in the data table.\n", id);
    printf("------------------------------------------------------------");
    printf("\n%-10s %-20s %-20s %s\n", "ID", "Name", "Programme", "Mark");
    printf("------------------------------------------------------------\n");
    printf("%-10d %-20s %-20s %.2f\n", 
        student_records[found].ID, 
        student_records[found].Name, 
        student_records[found].Programme, 
        student_records[found].Mark);
    //Bottom Border
    printf("------------------------------------------------------------\n");
    



}






/* Update record */
void UpdateRecord(void)
{
    if (recordCount == 0) {
        printf("No records available. Please load a file first.\n");
        return;
    }

    int id;
    printf("Enter the student ID to update: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid input.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    int found = -1;
    for (int i = 0; i < recordCount; i++) {
        if (student_records[i].ID == id) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        printf("CMS: The record with ID=%d does not exist.\n", id);
        return;
    }

    StudentRecords *rec = &student_records[found];
    printf("Record found:\n");
    printf("ID: %d\nName: %s\nProgramme: %s\nMark: %.2f\n",
           rec->ID, rec->Name, rec->Programme, rec->Mark);

    char input[100];
    printf("Enter new name (leave blank to keep current): ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    if (strlen(input) > 0)
        strncpy(rec->Name, input, sizeof(rec->Name));

    printf("Enter new programme (leave blank to keep current): ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    if (strlen(input) > 0)
        strncpy(rec->Programme, input, sizeof(rec->Programme));

    printf("Enter new mark (-1 to keep current): ");
    float mark;
    if (scanf("%f", &mark) == 1 && mark >= 0.0f && mark <= 100.0f)
        rec->Mark = mark;
    else if (mark != -1)
        printf("Invalid mark input. Keeping old mark.\n");

    while (getchar() != '\n');
    printf("CMS: The record with ID=%d is successfully updated.\n", rec->ID);
}
void DeleteRecord(void)     { /* TODO */ }
void Save(void)             { /* TODO */ }

//Missing out on the sorting -Enhancement features
//Missing out on the summary statistics - Enhancement features
//Missing on user interfaces
//Missing out interactive prompts 
//Missing out proper response formatting
//Features that we can consider doing 
//1. Attendance and grading system 
//2. Advanced search & visualizations
//3. Smart Analytics Suite
//4. Backup & Recovery

/*Extra Features*/
/* Attendance and grading */
void AttendanceAndGrading(void)
{
    if (recordCount == 0) {
        printf("No records available.\n");
        return;
    }

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



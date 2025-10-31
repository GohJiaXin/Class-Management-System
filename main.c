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
            case 8: 
                printf("Goodbye!\n"); 
                break;
            default: 
                printf("Invalid choice: %d\n", choice);
                break;
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
    //Open file for reading 
    FILE *pFile = fopen("Sample-CMS.txt", "r");
    if (!pFile) {
        perror("Could not open the file");
        return;
    }
    
    //Free previous data if any
    if (student_records != NULL) {
        free(student_records);
        recordCount = 0;
        capacity = 0;
    }

    //Initial memory allocation
    capacity = 100;
    student_records = malloc(capacity * sizeof(StudentRecords));
    if (!student_records) {
        perror("Memory allocation failed");
        fclose(pFile);
        return;
    }

    //Buffer to hold each line of the file
    char line[255];

    //Skip header lines until we reach the actual data lines
    while (fgets(line, sizeof(line), pFile)) {
        if (line[0] >= '0' && line[0] <= '9') {
            // We've reached a line that starts with a digit (the ID)
            break;
        }
    }
    
    //Process this line and the following lines
    do {
        if (recordCount >= capacity) {
            //Resize if needed
            capacity *= 2;
            StudentRecords *temp = realloc(student_records, capacity * sizeof(StudentRecords));
            if (!temp) {
                printf("Memory reallocation failed\n");
                break;
            }
            student_records = temp;
        }
        
        int parsed = sscanf(line, "%d %99s %99s %f",
            &student_records[recordCount].ID,
            student_records[recordCount].Name,
            student_records[recordCount].Programme,
            &student_records[recordCount].Mark);

        // Make sure that there are 4 outputs
        if (parsed == 4) { 
            recordCount++;
        } else {
            printf("Skipping invalid line: %s", line);
        }
    } while (fgets(line, sizeof(line), pFile));

    //Close the file
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
void Query(void)            { /* TODO */ }
void UpdateRecord(void)     { /* TODO */ }//Missing on validation of user inputs
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

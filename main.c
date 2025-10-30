#include <stdio.h>
#include <stdlib.h> // for memory allocation 
/* Prototypes */
int  option(void);
void readFile(void);
void ShowAllRecords(void);
void InsertNewRecord(void);
void Query(void);
void UpdateRecord(void);
void DeleteRecord(void);
void Save(void);
//Data structure for students records
typedef struct{
    int ID;
    char Name[100];
    char Programme[100];
    float Mark;
}StudentRecords;

//Global variables to share data between functions
StudentRecords *student_records = NULL;
int recordCount = 0;
int capacity = 0;


int main(void)
{   
   

    printf("**********************************************************\n");
    printf("\tWelcome to Class Management System\n");
    printf("**********************************************************\n\n");

    int choice = option();   // matches prototype & definition
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
    }while (choice != 8);
    //Free memory before exiting
    if (student_records != NULL) {
        free(student_records);
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
        //Clear input buffer by reading and discarding characters from the input buffer 
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        // bad input (non-number)
        printf("Invalid input.\n");
        return -1;
    }
    return choice;
}

void readFile(void) 
{
    //Open file for reading 
    FILE *pFile = fopen("Sample-CMS.txt", "r");
    //If the file is NULL
    if (!pFile) {
        perror("Could not open the file");
        return; // <-- don't continue if open failed
    }
    //Free previous data if any
    if (student_records != NULL) {
        free(student_records);
        recordCount = 0;
        capacity = 0;
    }

    //Initial memory allocation
    capacity=100;
    StudentRecords *student_records =malloc(capacity* sizeof(StudentRecords));
    if(!students_records){
        perror("Memory allocation failed");
        fclose(pFile);
        return;
    }

    //Buffer to hold each line of the file
    char line[255];

    //2. Skip header lines until we reach the actual data lines
    while (fgets(line, sizeof(line),pFile)) {
        if (line[0] >= '0' && line[0] <= '9') {
            // We've reached a line that starts with a digit (the ID)
            break;
        }
    }
    //3. Process this line and the following lines
    do{
        if (recordCount >= capacity){
            //Resize if needed
            capacity *=2;
            StudentRecords *temp = realloc(student_records, capacity * sizeof(StudentRecords));
            if (!temp) {
                printf("Memory reallocation failed\n");
                break;
            }
            student_records = temp;
        }
        
        int parsed=sscanf(line,"%d %99s %99s %f",
            &student_records[recordCount].ID,
            student_records[recordCount].Name,
            student_records[recordCount].Programme,
            &student_records[recordCount].Mark);

        // Make sure that there is 4 outputs
        if (parsed==4){ 
        recordCount++;
        }else{
            printf("Skipping invalid line: %s\n", line)
        }
    }while (fgets(line, sizeof(line), pFile));

//Close the file
fclose(pFile);

//Display the results in pretty table format
//1. Print table header
printf("===============================================================\n");
printf("| %-8s | %-18s | %-22s | %-5s |\n", "ID", "Name", "Programme", "Mark");
printf("===============================================================\n");
//2. Print each student record
for (int i=0; i< recordCount; i++){
    printf("| %-8d | %-18s | %-22s | %-5.2f |\n",
        student_records[i].ID,
        student_records[i].Name,
        student_records[i].Programme,
        student_records[i].Mark);
}
//Bottom Border
printf("===============================================================\n");
printf("Total records loaded: %d\n", recordCount);


}



void ShowAllRecords(void)   { /* TODO */ } 
void InsertNewRecord(void)  { /* TODO */ }
void Query(void)            { /* TODO */ }
void UpdateRecord(void)     { /* TODO */ }//Missing on validation of user inputs
void DeleteRecord(void)     { /* TODO */ }
void Save(void)             { /* TODO */ }


//4. Backup & Recovery

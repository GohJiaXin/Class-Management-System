#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Prototypes */
int option(void);
void readFile(void);
void ShowAllRecords(void);
void InsertNewRecord(void);
void Query(void);
void UpdateRecord(void);
void DeleteRecord(void);
void Save(void);
void Sorting(void);
void sortByID(int order);
void sortByName(int order);
void sortByProgramme(int order);
void sortByMarks(int order);


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
            case 9: Sorting(); break;
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
    printf("1. Read File\n");
    printf("2. Show All Records\n");
    printf("3. Insert New Record\n");
    printf("4. Query\n");
    printf("5. Update Record\n");
    printf("6. Delete Record\n");
    printf("7. Save\n");
    printf("8. Exit\n");
    printf("9. Sorting\n\n");
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
    //Always reload data before showing all records
    readFile();

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
void UpdateRecord(void)
{
    // Load data first
    readFile();

    if (recordCount == 0) {
        printf("No records available. Please load a valid file first.\n");
        return;
    }

    int id;
    printf("Enter the student ID to update: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid input.\n");
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n'); // clear buffer

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

    //Validate Name
    printf("Enter new name (leave blank to keep current): ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    if (strlen(input) > 0) {
        int valid = 1;
        for (size_t i = 0; i < strlen(input); i++) {
            if (!isalpha(input[i]) && input[i] != ' ' && input[i] != '-') {
                valid = 0;
                break;
            }
        }
        if (valid)
            strncpy(rec->Name, input, sizeof(rec->Name));
        else
            printf("Invalid name. Keeping old name.\n");
    }

    // Validate Programme
    printf("Enter new programme (leave blank to keep current): ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    if (strlen(input) > 0) {
        int valid = 1;
        for (size_t i = 0; i < strlen(input); i++) {
            if (!isalnum(input[i]) && input[i] != ' ' && input[i] != '-') {
                valid = 0;
                break;
            }
        }
        if (valid)
            strncpy(rec->Programme, input, sizeof(rec->Programme));
        else
            printf("Invalid programme. Keeping old programme.\n");
    }

    // Validate Marks
    printf("Enter new mark (-1 to keep current): ");
    float mark;
    if (scanf("%f", &mark) == 1) {
        if (mark == -1) {
            // keep old mark
        } else if (mark >= 0.0f && mark <= 100.0f) {
            rec->Mark = mark;
        } else {
            printf("Invalid mark. Must be between 0 and 100. Keeping old mark.\n");
        }
    } else {
        printf("Invalid input. Keeping old mark.\n");
    }

    while (getchar() != '\n'); // flush leftover input

    
    FILE *pFile = fopen("Sample-CMS.txt", "w");
    if (!pFile) {
        perror("Error writing to file");
        return;
    }

    fprintf(pFile, "ID\tName\tProgramme\tMark\n");
    for (int i = 0; i < recordCount; i++) {
        fprintf(pFile, "%d\t%s\t%s\t%.2f\n",
                student_records[i].ID,
                student_records[i].Name,
                student_records[i].Programme,
                student_records[i].Mark);
    }
    fclose(pFile);

    printf("CMS: The record with ID=%d is successfully updated and saved.\n", rec->ID);
}
void Sorting(void)
{
    // Load data first
    readFile();

    // Check if there are records
    if (recordCount == 0) {
        printf("No records available. Please load a file first.\n");
        return;
    }

    int sort_category;
    int order;

    do {
        printf("\nPlease choose one category to sort by:\n");
        printf("1. Sort by ID\n");
        printf("2. Sort by Name\n");
        printf("3. Sort by Programme\n");
        printf("4. Sort by Marks\n");
        printf("5. Exit Sorting\n");
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

        printf("Select order:\n1. Ascending\n2. Descending\nEnter your choice: ");
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

        // Display sorted results
        /*printf("\nSorted Results:\n");
        printf("ID\tName\tProgramme\tMark\n");
        for (int i = 0; i < recordCount; i++) {
            printf("%d\t%s\t%s\t%.2f\n",
                   student_records[i].ID,
                   student_records[i].Name,
                   student_records[i].Programme,
                   student_records[i].Mark);
        }*/
       //Display the results in pretty table format
    printf("\nSorted Results:\n");
    printf("\n%-10s %-20s %-20s %s\n", "ID", "Name", "Programme", "Mark");
    printf("===============================================================\n");
    
    for (int i = 0; i < recordCount; i++) {
        printf("%-10d %-20s %-20s %.2f\n", 
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

void InsertNewRecord(void)  { /* TODO */ }
void Query(void)            { /* TODO */ }
void DeleteRecord(void)     { /* TODO */ }
void Save(void)             { /* TODO */ }


//Missing out on the summary statistics - Enhancement features
//Missing on user interfaces
//Missing out interactive prompts 
//Missing out proper response formatting
//Features that we can consider doing 
//1. Attendance and grading system 
//2. Advanced search & visualizations
//3. Smart Analytics Suite
//4. Backup & Recovery

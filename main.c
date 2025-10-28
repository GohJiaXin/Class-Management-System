#include <stdio.h>

/* Prototypes */
int  option(void);
void readFile(void);
void ShowAllRecords(void);
void InsertNewRecord(void);
void Query(void);
void UpdateRecord(void);
void DeleteRecord(void);
void Save(void);

int main(void)
{
    printf("**********************************************************\n");
    printf("\tWelcome to Class Management System\n");
    printf("**********************************************************\n");

    int choice = option();   // matches prototype & definition

    /* Example dispatch (fill in as you implement features) */
    switch (choice) {
        case 1: readFile(); break;
        case 2: ShowAllRecords(); break;
        case 3: InsertNewRecord(); break;
        case 4: Query(); break;
        case 5: UpdateRecord(); break;
        case 6: DeleteRecord(); break;
        case 7: Save(); break;
        default:
            printf("Invalid choice: %d\n", choice);
    }

    return 0;
}

int option(void)
{
    int choice;
    /* int min = 1, max = 7;  // unused; remove or use for validation */

    printf("Please choose from the following options:\n");
    printf("1. Open File\n");
    printf("2. Show All Records\n");
    printf("3. Insert New Record\n");
    printf("4. Query\n");
    printf("5. Update Record\n");
    printf("6. Delete Record\n");
    printf("7. Save\n");
    printf("Your choice: ");

    if (scanf("%d", &choice) != 1) {
        // bad input (non-number)
        printf("Invalid input.\n");
        return -1;
    }
    return choice;
}

void readFile(void)
{
    FILE *pFile = fopen("P1_1-CMS.txt", "r");
    if (!pFile) {
        perror("Could not open the file");
        return; // <-- don't continue if open failed
    }

    char buffer[255];
    while (fgets(buffer, sizeof buffer, pFile)) {
        fputs(buffer, stdout);
    }

    if (ferror(pFile)) perror("read error");
    fclose(pFile);
}

void ShowAllRecords(void)   { /* TODO */ }
void InsertNewRecord(void)  { /* TODO */ }
void Query(void)            { /* TODO */ }
void UpdateRecord(void)     { /* TODO */ }
void DeleteRecord(void)     { /* TODO */ }
void Save(void)             { /* TODO */ }

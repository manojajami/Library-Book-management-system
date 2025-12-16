#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BOOK_FILE "books.dat"
#define ISSUE_FILE "issues.dat"
#define ALLOWED_DAYS 7
#define FINE_PER_DAY 2

// ---------------- STRUCTURES ----------------

typedef struct {
    int id;
    char name[100];
    char author[100];
    int quantity;
} Book;

typedef struct {
    int book_id;
    char student_name[100];
    int roll_no;
    time_t issue_date;
} Issue;

// ---------------- UTILITY -------------------

void pauseScreen() {
    printf("\nPress Enter to continue...");
    getchar();
}

// ---------------- BOOK FUNCTIONS ------------

void addBook() {
    Book b;
    FILE *fp = fopen(BOOK_FILE, "ab");

    if (fp == NULL) {
        printf("\nUnable to open file!\n");
        return;
    }

    printf("\nEnter Book ID: ");
    scanf("%d", &b.id);
    getchar();

    printf("Enter Book Name: ");
    fgets(b.name, 100, stdin);
    b.name[strcspn(b.name, "\n")] = 0;

    printf("Enter Author Name: ");
    fgets(b.author, 100, stdin);
    b.author[strcspn(b.author, "\n")] = 0;

    printf("Enter Quantity: ");
    scanf("%d", &b.quantity);

    fwrite(&b, sizeof(Book), 1, fp);
    fclose(fp);

    printf("\nBook added successfully!\n");
}

void displayBooks() {
    FILE *fp = fopen(BOOK_FILE, "rb");
    Book b;

    if (fp == NULL) {
        printf("\nNo books available!\n");
        return;
    }

    printf("\n----- BOOK STOCK -----\n");
    while (fread(&b, sizeof(Book), 1, fp)) {
        printf("\nBook ID: %d\nName: %s\nAuthor: %s\nQuantity: %d\n",
               b.id, b.name, b.author, b.quantity);
    }
    fclose(fp);
}

void deleteBook() {
    int id, found = 0;
    printf("\nEnter Book ID to delete: ");
    scanf("%d", &id);

    FILE *fp = fopen(BOOK_FILE, "rb");
    FILE *temp = fopen("temp.dat", "wb");
    Book b;

    if (fp == NULL) {
        printf("\nNo books available!\n");
        return;
    }

    while (fread(&b, sizeof(Book), 1, fp)) {
        if (b.id == id) {
            found = 1;
            continue;
        }
        fwrite(&b, sizeof(Book), 1, temp);
    }

    fclose(fp);
    fclose(temp);
    remove(BOOK_FILE);
    rename("temp.dat", BOOK_FILE);

    if (found)
        printf("\nBook deleted successfully!\n");
    else
        printf("\nBook not found!\n");
}

// ---------------- ISSUE FUNCTIONS -----------

void issueBook() {
    Issue iss;
    Book b;
    int found = 0;

    printf("\nEnter Book ID to Issue: ");
    scanf("%d", &iss.book_id);
    getchar();

    FILE *fp = fopen(BOOK_FILE, "rb+");

    if (fp == NULL) {
        printf("\nNo books available!\n");
        return;
    }

    while (fread(&b, sizeof(Book), 1, fp)) {
        if (b.id == iss.book_id) {
            found = 1;
            if (b.quantity > 0) {
                b.quantity--;
                fseek(fp, -sizeof(Book), SEEK_CUR);
                fwrite(&b, sizeof(Book), 1, fp);
            } else {
                printf("\nBook out of stock!\n");
                fclose(fp);
                return;
            }
            break;
        }
    }
    fclose(fp);

    if (!found) {
        printf("\nBook ID not found!\n");
        return;
    }

    printf("Enter Student Name: ");
    fgets(iss.student_name, 100, stdin);
    iss.student_name[strcspn(iss.student_name, "\n")] = 0;

    printf("Enter Roll Number: ");
    scanf("%d", &iss.roll_no);

    iss.issue_date = time(NULL);

    FILE *fp2 = fopen(ISSUE_FILE, "ab");
    fwrite(&iss, sizeof(Issue), 1, fp2);
    fclose(fp2);

    printf("\nBook Issued Successfully!\n");
}

void returnBook() {
    char name[100];
    int roll, bookID, found = 0;
    time_t issueDate;

    getchar();
    printf("\nEnter Student Name: ");
    fgets(name, 100, stdin);
    name[strcspn(name, "\n")] = 0;

    printf("Enter Roll Number: ");
    scanf("%d", &roll);

    FILE *fp = fopen(ISSUE_FILE, "rb");
    FILE *temp = fopen("temp.dat", "wb");
    Issue iss;

    if (fp == NULL) {
        printf("\nNo issued records found!\n");
        return;
    }

    while (fread(&iss, sizeof(Issue), 1, fp)) {
        if (strcmp(iss.student_name, name) == 0 && iss.roll_no == roll) {
            found = 1;
            bookID = iss.book_id;
            issueDate = iss.issue_date;
            continue;
        }
        fwrite(&iss, sizeof(Issue), 1, temp);
    }

    fclose(fp);
    fclose(temp);
    remove(ISSUE_FILE);
    rename("temp.dat", ISSUE_FILE);

    if (!found) {
        printf("\nNo issued book found!\n");
        return;
    }

    FILE *fp2 = fopen(BOOK_FILE, "rb+");
    Book b;

    while (fread(&b, sizeof(Book), 1, fp2)) {
        if (b.id == bookID) {
            b.quantity++;
            fseek(fp2, -sizeof(Book), SEEK_CUR);
            fwrite(&b, sizeof(Book), 1, fp2);
            break;
        }
    }
    fclose(fp2);

    int days, fine;

    printf("\nAllowed Days: %d", ALLOWED_DAYS);
    printf("\nFine Per Day After Due Date: %d", FINE_PER_DAY);
    printf("\nEnter number of days completed after issue: ");
    scanf("%d", &days);

    if (days > ALLOWED_DAYS) {
        fine = (days - ALLOWED_DAYS) * FINE_PER_DAY;
        printf("\nLate by %d days", days - ALLOWED_DAYS);
    } else {
        fine = 0;
        printf("\nReturned within due date");
    }

    printf("\nFine Amount: rs%d\n", fine);
    printf("\nBook Returned Successfully!\n");
}

// ---------------- MAIN ----------------------

int main() {
    int choice;

    while (1) {
        printf("\n\n------ Library Management System ------");
        printf("\n1. Add New Book");
        printf("\n2. Delete Book");
        printf("\n3. Issue Book");
        printf("\n4. Return Book (Calculate Fine)");
        printf("\n5. Display Books");
        printf("\n6. Exit");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addBook(); break;
            case 2: deleteBook(); break;
            case 3: issueBook(); break;
            case 4: returnBook(); break;
            case 5: displayBooks(); break;
            case 6: exit(0);
            default: printf("\nInvalid Choice!\n");
        }

        getchar();
        pauseScreen();
    }
}

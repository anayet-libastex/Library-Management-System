#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Constants
#define MAX_USERS 100
#define MAX_BOOKS 100
#define MAX_BORROWED_BOOKS 10
#define BORROW_PERIOD 14 // Borrow period in days
#define FINE_RATE_PER_DAY 1 // Fine rate per day for overdue books


// User struct to represent users
typedef struct {
    char username[50];
    char password[50];
    char securityQuestion[100];
    char securityAnswer[50];
    int borrowedBooks[MAX_BORROWED_BOOKS];
    time_t dueDates[MAX_BORROWED_BOOKS];
    int borrowedCount;
} User;

// Book struct to represent books (Model Struct)
typedef struct {
    char title[100];
    char author[100];
    int id;
    int isAvailable;
} Book;

// Library struct to manage books (Controller Struct)
typedef struct {
    Book books[MAX_BOOKS];
    int count;
} Library;

// Function prototypes
void initLibrary(Library* library);
void addBook(Library* library, char* title, char* author, int id);
void displayBooks(Library* library);
void editBook(Library* library, int id, char* newTitle, char* newAuthor);
void deleteBook(Library* library, int id);
void checkOutBook(Library* library, int bookId);
void checkInBook(Library* library, int bookId);
void displayBorrowedBooks();
void checkDueDates();
void initUsers();
int registerUser();
int login();
void logout();
void resetPassword();
void recoverPassword();
int findUserByUsername(char* username);

// Global variables
User users[MAX_USERS];
int userCount = 0;
int loggedInUserIndex = -1;

// Initialize library
void initLibrary(Library* library) {
    library->count = 0;
}

// Add book to the library
void addBook(Library* library, char* title, char* author, int id) {
    // Check for duplicate book ID
    for (int i = 0; i < library->count; i++) {
        if (library->books[i].id == id) {
            printf("A book with ID %d already exists. Please choose a different ID.\n", id);
            return;
        }
    }

    // Check for duplicate book title and author
    for (int i = 0; i < library->count; i++) {
        if (strcmp(library->books[i].title, title) == 0 && strcmp(library->books[i].author, author) == 0) {
            printf("A book with the same title and author already exists. Please choose a different title or author.\n");
            return;
        }
    }

    if (library->count == MAX_BOOKS) {
        printf("Library is full. Cannot add more books.\n");
        return;
    }

    strcpy(library->books[library->count].title, title);
    strcpy(library->books[library->count].author, author);
    library->books[library->count].id = id;
    library->books[library->count].isAvailable = 1;
    library->count++;

}



// Display all books
void displayBooks(Library* library) {
    if (library->count == 0) {
        printf("No Books Found\n");
    } else {
        printf("Books available in the library:\n");
        for (int i = 0; i < library->count; i++) {
            if (library->books[i].isAvailable) {
                printf("Title: %s, Author: %s, ID: %d\n", library->books[i].title, library->books[i].author, library->books[i].id);
            }
        }
    }
}

// Edit a book
void editBook(Library* library, int id, char* newTitle, char* newAuthor) {
    for (int i = 0; i < library->count; i++) {
        if (library->books[i].id == id && library->books[i].isAvailable) {
            strcpy(library->books[i].title, newTitle);
            strcpy(library->books[i].author, newAuthor);
            printf("Book with ID %d has been edited successfully.\n", id);
            return;
        }
    }
    printf("Book with ID %d is not available.\n", id);
}


// Delete a book
void deleteBook(Library* library, int id) {
    int found = 0;

    for (int i = 0; i < library->count; i++) {
        if (library->books[i].id == id) {
            if (library->books[i].isAvailable) {
                library->books[i].isAvailable = 0;
                printf("Book with ID %d has been deleted successfully.\n", id);
            } else {
                printf("Book with ID %d is already marked as deleted.\n", id);
            }
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Book with ID %d does not exist.\n", id);
    }
}

// Check out a book
void checkOutBook(Library* library, int bookId) {
    if (loggedInUserIndex == -1) {
        printf("No user is currently logged in.\n");
        return;
    }

    User* user = &users[loggedInUserIndex];
    if (user->borrowedCount == MAX_BORROWED_BOOKS) {
        printf("You have reached the maximum number of borrowed books.\n");
        return;
    }

    for (int i = 0; i < library->count; i++) {
        if (library->books[i].id == bookId && library->books[i].isAvailable) {
            library->books[i].isAvailable = 0;
            user->borrowedBooks[user->borrowedCount] = bookId;
            time(&user->dueDates[user->borrowedCount]);
            user->dueDates[user->borrowedCount] += BORROW_PERIOD * 24 * 3600; // Add borrow period in seconds
            user->borrowedCount++;
            printf("Book with ID %d has been checked out successfully.\n", bookId);
            return;
        }
    }
    printf("Book with ID %d is not available.\n", bookId);
}

// Check in a book
void checkInBook(Library* library, int bookId) {
    if (loggedInUserIndex == -1) {
        printf("No user is currently logged in.\n");
        return;
    }

    User* user = &users[loggedInUserIndex];
    for (int i = 0; i < user->borrowedCount; i++) {
        if (user->borrowedBooks[i] == bookId) {
            for (int j = 0; j < library->count; j++) {
                if (library->books[j].id == bookId) {
                    library->books[j].isAvailable = 1;
                    for (int k = i; k < user->borrowedCount - 1; k++) {
                        user->borrowedBooks[k] = user->borrowedBooks[k + 1];
                        user->dueDates[k] = user->dueDates[k + 1];
                    }
                    user->borrowedCount--;
                    printf("Book with ID %d has been checked in successfully.\n", bookId);
                    return;
                }
            }
        }
    }
    printf("Book with ID %d is not borrowed by you.\n", bookId);
}

// Display borrowed books
void displayBorrowedBooks() {
    if (loggedInUserIndex == -1) {
        printf("No user is currently logged in.\n");
        return;
    }

    User* user = &users[loggedInUserIndex];
    if (user->borrowedCount == 0) {
        printf("You have not borrowed any books.\n");
    } else {
        printf("Books you have borrowed:\n");
        for (int i = 0; i < user->borrowedCount; i++) {
            printf("Book ID: %d, Due Date: %s", user->borrowedBooks[i], ctime(&user->dueDates[i]));
        }
    }
}

// Check due dates and overdue books
void checkDueDates() {
    if (loggedInUserIndex == -1) {
        printf("You must be logged in to check due dates.\n");
        return;
    }

    User* user = &users[loggedInUserIndex];
    time_t currentTime = time(NULL);  // Get current time
    int anyDueBooks = 0;  // Flag to check if there are any due books

    printf("\nChecking Due Dates for Borrowed Books:\n");
    printf("---------------------------------------\n");

    for (int i = 0; i < user->borrowedCount; i++) {
        int bookId = user->borrowedBooks[i];
        time_t dueDate = user->dueDates[i];
        double secondsLeft = difftime(dueDate, currentTime);
        int daysLeft = (int)(secondsLeft / (24 * 3600));  // Calculate days left

        if (daysLeft >= 0) {
            printf("Book ID %d is due in %d days on %s", bookId, daysLeft, ctime(&dueDate));
        } else {
            int overdueDays = -daysLeft;  // Calculate overdue days
            int fine = overdueDays * FINE_RATE_PER_DAY;  // Calculate fine
            printf("Book ID %d is overdue by %d days! Fine: %d units. It was due on %s", bookId, overdueDays, fine, ctime(&dueDate));
        }
        anyDueBooks = 1;
    }

    if (!anyDueBooks) {
        printf("You have not borrowed any books or all borrowed books are returned.\n");
    }
}

//calculate Fine
int calculateTotalFines() {
    if (loggedInUserIndex == -1) {
        printf("You must be logged in to calculate fines.\n");
        return 0;
    }

    User* user = &users[loggedInUserIndex];
    time_t currentTime = time(NULL);
    int totalFine = 0;

    for (int i = 0; i < user->borrowedCount; i++) {
        time_t dueDate = user->dueDates[i];
        double secondsLeft = difftime(dueDate, currentTime);
        int daysLeft = (int)(secondsLeft / (24 * 3600));

        if (daysLeft < 0) {  // Book is overdue
            int overdueDays = -daysLeft;
            totalFine += overdueDays * FINE_RATE_PER_DAY;
        }
    }

    return totalFine;
}


// Initialize users
void initUsers() {
    userCount = 0;
}

// Register a new user
int registerUser() {
    if (userCount == MAX_USERS) {
        printf("User registration is full.\n");
        return 0;
    }

    User newUser;
    printf("Enter username: ");
    scanf("%s", newUser.username);

    if (findUserByUsername(newUser.username) != -1) {
        printf("Username already exists. Please choose another username.\n");
        return 0;
    }

    printf("Enter password: ");
    scanf("%s", newUser.password);
    printf("Enter a security question: ");
    getchar(); // To consume the newline character left by scanf
    fgets(newUser.securityQuestion, sizeof(newUser.securityQuestion), stdin);
    newUser.securityQuestion[strcspn(newUser.securityQuestion, "\n")] = 0; // Remove trailing newline
    printf("Enter the answer to the security question: ");
    fgets(newUser.securityAnswer, sizeof(newUser.securityAnswer), stdin);
    newUser.securityAnswer[strcspn(newUser.securityAnswer, "\n")] = 0; // Remove trailing newline

    newUser.borrowedCount = 0;
    users[userCount++] = newUser;
    printf("User registered successfully!\n");
    return 1;
}

// Login
int login() {
    char username[50];
    char password[50];

    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    int userIndex = findUserByUsername(username);
    if (userIndex != -1 && strcmp(users[userIndex].password, password) == 0) {
        printf("Login successful!\n");
        loggedInUserIndex = userIndex;
        return 1;
    } else {
        printf("Invalid credentials. Try again.\n");
        return 0;
    }
}

// Logout
void logout() {
    printf("Logged out successfully.\n");
    loggedInUserIndex = -1;
}

// Reset password
void resetPassword() {
    if (loggedInUserIndex == -1) {
        printf("No user is currently logged in.\n");
        return;
    }

    char newPassword[50];
    printf("Enter new password: ");
    scanf("%s", newPassword);

    strcpy(users[loggedInUserIndex].password, newPassword);
    printf("Password reset successfully.\n");
}

// Recover password
void recoverPassword() {
    char username[50];
    printf("Enter username: ");
    scanf("%s", username);

    int userIndex = findUserByUsername(username);
    if (userIndex == -1) {
        printf("Username not found.\n");
        return;
    }

    char answer[50];
    printf("%s\n", users[userIndex].securityQuestion);
    printf("Enter the answer to the security question: ");
    getchar(); // To consume the newline character left by scanf
    fgets(answer, sizeof(answer), stdin);
    answer[strcspn(answer, "\n")] = 0; // Remove trailing newline

    if (strcmp(users[userIndex].securityAnswer, answer) == 0) {
        printf("Answer correct. Your password is: %s\n", users[userIndex].password);
    } else {
        printf("Incorrect answer. Cannot recover password.\n");
    }
}

// Find user by username
int findUserByUsername(char* username) {
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    Library library;
    initLibrary(&library);
    initUsers();

    int choice, bookId;
    char bookTitle[100], bookAuthor[100];
    int isLoggedIn = 0;

    // Main menu
    do {
        if (!isLoggedIn) {
            printf("\n****Library Management System****\n");
            printf("1. Register\n");
            printf("2. Login\n");
            printf("3. Recover Password\n");
            printf("4. Exit\n");
            printf("Enter your choice: ");
            scanf("%d", &choice);

            switch (choice) {
            case 1:
                registerUser();
                break;
            case 2:
                isLoggedIn = login();
                break;
            case 3:
                recoverPassword();
                break;
            case 4:
                printf("Exiting the program. Thank you!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
            }
        } else {
            printf("\n****Library Management System****\n");
            printf("1. Display all books\n");
            printf("2. Add a book\n");
            printf("3. Edit a book\n");
            printf("4. Delete a book\n");
            printf("5. Check out a book\n");
            printf("6. Check in a book\n");
            printf("7. Display borrowed books\n");
            printf("8. Check due dates\n");
            printf("9. Display total fines\n");
            printf("10. Reset password\n");
            printf("11. Logout\n");
            printf("Enter your choice: ");
            scanf("%d", &choice);

            switch (choice) {
            case 1:
                displayBooks(&library);
                break;
            case 2:
                printf("Enter Book Title: ");
                getchar(); // To consume the newline character left by scanf
                fgets(bookTitle, sizeof(bookTitle), stdin);
                bookTitle[strcspn(bookTitle, "\n")] = 0; // Remove trailing newline
                printf("Enter Book Author: ");
                fgets(bookAuthor, sizeof(bookAuthor), stdin);
                bookAuthor[strcspn(bookAuthor, "\n")] = 0; // Remove trailing newline
                printf("Enter Book ID: ");
                scanf("%d", &bookId);
                addBook(&library, bookTitle, bookAuthor, bookId);
                printf("Book added successfully.\n");
                break;
            case 3:
                printf("Enter the ID of the book you want to edit: ");
                scanf("%d", &bookId);
                printf("Enter the new title for the book: ");
                getchar(); // To consume the newline character left by scanf
                fgets(bookTitle, sizeof(bookTitle), stdin);
                bookTitle[strcspn(bookTitle, "\n")] = 0; // Remove trailing newline
                printf("Enter the new author for the book: ");
                fgets(bookAuthor, sizeof(bookAuthor), stdin);
                bookAuthor[strcspn(bookAuthor, "\n")] = 0; // Remove trailing newline
                editBook(&library, bookId, bookTitle, bookAuthor);
                break;

            case 4:
                printf("Enter the ID of the book you want to delete: ");
                scanf("%d", &bookId);
                deleteBook(&library, bookId);
                break;
            case 5:
                printf("Enter the ID of the book you want to check out: ");
                scanf("%d", &bookId);
                checkOutBook(&library, bookId);
                break;
            case 6:
                printf("Enter the ID of the book you want to check in: ");
                scanf("%d", &bookId);
                checkInBook(&library, bookId);
                break;
            case 7:
                displayBorrowedBooks();
                break;
            case 8:
                checkDueDates();
                break;
            case 9:
            {
                int totalFines = calculateTotalFines();
                printf("Your total fines amount to: %d BDT.\n", totalFines);
            }
                break;
            case 10:
                resetPassword();
                break;
            case 11:
                logout();
                isLoggedIn = 0;
                break;


            default:
                printf("Invalid choice. Please try again.\n");
                break;
            }
        }
    } while (choice != 12);

    return 0;
}




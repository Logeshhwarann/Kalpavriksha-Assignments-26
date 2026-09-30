#include <stdio.h>

struct User
{
    int id;
    char name[50];
    int age;
};

void createFile()
{
    FILE *file = fopen("users.txt", "a");

    if (file == NULL)
    {
        printf("Error creating file.\n");
        return;
    }

    fclose(file);
}

void addUser()
{
    FILE *file = fopen("users.txt", "a");
    struct User user;

    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    printf("Enter ID: ");
    scanf("%d", &user.id);

    printf("Enter Name: ");
    scanf("%s", user.name);

    printf("Enter Age: ");
    scanf("%d", &user.age);

    fprintf(file, "%d,%s,%d\n", user.id, user.name, user.age);

    fclose(file);

    printf("User added successfully.\n");
}

int main()
{
    int choice;

    createFile();

    while (1)
    {
        printf("\n===== User Management =====\n");
        printf("1. Add User\n");
        printf("2. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        if (choice == 1)
        {
            addUser();
        }
        else if (choice == 2)
        {
            break;
        }
        else
        {
            printf("Invalid choice.\n");
        }
    }

    return 0;
}
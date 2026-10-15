#include <stdio.h>
#include <ctype.h>

struct User
{
    int id;
    char name[50];
    int age;
};

void clearBuffer()
{
    int inputCharacter;

    while ((inputCharacter = getchar()) != '\n' &&inputCharacter != EOF)
    {
    }
}

int validName(char name[])
{
    int position = 0;

    while (name[position] != '\0')
    {
        if (!isalpha((unsigned char)name[position]))
            return 0;

        position++;
    }

    return 1;
}

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

    while (1)
    {
        printf("Enter ID: ");

        if (scanf("%d", &user.id) != 1)
        {
            printf("Invalid ID.\n");
            clearBuffer();
            continue;
        }

        clearBuffer();

        if (user.id <= 0)
        {
            printf("ID must be positive.\n");
            continue;
        }

        break;
    }

    while (1)
    {
        printf("Enter Name: ");
        scanf("%49s", user.name);
        clearBuffer();

        if (validName(user.name))
            break;

        printf("Invalid name. Enter letters only.\n");
    }

    while (1)
    {
        printf("Enter Age: ");

        if (scanf("%d", &user.age) != 1)
        {
            printf("Invalid age.\n");
            clearBuffer();
            continue;
        }

        clearBuffer();

        if (user.age >= 0)
            break;

        printf("Age cannot be negative.\n");
    }

    fprintf(file, "%d,%s,%d\n", user.id, user.name, user.age);

    fclose(file);

    printf("User added successfully.\n");
}

void readUsers()
{
    FILE *file = fopen("users.txt", "r");
    struct User user;
    int found = 0;

    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    printf("\n   User Records   \n");

    while (fscanf(file, "%d,%49[^,],%d",&user.id, user.name, &user.age) == 3)
    {
        printf("ID: %d | Name: %s | Age: %d\n",user.id, user.name, user.age);

        found = 1;
    }

    if (found == 0)
    {
        printf("No users found.\n");
    }

    fclose(file);
}

void updateUser()
{
    FILE *file = fopen("users.txt", "r");
    FILE *temp = fopen("temp.txt", "w");

    struct User user;
    int id;
    int found = 0;

    if (file == NULL || temp == NULL)
    {
        printf("Error opening file.\n");

        if (file != NULL)
            fclose(file);

        if (temp != NULL)
            fclose(temp);

        return;
    }

    printf("Enter ID to update: ");

    if (scanf("%d", &id) != 1)
    {
        printf("Invalid ID.\n");
        clearBuffer();
        fclose(file);
        fclose(temp);
        remove("temp.txt");
        return;
    }

    clearBuffer();

    while (fscanf(file, "%d,%49[^,],%d",&user.id, user.name, &user.age) == 3)
    {
        if (user.id == id)
        {
            found = 1;

            while (1)
            {
                printf("Enter new name: ");
                scanf("%49s", user.name);
                clearBuffer();

                if (validName(user.name))
                    break;

                printf("Invalid name. Enter letters only.\n");
            }

            while (1)
            {
                printf("Enter new age: ");

                if (scanf("%d", &user.age) != 1)
                {
                    printf("Invalid age.\n");
                    clearBuffer();
                    continue;
                }

                clearBuffer();

                if (user.age >= 0)
                    break;

                printf("Age cannot be negative.\n");
            }
        }

        fprintf(temp, "%d,%s,%d\n",user.id, user.name, user.age);
    }

    fclose(file);
    fclose(temp);

    if (found == 0)
    {
        printf("User with ID %d not found.\n", id);
        remove("temp.txt");
        return;
    }

    remove("users.txt");
    rename("temp.txt", "users.txt");

    printf("User updated successfully.\n");
}

void deleteUser()
{
    FILE *file = fopen("users.txt", "r");
    FILE *temp = fopen("temp.txt", "w");

    struct User user;
    int id;
    int found = 0;

    if (file == NULL || temp == NULL)
    {
        printf("Error opening file.\n");

        if (file != NULL)
            fclose(file);

        if (temp != NULL)
            fclose(temp);

        return;
    }

    printf("Enter ID to delete: ");

    if (scanf("%d", &id) != 1)
    {
        printf("Invalid ID.\n");
        clearBuffer();
        fclose(file);
        fclose(temp);
        remove("temp.txt");
        return;
    }

    clearBuffer();

    while (fscanf(file, "%d,%49[^,],%d",&user.id, user.name, &user.age) == 3)
    {
        if (user.id == id)
        {
            found = 1;
            continue;
        }

        fprintf(temp, "%d,%s,%d\n",user.id, user.name, user.age);
    }

    fclose(file);
    fclose(temp);

    if (found == 0)
    {
        printf("User with ID %d not found.\n", id);
        remove("temp.txt");
        return;
    }

    remove("users.txt");
    rename("temp.txt", "users.txt");

    printf("User deleted successfully.\n");
}

int main()
{
    int choice;

    createFile();

    while (1)
    {
        printf("\n   User Management   \n");
        printf("1. Add User\n");
        printf("2. Read Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");

        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input. Enter a number.\n");
            clearBuffer();
            continue;
        }

        clearBuffer();

        switch (choice)
        {
            case 1:
                addUser();
                break;

            case 2:
                readUsers();
                break;

            case 3:
                updateUser();
                break;

            case 4:
                deleteUser();
                break;

            case 5:
                printf("Program ended.\n");
                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }

    return 0;
}
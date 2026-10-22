#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#define MAX_EMPLOYEES 100
#define NAME_SIZE 100

#define CEO 1
#define DIRECTOR 2
#define MANAGER 3
#define TEAM_LEAD 4
#define SENIOR_DEVELOPER 5
#define JUNIOR_DEVELOPER 6

const char *designations[] = {
    "",
    "CEO",
    "Director",
    "Manager",
    "Team Lead",
    "Senior Developer",
    "Junior Developer"
};

struct Employee
{
    int id;
    char name[NAME_SIZE];
    int designation;

    struct Employee *manager;
    struct Employee *reports[MAX_EMPLOYEES];
    int reportCount;

    int active;
};

struct Employee employees[MAX_EMPLOYEES];

int readinteger(const char *message, int *value)
{
    char input[100];
    char *end;
    long number;

    printf("%s", message);

    if (fgets(input, sizeof(input), stdin) == NULL)
        return -1;

    if (strchr(input, '\n') == NULL && !feof(stdin))
    {
        int ch;

        while ((ch = getchar()) != '\n' && ch != EOF)
        {
        }

        printf("Input is too long.\n");
        return 0;
    }

    errno = 0;
    number = strtol(input, &end, 10);

    if (end == input || errno == ERANGE ||
        number < INT_MIN || number > INT_MAX)
    {
        printf("Invalid integer.\n");
        return 0;
    }

    while (isspace((unsigned char)*end))
        end++;

    if (*end != '\0')
    {
        printf("Invalid integer.\n");
        return 0;
    }

    *value = (int)number;
    return 1;
}

int readname(char name[])
{
    printf("Enter employee name: ");

    if (fgets(name, NAME_SIZE, stdin) == NULL)
        return 0;

    if (strchr(name, '\n') == NULL && !feof(stdin))
    {
        int ch;

        while ((ch = getchar()) != '\n' && ch != EOF)
        {
        }

        printf("Name is too long.\n");
        return 0;
    }

    name[strcspn(name, "\n")] = '\0';

    int hasLetter = 0;

    for (int i = 0; name[i] != '\0'; i++)
    {
        if (isalpha((unsigned char)name[i]))
            hasLetter = 1;
        else if (name[i] != ' ')
        {
            printf("Name must contain only letters and spaces.\n");
            return 0;
        }
    }

    if (!hasLetter)
    {
        printf("Name cannot be empty.\n");
        return 0;
    }

    return 1;
}

struct Employee *findEmployee(int id)
{
    for (int i = 0; i < MAX_EMPLOYEES; i++)
    {
        if (employees[i].active && employees[i].id == id)
            return &employees[i];
    }

    return NULL;
}

struct Employee *findFreeSlot(void)
{
    for (int i = 0; i < MAX_EMPLOYEES; i++)
    {
        if (!employees[i].active)
            return &employees[i];
    }

    return NULL;
}

int ceoexists(void)
{
    for (int i = 0; i < MAX_EMPLOYEES; i++)
    {
        if (employees[i].active &&
            employees[i].designation == CEO)
            return 1;
    }

    return 0;
}

int wouldcreatecycle(struct Employee *employee,
                     struct Employee *manager)
{
    while (manager != NULL)
    {
        if (manager == employee)
            return 1;

        manager = manager->manager;
    }

    return 0;
}

int addreport(struct Employee *manager,
              struct Employee *employee)
{
    if (manager == NULL ||
        manager->reportCount >= MAX_EMPLOYEES)
        return 0;

    manager->reports[manager->reportCount] = employee;
    manager->reportCount++;

    return 1;
}

void removereport(struct Employee *manager,
                  struct Employee *employee)
{
    if (manager == NULL)
        return;

    for (int i = 0; i < manager->reportCount; i++)
    {
        if (manager->reports[i] == employee)
        {
            for (int j = i; j < manager->reportCount - 1; j++)
                manager->reports[j] = manager->reports[j + 1];

            manager->reportCount--;
            return;
        }
    }
}

void addemployee(void)
{
    int id, managerId, choice;

    struct Employee *slot = findFreeSlot();

    if (slot == NULL)
    {
        printf("Employee limit reached.\n");
        return;
    }

    if (readinteger("Enter Employee ID: ", &id) != 1)
        return;

    if (id <= 0 || findEmployee(id) != NULL)
    {
        printf("ID must be positive and unique.\n");
        return;
    }

    char name[NAME_SIZE];

    if (!readname(name))
        return;

    printf("\nSelect Designation:\n");

    for (int i = 1; i <= 6; i++)
        printf("%d. %s\n", i, designations[i]);

    if (readinteger("Enter choice: ", &choice) != 1)
        return;

    if (choice < CEO || choice > JUNIOR_DEVELOPER)
    {
        printf("Invalid designation.\n");
        return;
    }

    if (choice == CEO && ceoexists())
    {
        printf("A CEO already exists.\n");
        return;
    }

    if (readinteger("Enter Manager ID (0 for no manager): ",
                    &managerId) != 1)
        return;

    if (managerId < 0 || managerId == id)
    {
        printf("Invalid manager ID.\n");
        return;
    }

    struct Employee *manager = NULL;

    if (choice == CEO && managerId != 0)
    {
        printf("CEO cannot have a manager.\n");
        return;
    }

    if (managerId != 0)
    {
        manager = findEmployee(managerId);

        if (manager == NULL)
        {
            printf("Manager not found.\n");
            return;
        }

        if (manager->designation >= choice)
        {
            printf("Manager must have a higher designation.\n");
            return;
        }

        if (wouldcreatecycle(slot, manager))
        {
            printf("Circular reporting is not allowed.\n");
            return;
        }

        if (manager->reportCount >= MAX_EMPLOYEES)
        {
            printf("Manager cannot accept more employees.\n");
            return;
        }
    }

    slot->id = id;
    strcpy(slot->name, name);
    slot->designation = choice;
    slot->manager = manager;
    slot->reportCount = 0;
    slot->active = 1;

    if (manager != NULL)
        addreport(manager, slot);

    printf("Employee added successfully.\n");
}

void removeemployee(void)
{
    int id;

    if (readinteger("Enter Employee ID to remove: ", &id) != 1)
        return;

    struct Employee *employee = findEmployee(id);

    if (employee == NULL)
    {
        printf("Employee not found.\n");
        return;
    }

    if (employee->designation == CEO)
    {
        printf("CEO cannot be removed.\n");
        return;
    }

    struct Employee *manager = employee->manager;

    removereport(manager, employee);

    for (int i = 0; i < employee->reportCount; i++)
    {
        struct Employee *child = employee->reports[i];

        child->manager = manager;

        if (manager != NULL)
            addreport(manager, child);
    }

    employee->reportCount = 0;
    employee->manager = NULL;
    employee->active = 0;

    printf("Employee removed successfully.\n");
}

void displayHierarchy(struct Employee *employee, int level)
{
    if (employee == NULL || !employee->active)
        return;

    for (int i = 0; i < level; i++)
        printf("    ");

    printf("%d - %s - %s\n",
           employee->id,
           employee->name,
           designations[employee->designation]);

    for (int i = 0; i < employee->reportCount; i++)
        displayHierarchy(employee->reports[i], level + 1);
}

int countunder(struct Employee *employee)
{
    if (employee == NULL || !employee->active)
        return 0;

    int count = 0;

    for (int i = 0; i < employee->reportCount; i++)
        count += 1 + countunder(employee->reports[i]);

    return count;
}

void displayemployeehierarchy(void)
{
    int id;

    if (readinteger("Enter Employee ID: ", &id) != 1)
        return;

    struct Employee *employee = findEmployee(id);

    if (employee == NULL)
    {
        printf("Employee not found.\n");
        return;
    }

    printf("\n===== EMPLOYEE HIERARCHY =====\n");

    displayHierarchy(employee, 0);

    printf("Total employees under %d: %d\n",
           id, countunder(employee));
}

void displaycompletehierarchy(void)
{
    int total = 0;

    printf("\n===== ORGANIZATION HIERARCHY =====\n");

    for (int i = 0; i < MAX_EMPLOYEES; i++)
    {
        if (employees[i].active &&
            employees[i].manager == NULL)
        {
            displayHierarchy(&employees[i], 0);
            total += 1 + countunder(&employees[i]);
        }
    }

    if (total == 0)
        printf("No employees found.\n");

    printf("Total employees: %d\n", total);
}

int main(void)
{
    int choice;

    while (1)
    {
        printf("\n===== EMPLOYEE HIERARCHY MANAGEMENT =====\n");
        printf("1. Add Employee\n");
        printf("2. Remove Employee\n");
        printf("3. Display Employee Hierarchy\n");
        printf("4. Display Complete Hierarchy\n");
        printf("5. Exit\n");

        int result = readinteger("Enter your choice: ", &choice);

        if (result == -1)
            break;

        if (result == 0)
            continue;

        switch (choice)
        {
            case 1:
                addemployee();
                break;

            case 2:
                removeemployee();
                break;

            case 3:
                displayemployeehierarchy();
                break;

            case 4:
                displaycompletehierarchy();
                break;

            case 5:
                printf("Exiting program.\n");
                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }

    return 0;
}
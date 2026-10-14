
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define STANDARD 150
#define PREMIUM 250
#define RECLINER 400

int main(void)
{
    char name[200];
    char category[20];
    int tickets, choice, price;
    float subtotal, discount, final_bill;
    int name_position;

    // Read and validate customer name
    if (fgets(name, sizeof(name), stdin) == NULL)
    {
        printf("Error: Invalid Input.\n");
        return 0;
    }

    // Remove the newline character from the name
    name_position = 0;

    while (name[name_position] != '\0')
    {
        if (name[name_position] == '\n')
        {
            name[name_position] = '\0';
            break;
        }

        name_position++;
    }

    if (name[0] == '\0')
    {
        printf("Error: Name cannot be empty.\n");
        return 0;
    }

    // name validation
    for (name_position = 0; name[name_position] != '\0'; name_position++)
    {
        if (!isalpha(name[name_position]))
        {
            printf("Error: Name cannot contain special characters.\n");
            return 0;
        }
    }

    // ticket count validation
    if (scanf("%d", &tickets) != 1 || tickets < 1 || tickets > 20)
    {
        printf("Error: Invalid Ticket Count.\n");
        return 0;
    }

    // read and validate category
    if (scanf("%d", &choice) != 1)
    {
        printf("Error: Invalid Category.\n");
        return 0;
    }

    switch (choice)
    {
        case 1:
            price = STANDARD;
            strcpy(category, "Standard");
            break;

        case 2:
            price = PREMIUM;
            strcpy(category, "Premium");
            break;

        case 3:
            price = RECLINER;
            strcpy(category, "Recliner");
            break;

        default:
            printf("Error: Invalid Category.\n");
            return 0;
    }

    // subtotal
    subtotal = price * tickets;

    // discount 
    if (tickets >= 5)
    {
        discount = subtotal * 10 / 100;
    }
    else
    {
        discount = 0;
    }

    // bill
    final_bill = subtotal - discount;

    // booking details
    printf("Customer: %s\n", name);
    printf("Ticket Category: %s\n", category);
    printf("Tickets: %d\n", tickets);
    printf("Subtotal: %.2f\n", subtotal);
    printf("Discount: %.2f\n", discount);
    printf("Final Bill: %.2f\n", final_bill);

    return 0;
}

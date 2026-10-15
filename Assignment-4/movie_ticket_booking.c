#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define STANDARD 150
#define PREMIUM 250
#define RECLINER 400

float calculate_subtotal(float ticket_price, int tickets)
{
    return ticket_price * tickets;
}

float calculate_discount(float subtotal)
{
    return subtotal * 0.1;
}

int main(void)
{
    char name[200];
    char category[20];
    int tickets, choice, price;
    float subtotal, discount, final_bill;
    int name_position;

    if (fgets(name, sizeof(name), stdin) == NULL)
    {
        printf("Error: Invalid Input.\n");
        return 0;
    }

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

    for (name_position = 0; name[name_position] != '\0'; name_position++)
    {
        if (!isalpha(name[name_position]))
        {
            printf("Error: Name cannot contain special characters.\n");
            return 0;
        }
    }

    if (scanf("%d", &tickets) != 1 || tickets < 1 || tickets > 20)
    {
        printf("Error: Invalid Ticket Count.\n");
        return 0;
    }

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

    subtotal = calculate_subtotal(price, tickets);

    if (tickets >= 5)
    {
        discount = calculate_discount(subtotal);
    }
    else
    {
        discount = 0;
    }

    final_bill = subtotal - discount;

    printf("Customer: %s\n", name);
    printf("Ticket Category: %s\n", category);
    printf("Tickets: %d\n", tickets);
    printf("Subtotal: %.2f\n", subtotal);
    printf("Discount: %.2f\n", discount);
    printf("Final Bill: %.2f\n", final_bill);

    return 0;
}

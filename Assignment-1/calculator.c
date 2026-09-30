#include <stdio.h>
#include <ctype.h>

#define SIZE 100

int main()
{
    char expression[SIZE];
    int result = 0;
    int lastNumber = 0;
    int number = 0;
    char operation = '+';
    int i = 0;
    int expectingNumber = 1;

    fgets(expression, SIZE, stdin);

    while (expression[i] != '\0' && expression[i] != '\n')
    {
        if (isspace((unsigned char)expression[i]))
        {
            i++;
            continue;
        }

        if (isdigit((unsigned char)expression[i]))
        {
            if (!expectingNumber)
            {
                printf("Error: Invalid expression.\n");
                return 0;
            }

            number = 0;

            while (isdigit((unsigned char)expression[i]))
            {
                number = number * 10 + (expression[i] - '0');
                i++;
            }

            if (operation == '+')
            {
                result += lastNumber;
                lastNumber = number;
            }
            else if (operation == '-')
            {
                result += lastNumber;
                lastNumber = -number;
            }
            else if (operation == '*')
            {
                lastNumber *= number;
            }
            else if (operation == '/')
            {
                if (number == 0)
                {
                    printf("Error: Division by zero.\n");
                    return 0;
                }

                lastNumber /= number;
            }

            expectingNumber = 0;
        }
        else if (expression[i] == '+' ||
                 expression[i] == '-' ||
                 expression[i] == '*' ||
                 expression[i] == '/')
        {
            if (expectingNumber)
            {
                printf("Error: Invalid expression.\n");
                return 0;
            }

            operation = expression[i];
            expectingNumber = 1;
            i++;
        }
        else
        {
            printf("Error: Invalid expression.\n");
            return 0;
        }
    }

    if (expectingNumber)
    {
        printf("Error: Invalid expression.\n");
        return 0;
    }

    result += lastNumber;

    printf("%d\n", result);

    return 0;
}

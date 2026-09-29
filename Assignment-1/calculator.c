#include <stdio.h>
#include <ctype.h>

#define MAX_SIZE 100

int calculate(char expression[], int *result);

int main()
{
    char expression[MAX_SIZE];
    int result;

    printf("Enter expression: ");
    fgets(expression, MAX_SIZE, stdin);

    if (calculate(expression, &result) == 0)
    {
        printf("%d\n", result);
    }

    return 0;
}

int calculate(char expression[], int *result)
{
    int currentNumber = 0;
    int previousNumber = 0;
    int total = 0;
    char operation = '+';
    int i = 0;

    while (expression[i] != '\0' && expression[i] != '\n')
    {
        if (isspace(expression[i]))
        {
            i++;
            continue;
        }

        if (isdigit(expression[i]))
        {
            currentNumber = 0;

            while (isdigit(expression[i]))
            {
                currentNumber = currentNumber * 10
                               + (expression[i] - '0');
                i++;
            }

            if (operation == '+')
            {
                total += previousNumber;
                previousNumber = currentNumber;
            }
            else if (operation == '-')
            {
                total += previousNumber;
                previousNumber = -currentNumber;
            }
            else if (operation == '*')
            {
                previousNumber = previousNumber * currentNumber;
            }
            else if (operation == '/')
            {
                previousNumber = previousNumber / currentNumber;
            }
        }
        else if (expression[i] == '+' ||
                 expression[i] == '-' ||
                 expression[i] == '*' ||
                 expression[i] == '/')
        {
            operation = expression[i];
            i++;
        }
        else
        {
            printf("Error: Invalid expression.\n");
            return 1;
        }
    }

    total += previousNumber;
    *result = total;

    return 0;
}
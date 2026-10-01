#include <stdio.h>
#include <ctype.h>
#define SIZE 100

int main()
{
    char expression[SIZE];

    long long result = 0;
    long long lastNumber = 0;
    long long currentNumber = 0;

    char operation = '+';

    int position = 0;

    int expectingNumber = 1;

    fgets(expression, SIZE, stdin);

    while (expression[position] != '\0' &&
           expression[position] != '\n')
    {
        if (isspace((unsigned char)expression[position]))
        {
            position++;
            continue;
        }

        if (isdigit((unsigned char)expression[position]))
        {
            if (!expectingNumber)
            {
                printf("Error: Invalid expression.\n");
                return 0;
            }

            currentNumber = 0;

            while (isdigit((unsigned char)expression[position]))
            {
                currentNumber = currentNumber * 10 +(expression[position] - '0');
                position++;
            }

            if (operation == '+')
            {
                result += lastNumber;
                lastNumber = currentNumber;
            }
            else if (operation == '-')
            {
                result += lastNumber;
                lastNumber = -currentNumber;
            }
            else if (operation == '*')
            {
                lastNumber *= currentNumber;
            }
            else if (operation == '/')
            {
                if (currentNumber == 0)
                {
                    printf("Error: Division by zero.\n");
                    return 0;
                }

                lastNumber /= currentNumber;
            }

            expectingNumber = 0;
        }
        else if (expression[position] == '+' ||
                 expression[position] == '-' ||
                 expression[position] == '*' ||
                 expression[position] == '/')
        {
            if (expectingNumber)
            {
                printf("Error: Invalid expression.\n");
                return 0;
            }

            operation = expression[position];
            expectingNumber = 1;
            position++;
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

    printf("%lld\n", result);

    return 0;
<<<<<<< HEAD
}
=======
}
>>>>>>> 95bfe14 (fix: calculator review comments)

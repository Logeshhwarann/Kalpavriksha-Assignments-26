#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_STUDENTS 100

struct Student
{
    int roll_number;
    char name[50];
    int marks[3];
    int total_marks;
    float average_mark;
    char grade;
};

void clear_input_buffer()
{
    int character;

    while ((character = getchar()) != '\n' && character != EOF)
    {
    }
}

int valid_student_count(int student_count)
{
    if (student_count >= 1 && student_count <= MAX_STUDENTS)
        return 1;

    return 0;
}

int valid_roll_number(int roll_number)
{
    if (roll_number > 0)
        return 1;

    return 0;
}

int valid_name(char name[])
{
    int position = 0;

    if (name[0] == '\0')
        return 0;

    while (name[position] != '\0')
    {
        if (!isalpha(name[position]))
            return 0;

        position++;
    }

    return 1;
}

int valid_marks(int marks[])
{
    int position;

    for (position = 0; position < 3; position++)
    {
        if (marks[position] < 0 || marks[position] > 100)
            return 0;
    }

    return 1;
}

int calculate_total(int marks[])
{
    return marks[0] + marks[1] + marks[2];
}

float calculate_average(int total)
{
    return (float)total / 3;
}

char calculate_grade(float average)
{
    if (average >= 85)
        return 'A';
    if (average >= 70)
        return 'B';
    if (average >= 50)
        return 'C';
    if (average >= 35)
        return 'D';
    return 'F';
}

void display_performance(char grade)
{
    switch (grade)
    {
        case 'A':
            printf("*****");
            break;

        case 'B':
            printf("****");
            break;

        case 'C':
            printf("***");
            break;

        case 'D':
            printf("**");
            break;
    }
}

void print_roll_numbers(struct Student students[],int position,int student_count)
{
    if (position >= student_count)
        return;

    printf("%d", students[position].roll_number);

    if (position < student_count - 1)
        printf(" ");

    print_roll_numbers(students, position + 1, student_count);
}

int main()
{
    struct Student students[MAX_STUDENTS];

    int student_count;
    int student_index;

    printf("Enter the number of students: ");

    if (scanf("%d", &student_count) != 1 ||
        !valid_student_count(student_count))
    {
        printf("Invalid number of students.\n");
        return 1;
    }

    clear_input_buffer();

    for (student_index = 0;student_index < student_count;student_index++)
    {
        char input[200];

        while (1)
        {
            printf("Enter details for student %d: ",
                   student_index + 1);

            if (fgets(input, sizeof(input), stdin) == NULL)
            {
                printf("Invalid input.\n");
                return 1;
            }

            int values_read = sscanf(
                input,
                "%d %49s %d %d %d",
                &students[student_index].roll_number,
                students[student_index].name,
                &students[student_index].marks[0],
                &students[student_index].marks[1],
                &students[student_index].marks[2]
            );

            if (values_read != 5)
            {
                printf("Invalid input. Use: Roll Name Mark1 Mark2 Mark3\n");
                continue;
            }

            if (!valid_roll_number(
                    students[student_index].roll_number))
            {
                printf("Invalid roll number.\n");
                continue;
            }

            if (!valid_name(students[student_index].name))
            {
                printf("Invalid name. Enter alphabets only.\n");
                continue;
            }

            if (!valid_marks(students[student_index].marks))
            {
                printf("Invalid marks. Marks must be between 0 and 100.\n");
                continue;
            }

            break;
        }

        students[student_index].total_marks = calculate_total(students[student_index].marks);

        students[student_index].average_mark = calculate_average(students[student_index].total_marks);

        students[student_index].grade = calculate_grade(students[student_index].average_mark);
    }

    printf("\nSTUDENT PERFORMANCE\n");

    for (student_index = 0;student_index < student_count;student_index++)
    {
        printf("\nRoll: %d\n",students[student_index].roll_number);
        printf("Name: %s\n",students[student_index].name);
        printf("Total: %d\n",students[student_index].total_marks);
        printf("Average: %.2f\n",students[student_index].average_mark);
        printf("Grade: %c\n",students[student_index].grade);

        if (students[student_index].average_mark < 35)
            continue;

        printf("Performance: ");
        display_performance(students[student_index].grade);
        printf("\n");
    }

    printf("\nList of Roll Numbers (via recursion): ");
    print_roll_numbers(students, 0, student_count);
    printf("\n");
    return 0;
}
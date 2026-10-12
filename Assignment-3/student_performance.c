#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_STUDENTS 100

struct Student
{
    int roll_number;
    char name[200];
    int marks[3];
    int total_marks;
    float average_mark;
    char grade;
};

void clear_input_buffer()
{
    int input_character;

    while ((input_character = getchar()) != '\n' && input_character != EOF)
    {
    }
}

int valid_student_count(int student_count)
{
    return student_count >= 1 && student_count <= MAX_STUDENTS;
}

// checks if rollno is positive.
int valid_roll_number(int roll_number)
{
    return roll_number > 0;
}

// rollno validation.
int rollNumberExists(struct Student students[], int current_student_index, int roll_number)
{
    int student_position;

    for (student_position = 0; student_position < current_student_index; student_position++)
    {
        if (students[student_position].roll_number == roll_number)
            return 1;
    }

    return 0;
}

//  validates name.
int valid_name(char name[])
{
    int name_position = 0;

    if (name[0] == '\0')
        return 0;

    while (name[name_position] != '\0')
    {
        if (!isalpha((unsigned char)name[name_position]))
            return 0;

        name_position++;
    }

    return 1;
}

// Checks if marks is between 0 and 100.
int valid_marks(int marks[])
{
    int subject_position;

    for (subject_position = 0; subject_position < 3; subject_position++)
    {
        if (marks[subject_position] < 0 || marks[subject_position] > 100)
            return 0;
    }

    return 1;
}

// adds the marks of all three subjects.
int calculate_total(int marks[])
{
    return marks[0] + marks[1] + marks[2];
}

// converts the total into float
float calculate_average(int total_marks)
{
    return (float)total_marks / 3;
}

// assigns a grade according to the average marks.
char calculate_grade(float average_mark)
{
    if (average_mark >= 85)
        return 'A';
    else if (average_mark >= 70)
        return 'B';
    else if (average_mark >= 50)
        return 'C';
    else if (average_mark >= 35)
        return 'D';
    else
        return 'F';
}

// prints stars according to the grade.
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

// Prints roll numbers recursively.
void print_roll_numbers(struct Student students[], int student_position, int student_count)
{
    if (student_position >= student_count)
        return;

    printf("%d", students[student_position].roll_number);

    if (student_position < student_count - 1)
        printf(" ");

    // Calls itself for the next student.
    print_roll_numbers(students, student_position + 1, student_count);
}

int main()
{
    struct Student students[MAX_STUDENTS];

    int student_count;
    int student_index;

    char count_input[100];
    char extra_character;
    int values_read;

    // Reads input for number of students
    printf("Enter the number of students\n");

    if (fgets(count_input, sizeof(count_input), stdin) == NULL)
    {
        printf("Invalid input.\n");
        return 1;
    }

    // Rejects a student-count line that is too long.
    if (strchr(count_input, '\n') == NULL && !feof(stdin))
    {
        clear_input_buffer();
        printf("Invalid number of students.\n");
        return 1;
    }

    values_read = sscanf(count_input, "%d %c", &student_count, &extra_character);

    if (values_read != 1 || !valid_student_count(student_count))
    {
        printf("Invalid number of students.\n");
        return 1;
    }

    // Reads and validates each student's details.
    for (student_index = 0; student_index < student_count; student_index++)
    {
        char input[300];

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            printf("Invalid input.\n");
            return 1;
        }

        // Rejects a student record that exceeds the input buffer.
        if (strchr(input, '\n') == NULL && !feof(stdin))
        {
            clear_input_buffer();
            printf("Invalid input. Line is too long.\n");
            return 1;
        }

        // Reads roll number, name, and three marks.
        values_read = sscanf(input, "%d %199s %d %d %d %c",
            &students[student_index].roll_number,
            students[student_index].name,
            &students[student_index].marks[0],
            &students[student_index].marks[1],
            &students[student_index].marks[2],
            &extra_character);

        if (values_read != 5)
        {
            printf("Invalid input, Use this format Roll Name Mark1 Mark2 Mark3\n");
            return 1;
        }

        if (!valid_roll_number(students[student_index].roll_number))
        {
            printf("Invalid roll number.\n");
            return 1;
        }

        // checks for duplicate rool number a roll number
        if (rollNumberExists(students, student_index, students[student_index].roll_number))
        {
            printf("Roll number already used.\n");
            return 1;
        }

        if (!valid_name(students[student_index].name))
        {
            printf("Name contains only alphabets, Invalid name.\n");
            return 1;
        }

        if (!valid_marks(students[student_index].marks))
        {
            printf("Invalid marks. Marks must be between 0 and 100.\n");
            return 1;
        }

        // calculates and stores the student results.
        students[student_index].total_marks = calculate_total(students[student_index].marks);
        students[student_index].average_mark = calculate_average(students[student_index].total_marks);
        students[student_index].grade = calculate_grade(students[student_index].average_mark);
    }

    // displays the results after all student records hav been validated.
    for (student_index = 0; student_index < student_count; student_index++)
    {
        printf("\nRoll: %d\n", students[student_index].roll_number);
        printf("Name: %s\n", students[student_index].name);
        printf("Total: %d\n", students[student_index].total_marks);
        printf("Average: %.2f\n", students[student_index].average_mark);
        printf("Grade: %c\n", students[student_index].grade);

        if (students[student_index].average_mark < 35)
            continue;

        printf("Performance: ");
        display_performance(students[student_index].grade);
        printf("\n");
    }

    // prints all roll numbers using recursion.
    printf("\nList of Roll Numbers (via recursion): ");
    print_roll_numbers(students, 0, student_count);
    printf("\n");

    return 0;
}
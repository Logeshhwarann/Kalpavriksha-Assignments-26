#include <stdio.h>

#define MAX_STUDENTS 100

struct Student
{
    int rollNumber;
    char name[50];
    int mark1;
    int mark2;
    int mark3;
};

void clearBuffer()
{
    int inputCharacter;

    while ((inputCharacter = getchar()) != '\n' && inputCharacter != EOF)
    {
    }
}

int calculateTotal(struct Student student)
{
    return student.mark1 + student.mark2 + student.mark3;
}

float calculateAverage(int total)
{
    return total / 3.0;
}

char calculateGrade(float average)
{
    if (average >= 85)
        return 'A';
    else if (average >= 70)
        return 'B';
    else if (average >= 50)
        return 'C';
    else if (average >= 35)
        return 'D';
    else
        return 'F';
}

// Recursive function to print roll numbers
void printRollNumbers(struct Student student[],int position,int studentCount)
{
    if (position == studentCount)
        return;

    printf("%d ", student[position].rollNumber);

    printRollNumbers(student,position + 1,studentCount);
}

int main()
{
    struct Student student[MAX_STUDENTS];

    int studentCount;
    int count;

    while (1)
    {
        printf("Enter number of students (1-100): ");

        // Student count validation
        if (scanf("%d", &studentCount) == 1 &&
            studentCount >= 1 &&
            studentCount <= MAX_STUDENTS)
        {
            clearBuffer();
            break;
        }

        printf("Student count must be between 1 and 100.\n");
        clearBuffer();
    }

    for (count = 0; count < studentCount; count++)
    {
        printf("\nEnter student %d details\n", count + 1);

        // Roll number validation
        while (1)
        {
            printf("Roll Number: ");

            if (scanf("%d", &student[count].rollNumber) == 1 &&
                student[count].rollNumber > 0)
            {
                clearBuffer();
                break;
            }

            printf("Invalid roll number, enter number greater than 0.\n");
            clearBuffer();
        }

        printf("Name: ");
        scanf("%49s", student[count].name);
        clearBuffer();

        printf("Mark 1: ");
        scanf("%d", &student[count].mark1);
        clearBuffer();

        printf("Mark 2: ");
        scanf("%d", &student[count].mark2);
        clearBuffer();

        printf("Mark 3: ");
        scanf("%d", &student[count].mark3);
        clearBuffer();
    }

    printf("\nSTUDENT PERFORMANCE\n");

    for (count = 0; count < studentCount; count++)
    {
        int totalMarks = calculateTotal(student[count]);
        float average = calculateAverage(totalMarks);
        char grade = calculateGrade(average);

        printf("\nRoll Number : %d\n", student[count].rollNumber);
        printf("Name          : %s\n", student[count].name);
        printf("Total         : %d\n", totalMarks);
        printf("Average       : %.2f\n", average);
        printf("Grade         : %c\n", grade);

        if (average < 35)
            continue;

        printf("Performance : ");

        if (grade == 'A')
            printf("*****");
        else if (grade == 'B')
            printf("****");
        else if (grade == 'C')
            printf("***");
        else if (grade == 'D')
            printf("**");

        printf("\n");
    }

    printf("\nRoll Numbers using Recursion: ");

    printRollNumbers(student, 0, studentCount);

    printf("\n");

    return 0;
}
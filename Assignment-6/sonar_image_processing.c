
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 10

int generateMatrix(int **matrixPointer, int matrixSize)
{
    int row, column, position;

    for (row = 0; row < matrixSize; row++) {
        for (column = 0; column < matrixSize; column++) {

            position = row * matrixSize + column;
            *(matrixPointer + position) = malloc(sizeof(int));
            if (*(matrixPointer + position) == NULL) {
                int previousPosition;

                for (previousPosition = 0; previousPosition < position; previousPosition++) {
                    free(*(matrixPointer + previousPosition));
                }
                return 0;
            }
            *(*(matrixPointer + position)) = rand() % 256;
        }
    }
    return 1;
}

void displayMatrix(int **matrixPointer, int matrixSize)
{
    int row, column, position;
    for (row = 0; row < matrixSize; row++) {
        for (column = 0; column < matrixSize; column++) {
            position = row * matrixSize + column;
            printf("%3d ", *(*(matrixPointer + position)));
        }
        printf("\n");
    }
}

void rotateMatrix(int **matrixPointer, int matrixSize)
{
    int row, column;
    int **firstPointer, **secondPointer;
    int *temporaryPointer;
    for (row = 0; row < matrixSize; row++) {
        for (column = row + 1; column < matrixSize; column++) {
            firstPointer = matrixPointer + row * matrixSize + column;
            secondPointer = matrixPointer + column * matrixSize + row;
            temporaryPointer = *firstPointer;
            *firstPointer = *secondPointer;
            *secondPointer = temporaryPointer;
        }
    }

    for (row = 0; row < matrixSize; row++) {
        for (column = 0; column < matrixSize / 2; column++) {
            firstPointer = matrixPointer + row * matrixSize + column;
            secondPointer = matrixPointer + row * matrixSize + (matrixSize - 1 - column);
            temporaryPointer = *firstPointer;
            *firstPointer = *secondPointer;
            *secondPointer = temporaryPointer;
        }
    }
}

void smoothMatrix(int **matrixPointer, int matrixSize)
{
    int previousRow[MAX], currentRow[MAX];
    int row, column, neighborRow, neighborColumn;
    int neighborSum, neighborCount, position;
    for (column = 0; column < matrixSize; column++) {
        *(currentRow + column) = *(*(matrixPointer + column));
    }

    for (row = 0; row < matrixSize; row++) {
        for (column = 0; column < matrixSize; column++) {
            neighborSum = 0;
            neighborCount = 0;
            for (neighborRow = row - 1; neighborRow <= row + 1; neighborRow++) {
                for (neighborColumn = column - 1; neighborColumn <= column + 1; neighborColumn++) {

                    if (neighborRow >= 0 && neighborRow < matrixSize && neighborColumn >= 0 && neighborColumn < matrixSize) {

                        if (neighborRow == row - 1) {
                            neighborSum += *(previousRow + neighborColumn);
                        }
                        else if (neighborRow == row) {
                            neighborSum += *(currentRow + neighborColumn);
                        }
                        else {
                            position = neighborRow * matrixSize + neighborColumn;
                            neighborSum += *(*(matrixPointer + position));
                        }
                        neighborCount++;
                    }
                }
            }
            position = row * matrixSize + column;
            *(*(matrixPointer + position)) = neighborSum / neighborCount;
        }
        for (column = 0; column < matrixSize; column++) {
            *(previousRow + column) = *(currentRow + column);
        }
        if (row + 1 < matrixSize) {
            for (column = 0; column < matrixSize; column++) {
                position = (row + 1) * matrixSize + column;
                *(currentRow + column) = *(*(matrixPointer + position));
            }
        }
    }
}

int main(void)
{
    int matrixSize;
    int position;
    int *matrix[MAX * MAX];
    int **matrixPointer = matrix;
    printf("Enter matrix size (2-10): ");
    if (scanf("%d", &matrixSize) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    if (matrixSize < 2 || matrixSize > MAX) {
        printf("Invalid size.\n");
        return 1;
    }

    srand(time(NULL));

    if (!generateMatrix(matrixPointer, matrixSize)) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    printf("\nOriginal Randomly Generated Matrix:\n");
    displayMatrix(matrixPointer, matrixSize);

    rotateMatrix(matrixPointer, matrixSize);

    printf("\nMatrix after 90-degree Clockwise Rotation:\n");
    displayMatrix(matrixPointer, matrixSize);

    smoothMatrix(matrixPointer, matrixSize);

    printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
    displayMatrix(matrixPointer, matrixSize);

    for (position = 0; position < matrixSize * matrixSize; position++) {
        free(*(matrixPointer + position));
    }
    return 0;
}

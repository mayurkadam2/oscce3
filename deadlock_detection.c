#include <stdio.h>

#define P 4   // Number of processes
#define R 3   // Number of resource types

int main()
{
    // Allocation Matrix
    int allocation[P][R] = {
        {1, 0, 1},
        {1, 1, 0},
        {0, 1, 1},
        {0, 0, 0}
    };

    // Request Matrix
    int request[P][R] = {
        {0, 1, 0},
        {0, 0, 1},
        {1, 0, 0},
        {0, 0, 0}
    };

    // Available Resources
    int available[R] = {0, 0, 0};

    int work[R];
    int finish[P] = {0, 0, 0, 0};

    // Work = Available
    for (int j = 0; j < R; j++)
    {
        work[j] = available[j];
    }

    // Deadlock Detection Algorithm
    int found;

    do
    {
        found = 0;

        for (int i = 0; i < P; i++)
        {
            if (finish[i] == 1)
                continue;

            int canFinish = 1;

            // Check Request[i] <= Work
            for (int j = 0; j < R; j++)
            {
                if (request[i][j] > work[j])
                {
                    canFinish = 0;
                    break;
                }
            }

            // If process can finish
            if (canFinish)
            {
                finish[i] = 1;
                found = 1;

                // Release allocated resources
                for (int j = 0; j < R; j++)
                {
                    work[j] += allocation[i][j];
                }
            }
        }

    } while (found);

    // Display deadlocked processes
    int deadlock = 0;

    printf("Deadlocked Processes: ");

    for (int i = 0; i < P; i++)
    {
        if (finish[i] == 0)
        {
            printf("P%d ", i);
            deadlock = 1;
        }
    }

    if (deadlock)
    {
        printf("\nDeadlock exists.\n");
    }
    else
    {
        printf("None\n");
        printf("No deadlock exists.\n");
    }

    return 0;
}


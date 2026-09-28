#include <stdio.h>

#define P 4
#define R 3

int main()
{
    int allocation[P][R];
    int request[P][R];
    int available[R];
    int work[R];
    int finish[P];

    // Input Allocation Matrix
    printf("Enter Allocation Matrix (%d x %d):\n", P, R);

    for (int i = 0; i < P; i++)
    {
        for (int j = 0; j < R; j++)
        {
            scanf("%d", &allocation[i][j]);
        }
    }

    // Input Request Matrix
    printf("\nEnter Request Matrix (%d x %d):\n", P, R);

    for (int i = 0; i < P; i++)
    {
        for (int j = 0; j < R; j++)
        {
            scanf("%d", &request[i][j]);
        }
    }

    // Input Available Vector
    printf("\nEnter Available Resources:\n");

    for (int j = 0; j < R; j++)
    {
        scanf("%d", &available[j]);
    }

    // Work = Available
    for (int j = 0; j < R; j++)
    {
        work[j] = available[j];
    }

    // Initially no process has finished
    for (int i = 0; i < P; i++)
    {
        finish[i] = 0;
    }

    // Deadlock Detection Algorithm
    int found;

    do
    {
        found = 0;

        for (int i = 0; i < P; i++)
        {
            // Skip if process has already finished
            if (finish[i] == 1)
                continue;

            // Check whether Request[i] <= Work
            int canFinish = 1;

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
                printf("\nP%d can finish.", i);

                // Release its allocated resources
                for (int j = 0; j < R; j++)
                {
                    work[j] += allocation[i][j];
                }

                finish[i] = 1;
                found = 1;
            }
        }

    } while (found);

    // Check for deadlocked processes
    int deadlock = 0;

    printf("\n\nDeadlocked Processes: ");

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
        printf("\n\nDeadlock exists.");
    }
    else
    {
        printf("None");
        printf("\n\nNo deadlock exists.");
    }

    return 0;
}
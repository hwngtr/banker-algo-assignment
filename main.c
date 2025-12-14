
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4

int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES]={0};
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

int release_resources(int customer_num, int release[]);
int request_resources(int customer_num, int request[]);
void printState();
int bankerAlgorithm(int customer_num,int request[]);


int main(int argc, const char * argv[]) {
    if (argc != NUMBER_OF_RESOURCES + 1) {
        printf("Usage: ./a.out <res1> <res2> <res3> <res4>\n");
        return -1;
    }

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] = atoi(argv[i + 1]);
    }

    FILE *fp = fopen("input.txt", "r");
    if (fp == NULL) {
        printf("Error: could not open input.txt\n");
        return -1;
    }
    char line[256];
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        if (fgets(line, sizeof(line), fp) == NULL) {
            break; 
        }
        char *token = strtok(line, ",");
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            if (token == NULL) {
                break;
            }
            maximum[i][j] = atoi(token);
            need[i][j] = maximum[i][j];
            token = strtok(NULL, ",");
        }
    }
    fclose(fp);

    char command[10];
    int customer_num;
    int request[NUMBER_OF_RESOURCES];
    int release[NUMBER_OF_RESOURCES];

    while (1) {
        printf("\nEnter command: ");
        scanf("%s", command);

        if (strcmp(command, "RQ") == 0) {
            scanf("%d", &customer_num);
            for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
                scanf("%d", &request[i]);
            }
            request_resources(customer_num, request);
        } else if (strcmp(command, "RL") == 0) {
            scanf("%d", &customer_num);
            for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
                scanf("%d", &release[i]);
            }
            release_resources(customer_num, release);
        } else if (strcmp(command, "*") == 0) {
            printState();
        } else if (strcmp(command, "exit") == 0) {
            break;
        } else {
            printf("Invalid command.\n");
        }
    }

    return 0;
}


int request_resources(int customer_num, int request[]) {
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (request[i] > need[customer_num][i]) {
            printf("Error: Process has exceeded its maximum claim.\n");
            return -1;
        }
    }

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (request[i] > available[i]) {
            printf("Request denied. Resources not available.\n");
            return -1;
        }
    }

    if (bankerAlgorithm(customer_num, request) == 0) {
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
            available[i] -= request[i];
            allocation[customer_num][i] += request[i];
            need[customer_num][i] -= request[i];
        }
        printf("Request granted.\n");
        return 0;
    } else {
        printf("Request denied. Granting request would lead to an unsafe state.\n");
        return -1;
    }
}


int release_resources(int customer_num, int release[]) {
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (release[i] > allocation[customer_num][i]) {
            printf("Error: Cannot release more resources than allocated.\n");
            return -1;
        }
    }

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] += release[i];
        allocation[customer_num][i] -= release[i];
    }
    printf("Resources released successfully.\n");
    return 0;
}
int bankerAlgorithm(int customer_num, int request[]) {
    int work[NUMBER_OF_RESOURCES];
    int finish[NUMBER_OF_CUSTOMERS];
    int temp_alloc[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
    int temp_need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        work[i] = available[i] - request[i];
    }

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        finish[i] = 0;
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            temp_alloc[i][j] = allocation[i][j];
            temp_need[i][j] = need[i][j];
        }
    }

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        temp_alloc[customer_num][i] += request[i];
        temp_need[customer_num][i] -= request[i];
    }

    while (1) {
        int found = 0;
        for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
            if (finish[i] == 0) {
                int can_finish = 1;
                for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                    if (temp_need[i][j] > work[j]) {
                        can_finish = 0;
                        break;
                    }
                }
                if (can_finish) {
                    for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
                        work[j] += temp_alloc[i][j];
                    }
                    finish[i] = 1;
                    found = 1;
                }
            }
        }
        if (!found) {
            break;
        }
    }

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        if (finish[i] == 0) {
            return -1;
        }
    }

    return 0;
}


void printState() {
    printf("\n--- System State ---\n");
    printf("Maximum Resources:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printf("P%d: ", i);
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("%d ", maximum[i][j]);
        }
        printf("\n");
    }

    printf("\nAllocated Resources:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printf("P%d: ", i);
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("%d ", allocation[i][j]);
        }
        printf("\n");
    }

    printf("\nNeeded Resources:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        printf("P%d: ", i);
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("%d ", need[i][j]);
        }
        printf("\n");
    }

    printf("\nAvailable Resources:\n");
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        printf("%d ", available[i]);
    }
    printf("\n--------------------\n");
}


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAX_PROCESSES 100
#define LEVELS 4

#ifdef _WIN32
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif


typedef struct {
    int pid;
    int arrival;
    int burst;
    int remaining;
    int completion;
    int turnaround;
    int response;
    bool started;
    int queueLevel;
    int allotmentUsed;
} Process;

Process processes[MAX_PROCESSES];
int n;

void inputProcesses() {
    printf("Enter number of processes: ");
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        processes[i].pid = i + 1;
        printf("P%d Arrival Time: ", i + 1);
        scanf("%d", &processes[i].arrival);
        printf("P%d Burst Time: ", i + 1);
        scanf("%d", &processes[i].burst);
        processes[i].remaining = processes[i].burst;
        processes[i].started = false;
        processes[i].queueLevel = 0;
        processes[i].allotmentUsed = 0;
    }
}

void randomProcesses() {
    printf("Enter number of processes: ");
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        processes[i].pid = i + 1;
        processes[i].arrival = rand() % 10;
        processes[i].burst = (rand() % 9) + 1;
        processes[i].remaining = processes[i].burst;
        processes[i].started = false;
        processes[i].queueLevel = 0;
        processes[i].allotmentUsed = 0;
        printf("P%d -> Arrival: %d, Burst: %d\n", i + 1, processes[i].arrival, processes[i].burst);
    }
}

void resetProcesses() {
    for (int i = 0; i < n; i++) {
        processes[i].remaining = processes[i].burst;
        processes[i].started = false;
        processes[i].queueLevel = 0;
        processes[i].allotmentUsed = 0;
    }
}

void sortByArrival() {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (processes[j].arrival > processes[j + 1].arrival) {
                Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
}

void printTable(float avgTAT, float avgRT) {
    Process tempArray[MAX_PROCESSES];
    for (int i = 0; i < n; i++) tempArray[i] = processes[i];

    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (tempArray[j].completion > tempArray[j + 1].completion) {
                Process tmp = tempArray[j];
                tempArray[j] = tempArray[j + 1];
                tempArray[j + 1] = tmp;
            }
        }
    }

    printf("\nPID\tAT\tBT\tCT\tTAT\tRT\n");
    for (int i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\n",
               tempArray[i].pid,
               tempArray[i].arrival,
               tempArray[i].burst,
               tempArray[i].completion,
               tempArray[i].turnaround,
               tempArray[i].response);
    }
    printf("\nAverage TAT: %.2f\nAverage RT: %.2f\n", avgTAT, avgRT);
}

void printGantt(int timeline[], int times[], int level[], int count) {
    printf("\nGantt Chart:\n ");
    for (int i = 0; i < count; i++) {
        int width = times[i + 1] - times[i];
        for (int j = 0; j < width * 2; j++) printf("-");
        printf(" ");
    }
    printf("\n|");
    for (int i = 0; i < count; i++) {
        int width = times[i + 1] - times[i];
        int padding = width * 2 - 2;
        int left = padding / 2;
        int right = padding - left;

        for (int j = 0; j < left; j++) printf(" ");
        printf("P%d", timeline[i]);
        for (int j = 0; j < right; j++) printf(" ");
        printf("|");
    }
    printf("\n ");
    for (int i = 0; i < count; i++) {
        int width = times[i + 1] - times[i];
        for (int j = 0; j < width * 2; j++) printf("-");
        printf(" ");
    }
    printf("\n");
    for (int i = 0; i <= count; i++) {
        printf("%-3d", times[i]);
        if (i < count) {
            int width = times[i + 1] - times[i];
            int space = width * 2 - 3;
            for (int j = 0; j < space; j++) printf(" ");
        }
    }
    printf("\n");

    if (level != NULL) {
        printf("\nQueue Levels:\n");
        for (int i = 0; i < count; i++) {
            printf("P%d ran at Q%d\n", timeline[i], level[i]);
        }
    }
}

void FCFS() {
    printf("=============== FCFS Algorithm ===============\n");
    resetProcesses();
    sortByArrival();
    int time = 0;
    int timeline[MAX_PROCESSES], times[MAX_PROCESSES + 1];
    int count = 0;

    times[0] = 0;
    for (int i = 0; i < n; i++) {
        if (time < processes[i].arrival) time = processes[i].arrival;
        processes[i].response = time - processes[i].arrival;
        time += processes[i].burst;
        processes[i].completion = time;
        processes[i].turnaround = processes[i].completion - processes[i].arrival;

        timeline[count] = processes[i].pid;
        times[count + 1] = time;
        count++;
    }

    float avgTAT = 0, avgRT = 0;
    for (int i = 0; i < n; i++) {
        avgTAT += processes[i].turnaround;
        avgRT += processes[i].response;
    }

    printGantt(timeline, times, NULL, count);
    printTable(avgTAT / n, avgRT / n);
}

void SJF() {
    printf("=============== SJF Algorithm ===============\n");
    resetProcesses();
    int completed = 0, time = 0;
    int timeline[MAX_PROCESSES], times[MAX_PROCESSES + 1];
    int count = 0;
    times[0] = 0;

    while (completed < n) {
        int idx = -1, minBT = 1e9;
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival <= time && processes[i].remaining > 0 && processes[i].burst < minBT) {
                minBT = processes[i].burst;
                idx = i;
            }
        }
        if (idx == -1) {
            time++;
            continue;
        }
        processes[idx].response = time - processes[idx].arrival;
        time += processes[idx].burst;
        processes[idx].completion = time;
        processes[idx].turnaround = time - processes[idx].arrival;
        processes[idx].remaining = 0;

        timeline[count] = processes[idx].pid;
        times[count + 1] = time;
        count++;
        completed++;
    }

    float avgTAT = 0, avgRT = 0;
    for (int i = 0; i < n; i++) {
        avgTAT += processes[i].turnaround;
        avgRT += processes[i].response;
    }

    printGantt(timeline, times, NULL, count);
    printTable(avgTAT / n, avgRT / n);
}

void SRTF() {
    printf("=============== SRTF Algorithm ===============\n");
    resetProcesses();
    int completed = 0, time = 0;
    int timeline[MAX_PROCESSES * 20], times[MAX_PROCESSES * 20 + 1];
    int count = 0;
    times[0] = 0;

    while (completed < n) {
        int idx = -1, minRT = 1e9;
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival <= time && processes[i].remaining > 0 && processes[i].remaining < minRT) {
                minRT = processes[i].remaining;
                idx = i;
            }
        }
        if (idx == -1) {
            time++;
            continue;
        }
        if (!processes[idx].started) {
            processes[idx].response = time - processes[idx].arrival;
            processes[idx].started = true;
        }
        timeline[count] = processes[idx].pid;
        time++;
        processes[idx].remaining--;
        times[count + 1] = time;
        count++;

        if (processes[idx].remaining == 0) {
            processes[idx].completion = time;
            processes[idx].turnaround = processes[idx].completion - processes[idx].arrival;
            completed++;
        }
    }

    float avgTAT = 0, avgRT = 0;
    for (int i = 0; i < n; i++) {
        avgTAT += processes[i].turnaround;
        avgRT += processes[i].response;
    }

    printGantt(timeline, times, NULL, count);
    printTable(avgTAT / n, avgRT / n);
}

void RoundRobin() {
    printf("=============== Round Robin Algorithm ===============\n");
    resetProcesses();
    int timeQuantum;
    printf("Enter Time Quantum: ");
    scanf("%d", &timeQuantum);

    int time = 0, completed = 0;
    int timeline[MAX_PROCESSES * 20], times[MAX_PROCESSES * 20 + 1];
    int count = 0;
    times[0] = 0;

    int queue[MAX_PROCESSES], front = 0, rear = 0;
    bool inQueue[MAX_PROCESSES] = {false};

    while (completed < n) {
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival <= time && processes[i].remaining > 0 && !inQueue[i]) {
                queue[rear++] = i;
                inQueue[i] = true;
            }
        }

        if (front == rear) {
            time++;
            continue;
        }

        int idx = queue[front++];
        if (processes[idx].remaining > 0) {
            if (!processes[idx].started) {
                processes[idx].started = true;
                processes[idx].response = time - processes[idx].arrival;
            }

            int runTime = (processes[idx].remaining < timeQuantum) ? processes[idx].remaining : timeQuantum;

            for (int t = 0; t < runTime; t++) {
                timeline[count] = processes[idx].pid;
                time++;
                times[count + 1] = time;
                count++;

                for (int j = 0; j < n; j++) {
                    if (processes[j].arrival == time && processes[j].remaining > 0 && !inQueue[j]) {
                        queue[rear++] = j;
                        inQueue[j] = true;
                    }
                }
            }

            processes[idx].remaining -= runTime;

            if (processes[idx].remaining == 0) {
                processes[idx].completion = time;
                processes[idx].turnaround = time - processes[idx].arrival;
                completed++;
            } else {
                queue[rear++] = idx;
            }
        }
    }

    float avgTAT = 0, avgRT = 0;
    for (int i = 0; i < n; i++) {
        avgTAT += processes[i].turnaround;
        avgRT += processes[i].response;
    }

    printGantt(timeline, times, NULL, count);
    printTable(avgTAT / n, avgRT / n);
}

void MLFQ() {
    printf("=============== Multi-Level Feedback Queue (MLFQ) ===============\n");
    resetProcesses();

    int timeQuantum[LEVELS];
    int maxAllotment[LEVELS];

    for (int i = 0; i < LEVELS; i++) {
        printf("Enter Time Quantum for Queue Level Q%d: ", i);
        scanf("%d", &timeQuantum[i]);
        if (i != LEVELS - 1) {
            printf("Enter Max Allotment for Queue Level Q%d: ", i);
            scanf("%d", &maxAllotment[i]);
        } else {
            maxAllotment[i] = 0; 
        }
    }

    int time = 0, completed = 0;
    int timeline[MAX_PROCESSES * 50], times[MAX_PROCESSES * 50 + 1], levels[MAX_PROCESSES * 50];
    int count = 0;
    times[0] = 0;

    int queues[LEVELS][MAX_PROCESSES];
    int front[LEVELS] = {0}, rear[LEVELS] = {0};
    bool inQueue[MAX_PROCESSES] = {false};

    while (completed < n) {
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival == time && processes[i].remaining > 0 && !inQueue[i]) {
                queues[0][rear[0]++] = i;
                inQueue[i] = true;
            }
        }

        int q = -1;
        for (int i = 0; i < LEVELS; i++) {
            if (front[i] < rear[i]) {
                q = i;
                break;
            }
        }

        if (q == -1) {
            time++;
            continue;
        }

        int idx = queues[q][front[q]++];
        int tq = timeQuantum[q];
        int allotment = maxAllotment[q];

        if (!processes[idx].started) {
            processes[idx].started = true;
            processes[idx].response = time - processes[idx].arrival;
        }

        int runTime = (processes[idx].remaining < tq) ? processes[idx].remaining : tq;
        if (allotment != 0) {
            int remainingAllotment = allotment - processes[idx].allotmentUsed;
            if (runTime > remainingAllotment) runTime = remainingAllotment;
        }

        for (int i = 0; i < runTime; i++) {
            timeline[count] = processes[idx].pid;
            levels[count] = q;
            time++;
            times[count + 1] = time;
            count++;

            for (int j = 0; j < n; j++) {
                if (processes[j].arrival == time && processes[j].remaining > 0 && !inQueue[j]) {
                    queues[0][rear[0]++] = j;
                    inQueue[j] = true;
                }
            }
        }

        processes[idx].remaining -= runTime;
        processes[idx].allotmentUsed += runTime;

        if (processes[idx].remaining == 0) {
            processes[idx].completion = time;
            processes[idx].turnaround = time - processes[idx].arrival;
            completed++;
        } else {
            if (maxAllotment[q] != 0 && processes[idx].allotmentUsed >= maxAllotment[q] && q + 1 < LEVELS) {
                processes[idx].queueLevel++;
                processes[idx].allotmentUsed = 0;
                queues[q + 1][rear[q + 1]++] = idx;
            } else {
                queues[q][rear[q]++] = idx;
            }
        }
    }

    float avgTAT = 0, avgRT = 0;
    for (int i = 0; i < n; i++) {
        avgTAT += processes[i].turnaround;
        avgRT += processes[i].response;
    }

    printGantt(timeline, times, levels, count);
    printTable(avgTAT / n, avgRT / n);
}


void clearScreen() { system(CLEAR); }

int main() {
    srand(time(NULL));
    int choice, inputType;
    char again;

    do {
        clearScreen();
        printf("============ CPU SCHEDULING VISUALIZATION ============\n\n");
        printf("CHOOSE INPUT TYPE\n1. Manual Input\n2. Random Input\nChoice: ");
        scanf("%d", &inputType);
        if (inputType == 1) inputProcesses();
        else randomProcesses();
        

        do {
            clearScreen();
            printf("============ CHOOSE ALGORITHM TO RU:============ \n[1] FCFS\n[2] SJF\n[3] SRTF\n[4] Round Robin\n[5] MLFQ\n[0] Exit\nChoice: ");
            scanf("%d", &choice);
            clearScreen();
            switch (choice) {
                case 1: FCFS(); break;
                case 2: SJF(); break;
                case 3: SRTF(); break;
                case 4: RoundRobin(); break;
                case 5: MLFQ(); break;
                case 0: break;
                default: printf("Invalid choice!\n"); break;
            }
            if (choice != 0) {
                printf("\nTry another algorithm with same processes? (y/n): ");
                getchar();
                scanf("%c", &again);
            } else again = 'n';
        } while (again == 'y' || again == 'Y');

        printf("\nDo you want to input a new set of processes? (y/n): ");
        getchar();
        scanf("%c", &again);

    } while (again == 'y' || again == 'Y');

    printf("\nExiting... Thank you!\n");
    return 0;
}


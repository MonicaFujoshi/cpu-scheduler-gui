#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef _WIN32
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif

#define MAX_PROCESSES 100
#define LEVELS 4

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
    printf("\nPID\tAT\tBT\tCT\tTAT\tRT\n");
    for (int i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\n",
               processes[i].pid, processes[i].arrival, processes[i].burst,
               processes[i].completion, processes[i].turnaround,
               processes[i].response);
    }
    printf("\nAverage TAT: %.2f\nAverage RT: %.2f\n",
           avgTAT, avgRT);
}

void printGantt(int timeline[], int times[], int level[], int count) {
    printf("\nGantt Chart:\n|");
    for (int i = 0; i < count; i++) {
        if (level) printf(" P%d(Q%d) |", timeline[i], level[i]);
        else printf(" P%d |", timeline[i]);
    }
    printf("\n%d", times[0]);
    for (int i = 1; i <= count; i++) {
        printf("   %d", times[i]);
    }
    printf("\n");
}

void FCFS() {
    resetProcesses();
    sortByArrival();

    int time = 0;
    int timeline[MAX_PROCESSES];
    int times[MAX_PROCESSES + 1];
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
    avgTAT /= n; avgRT /= n;

    printGantt(timeline, times, NULL, count);
    printTable(avgTAT, avgRT);
}

void SJF() {
    resetProcesses();
    int completed = 0, time = 0;
    int timeline[MAX_PROCESSES];
    int times[MAX_PROCESSES + 1];
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
        processes[idx].turnaround = processes[idx].completion - processes[idx].arrival;
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
    avgTAT /= n; avgRT /= n;

    printGantt(timeline, times, NULL, count);
    printTable(avgTAT, avgRT);
}

void SRTF() {
    resetProcesses();
    int completed = 0, time = 0;
    int timeline[MAX_PROCESSES * 20];
    int times[MAX_PROCESSES * 20 + 1];
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
    avgTAT /= n; avgRT /= n;

    printGantt(timeline, times, NULL, count);
    printTable(avgTAT, avgRT);
}

void RoundRobin() {
    resetProcesses();
    int timeSlice;
    printf("Enter time slice for Round Robin: ");
    scanf("%d", &timeSlice);

    int time = 0, completed = 0;
    int timeline[MAX_PROCESSES * 50];
    int times[MAX_PROCESSES * 50 + 1];
    int count = 0;
    times[0] = 0;

    int queue[MAX_PROCESSES];
    int front = 0, rear = 0;
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
        if (!processes[idx].started) {
            processes[idx].response = time - processes[idx].arrival;
            processes[idx].started = true;
        }

        int run = processes[idx].remaining > timeSlice ? timeSlice : processes[idx].remaining;
        time += run;
        processes[idx].remaining -= run;

        for (int i = 0; i < n; i++) {
            if (processes[i].arrival <= time && processes[i].remaining > 0 && !inQueue[i]) {
                queue[rear++] = i;
                inQueue[i] = true;
            }
        }
        if (processes[idx].remaining > 0) {
            queue[rear++] = idx;
        } else {
            processes[idx].completion = time;
            processes[idx].turnaround = processes[idx].completion - processes[idx].arrival;
            completed++;
        }

        timeline[count] = processes[idx].pid;
        times[count + 1] = time;
        count++;
    }

    float avgTAT = 0, avgRT = 0;
    for (int i = 0; i < n; i++) {
        avgTAT += processes[i].turnaround;
        avgRT += processes[i].response;
    }
    avgTAT /= n; avgRT /= n;

    printGantt(timeline, times, NULL, count);
    printTable(avgTAT, avgRT);
}

void MLFQ() {
    resetProcesses();
    int quantum[LEVELS], allotment[LEVELS];
    for (int i = 0; i < LEVELS; i++) {
        printf("Q%d Time Slice: ", i);
        scanf("%d", &quantum[i]);
        printf("Q%d Allotment: ", i);
        scanf("%d", &allotment[i]);
    }

    int time = 0, completed = 0;
    int timeline[MAX_PROCESSES * 100];
    int times[MAX_PROCESSES * 100 + 1];
    int levelLog[MAX_PROCESSES * 100];
    int count = 0;
    times[0] = 0;

    int queue[MAX_PROCESSES * LEVELS];
    int front = 0, rear = 0;

    while (completed < n) {
        for (int i = 0; i < n; i++) {
            if (processes[i].arrival <= time && processes[i].remaining > 0) {
                bool alreadyInQueue = false;
                for (int j = front; j < rear; j++) {
                    if (queue[j] == i) {
                        alreadyInQueue = true;
                        break;
                    }
                }
                if (!alreadyInQueue) {
                    queue[rear++] = i;
                }
            }
        }

        if (front == rear) {
            time++;
            continue;
        }

        int idx = -1, minLevel = LEVELS + 1;
        for (int i = front; i < rear; i++) {
            if (processes[queue[i]].queueLevel < minLevel) {
                minLevel = processes[queue[i]].queueLevel;
                idx = queue[i];
                front = i; 
                break;
            }
        }

        if (!processes[idx].started) {
            processes[idx].response = time - processes[idx].arrival;
            processes[idx].started = true;
        }

        int run = processes[idx].remaining < quantum[processes[idx].queueLevel] ? processes[idx].remaining : quantum[processes[idx].queueLevel];
        if (run > allotment[processes[idx].queueLevel] - processes[idx].allotmentUsed) {
            run = allotment[processes[idx].queueLevel] - processes[idx].allotmentUsed;
        }

        time += run;
        processes[idx].remaining -= run;
        processes[idx].allotmentUsed += run;

        for (int i = 0; i < n; i++) {
            if (processes[i].arrival <= time && processes[i].remaining > 0) {
                bool alreadyInQueue = false;
                for (int j = front; j < rear; j++) {
                    if (queue[j] == i) {
                        alreadyInQueue = true;
                        break;
                    }
                }
                if (!alreadyInQueue) {
                    queue[rear++] = i;
                }
            }
        }

        if (processes[idx].remaining == 0) {
            processes[idx].completion = time;
            processes[idx].turnaround = processes[idx].completion - processes[idx].arrival;
            completed++;
        } else {
            if (processes[idx].allotmentUsed == allotment[processes[idx].queueLevel] && processes[idx].queueLevel < LEVELS - 1) {
                processes[idx].queueLevel++;
                processes[idx].allotmentUsed = 0;
            }
            queue[rear++] = idx;
        }

        timeline[count] = processes[idx].pid;
        levelLog[count] = processes[idx].queueLevel;
        times[count + 1] = time;
        count++;

        front++;
    }

    float avgTAT = 0, avgRT = 0;
    for (int i = 0; i < n; i++) {
        avgTAT += processes[i].turnaround;
        avgRT += processes[i].response;
    }
    avgTAT /= n; avgRT /= n;

    printGantt(timeline, times, levelLog, count);
    printTable(avgTAT, avgRT);
}

void clearScreen() { system(CLEAR); }

int main() {
    int choice, inputType;
    do {
        clearScreen();
        printf("===== CPU Scheduler =====\n");
        printf("1. Manual Input\n2. Random Input\nChoice: ");
        scanf("%d", &inputType);
        if (inputType == 1) inputProcesses();
        else randomProcesses();

        do {
        	printf("\n");
        	printf("CHOOSE WHAT ALGORITHM TO PERFORM");
            printf("\n[1] FCFS\n[2] SJF\n[3] SRTF\n[4] Round Robin\n[5] MLFQ\n[0] Exit\nChoice: ");
            scanf("%d", &choice);
            clearScreen();
            switch (choice) {
                case 1: FCFS(); break;
                case 2: SJF(); break;
                case 3: SRTF(); break;
                case 4: RoundRobin(); break;
                case 5: MLFQ(); break;
                case 0: break;
                default: printf("Invalid!\n");
            }
            if (choice != 0) {
                printf("\nPress ENTER to continue...");
                getchar(); getchar();
                clearScreen();
            }
        } while (choice != 0);

    } while (choice != 0);

    return 0;
}


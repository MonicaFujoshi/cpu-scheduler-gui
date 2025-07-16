**# cpu-scheduler-gui
Python GUI tool for simulating CPU scheduling algorithms like FCFS, SJF, SRTF, Round Robin, and MLFQ with Gantt chart visualization.**

# CPU Scheduler Simulator (Python + Tkinter)

A graphical simulation of CPU scheduling algorithms implemented using Python's `tkinter` GUI toolkit. This tool allows users to visualize, compare, and analyze various CPU scheduling techniques like **FCFS**, **SJF**, **SRTF**, **Round Robin**, and **MLFQ**.

##  Features

**Supports 5 Scheduling Algorithms:**
FCFS, SJF (Non-preemptive), SRTF (Preemptive), Round Robin, MLFQ

**Input Options:**
Manual input or randomized process generation

**Output Includes:**
Gantt chart (with queue level in MLFQ)
Detailed process table (PID, AT, BT, CT, TAT, RT)
Average Turnaround Time (TAT) and Response Time (RT)

**Algorithm Behaviors:**
Round Robin: Custom time slice
MLFQ: Multiple levels, custom quantum & allotments, dynamic demotion

**Utility Features:**
Cross-platform screen clearing
Reset function for process reuse
Menu-driven, user-friendly interface

**Purpose:**
Ideal for simulating, testing, and comparing CPU scheduling algorithms in an educational or project setting.

## Algorithms Overview
Algorithm	Description
FCFS	Processes are scheduled in order of arrival.
SJF	Non-preemptive. Selects process with the shortest burst time available.
SRTF	Preemptive version of SJF. Picks the process with the shortest remaining time.
Round Robin	Processes get equal CPU time (quantum). Useful for time-sharing systems.
MLFQ	Multi-level queues with decreasing priorities. Each queue uses different quantum.

## Sample Input

P1 -> Arrival: 1, Burst: 9
P2 -> Arrival: 4, Burst: 5
P3 -> Arrival: 9, Burst: 2
P4 -> Arrival: 8, Burst: 1
P5 -> Arrival: 2, Burst: 3

## Expected Output

**FCFS (First-Come, First-Served)**
PID	AT	BT	CT	TAT	RT
P1	1 	9 	10	9 	0
P2	4 	5 	18 	14 	9
P3	9 	2 	21 	12 	10
P4	8 	1 	19 	11 	10
P5	2 	3 	13 	11 	8
Average TAT: 11.40
Average RT : 7.40

**SJF (Shortest Job First)**
PID	AT	BT	CT	TAT	RT
P1	1 	9 	10	9 	0
P2	4 	5 	21 	17 	12
P3	9 	2 	13 	4 	2
P4	8 	1   11 	3 	2
P5	2 	3 	16 	14 	11
Average TAT: 9.40
Average RT : 5.40

**SRTF (Shortest Remaining Time First)**
PID	AT	BT	CT	TAT	RT
P1	1 	9 	21	20	0
P2	4 	5 	11 	7 	1
P3	9 	2 	13 	4 	2
P4	8 	1 	9 	1 	0
P5	2 	3 	5 	3 	0
Average TAT: 7.00
Average RT : 0.60

**⏱️ Round Robin (Quantum = 3)**
PID	AT	BT	CT	TAT	RT
P1	1 	9 	21	20	0
P2	4	  5 	18	14	3
P3	9 	2 	16	7 	5
P4	8 	1 	14	6	  5
P5	2 	3 	7	  5	  2
Average TAT: 10.40  
Average RT : 3.00

**MLFQ (Quanta: Q1=3,5 , Q2=4,5 , Q3=5,5 , Q4=5,5)**
PID	AT	BT	CT	TAT	RT
P2	1	6	19	18	0
P4	1	8	24	23	3
P0	2	8	29	27	5
P1	2	4	30	28	8
P3	2	9	36	34	11
Average TAT: 26.0
Average RT: 5.4

## Screenshots
**FCFS** 
![FCFS Example](FCFS.png)
**SJF**
![SJF Example](SJF.png)
**SRTF**
![SRTF Example](SRTF.png)
**RR**
![Round Robin Example](RR.png)
**MLFQ**
![MLFQ Example](MLFQ.png)



## Known Issues / Limitations

-No GUI (command-line only).
-SJF assumes non-preemptive implementation.
-No priority-based scheduling implemented.
-MLFQ is limited to 4 levels (can be expanded).
-No persistent logging of results (output only to screen).

## Team Members & Contributions

**Name	                           Role**
Martina Calledo      	Round Robin/MLFQ Logic
Jasmine Omandam 	  	FCFS/SJF/SRTF Logic


## Requirements

- Dev C++ (IDE)


## Run the app:

compile cpu_scheduler.cpp

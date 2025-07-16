**# cpu-scheduler-gui
Python GUI tool for simulating CPU scheduling algorithms like FCFS, SJF, SRTF, Round Robin, and MLFQ with Gantt chart visualization.**

# CPU Scheduler Simulator (Python + Tkinter)

A graphical simulation of CPU scheduling algorithms implemented using Python's `tkinter` GUI toolkit. This tool allows users to visualize, compare, and analyze various CPU scheduling techniques like **FCFS**, **SJF**, **SRTF**, **Round Robin**, and **MLFQ**.

##  Features

- Intuitive graphical user interface (GUI)
- Manual or random process input
- Gantt chart visualization
- Performance metrics:
  - Completion Time (CT)
  - Turnaround Time (TAT)
  - Response Time (RT)
  - Averages of TAT and RT
- Supports 5 algorithms:
  - First-Come First-Served (FCFS)
  - Shortest Job First (SJF)
  - Shortest Remaining Time First (SRTF)
  - Round Robin (with customizable quantum)
  - Multi-Level Feedback Queue (MLFQ)

### Algorithms Overview
Algorithm	Description
FCFS	Processes are scheduled in order of arrival.
SJF	Non-preemptive. Selects process with the shortest burst time available.
SRTF	Preemptive version of SJF. Picks the process with the shortest remaining time.
Round Robin	Processes get equal CPU time (quantum). Useful for time-sharing systems.
MLFQ	Multi-level queues with decreasing priorities. Each queue uses different quantum.

##### Screenshots
FCFS



#### Known Issues / Limitations
-GUI may lag with more than ~20 processes.
-No process priority support (for now).
-MLFQ fixed to 4 levels only.
-No persistent save/load for process sets.


##### Team Members & Contributions
**Name	                           Role**
Martina Calledo      	UI Design,Round Robin/MLFQ Algorithms
Jasmine Omandam 	  	UI Design, FCFS/SJF Logic


#### 🔧 Requirements

- Python 3.x
- `tkinter` (usually included with Python)

### ▶️ Run the app:

```bash
python scheduler_gui.py

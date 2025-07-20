import tkinter as tk
from tkinter import ttk, messagebox, simpledialog
import random
import time as systime

class Process:
    def __init__(self, pid, arrival, burst):
        self.pid = pid
        self.arrival = arrival
        self.burst = burst
        self.remaining = burst
        self.start_time = None
        self.completion = 0
        self.turnaround = 0
        self.response = None

class SchedulerGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("CPU Scheduler Simulator")
        self.root.geometry("1200x800")
        self.root.configure(bg="#d0e7f9")

        self.font = ("Times New Roman", 12, "bold")
        self.processes = []

        self.setup_widgets()

    def setup_widgets(self):
        frame = ttk.LabelFrame(self.root, text="Input Configuration", padding=10)
        frame.pack(padx=10, pady=10, fill="x")

        ttk.Label(frame, text="Number of Processes:", font=self.font).grid(row=0, column=0, sticky="w")
        self.num_entry = ttk.Entry(frame, width=5, font=self.font)
        self.num_entry.grid(row=0, column=1, sticky="w", padx=5)

        ttk.Button(frame, text="Manual Input", command=self.manual_input).grid(row=0, column=2, padx=5)
        ttk.Button(frame, text="Random Input", command=self.random_input).grid(row=0, column=3, padx=5)

        ttk.Label(frame, text="Select Algorithm:", font=self.font).grid(row=1, column=0, sticky="w", pady=5)
        self.algo_var = tk.StringVar(value="FCFS")
        algo_options = ["FCFS", "SJF", "SRTF", "Round Robin", "MLFQ"]
        self.algo_menu = ttk.OptionMenu(frame, self.algo_var, "FCFS", *algo_options)
        self.algo_menu.grid(row=1, column=1, sticky="w")

        self.quantum_label = ttk.Label(frame, text="Quantum (RR):", font=self.font)
        self.quantum_label.grid(row=2, column=0, sticky="w")
        self.quantum_entry = ttk.Entry(frame, width=5, font=self.font)
        self.quantum_entry.grid(row=2, column=1, sticky="w", padx=5)

        self.ts_labels = []
        self.ts_entries = []
        self.allot_labels = []
        self.allot_entries = []

        for i in range(4):
            ts_label = ttk.Label(frame, text=f"Q{i} Time Slice:", font=self.font)
            ts_entry = ttk.Entry(frame, width=5, font=self.font)
            ts_label.grid(row=3 + i, column=0, sticky="w")
            ts_entry.grid(row=3 + i, column=1, sticky="w", padx=5)
            self.ts_labels.append(ts_label)
            self.ts_entries.append(ts_entry)

            allot_label = ttk.Label(frame, text=f"Q{i} Allotment:", font=self.font)
            allot_entry = ttk.Entry(frame, width=5, font=self.font)
            allot_label.grid(row=3 + i, column=2, sticky="w")
            allot_entry.grid(row=3 + i, column=3, sticky="w", padx=5)
            self.allot_labels.append(allot_label)
            self.allot_entries.append(allot_entry)

        ttk.Button(frame, text="Run Scheduler", command=self.run_scheduler).grid(row=10, column=0, columnspan=15, pady=25)
        ttk.Button(frame, text="Clear", command=self.clear_all).grid(row=12, column=0, columnspan=10, pady=5)


        self.progress = ttk.Progressbar(self.root, orient="horizontal", length=500, mode="determinate")
        self.progress.pack(pady=5)

        self.output = tk.Text(self.root, height=10, font=self.font, wrap="word", bg="#ffe6f0")
        self.output.pack(fill="x", padx=10, pady=5)

        gantt_frame = ttk.Frame(self.root)
        gantt_frame.pack(fill="both", expand=True, padx=10, pady=10)

        self.gantt_canvas = tk.Canvas(gantt_frame, height=250, bg="white")
        self.h_scroll = ttk.Scrollbar(gantt_frame, orient="horizontal", command=self.gantt_canvas.xview)
        self.v_scroll = ttk.Scrollbar(gantt_frame, orient="vertical", command=self.gantt_canvas.yview)
        self.gantt_canvas.configure(xscrollcommand=self.h_scroll.set, yscrollcommand=self.v_scroll.set)

        self.gantt_canvas.grid(row=0, column=0, sticky="nsew")
        self.h_scroll.grid(row=1, column=0, sticky="ew")
        self.v_scroll.grid(row=0, column=1, sticky="ns")

        gantt_frame.columnconfigure(0, weight=1)
        gantt_frame.rowconfigure(0, weight=1)
    def clear_all(self):
        self.processes = []
        self.output.delete(1.0, tk.END)
        self.progress["value"] = 0
        self.gantt_canvas.delete("all")
        self.num_entry.delete(0, tk.END)
        self.quantum_entry.delete(0, tk.END)
        for entry in self.ts_entries + self.allot_entries:
            entry.delete(0, tk.END)

    def manual_input(self):
        try:
            n = int(self.num_entry.get())
        except:
            messagebox.showerror("Error", "Enter a valid number of processes")
            return
        self.processes = []
        for i in range(n):
            arrival = simpledialog.askinteger("Input", f"Arrival Time of P{i}:")
            burst = simpledialog.askinteger("Input", f"Burst Time of P{i}:")
            self.processes.append(Process(i, arrival, burst))
        self.display_processes()

    def random_input(self):
        try:
            n = int(self.num_entry.get())
        except:
            messagebox.showerror("Error", "Enter a valid number of processes")
            return
        self.processes = []
        for i in range(n):
            arrival = random.randint(0, 5)
            burst = random.randint(1, 10)
            self.processes.append(Process(i, arrival, burst))
        self.display_processes()

    def display_processes(self):
        self.output.delete(1.0, tk.END)
        self.output.insert(tk.END, "Processes Entered:\n\n")
        for p in self.processes:
            self.output.insert(tk.END, f"P{p.pid} → Arrival: {p.arrival}, Burst: {p.burst}\n")

    def run_scheduler(self):
        algo = self.algo_var.get()
        for p in self.processes:
            p.remaining = p.burst
            p.start_time = None
            p.completion = 0

        self.progress["value"] = 0
        self.root.update_idletasks()

        if algo == "FCFS":
            self.fcfs()
        elif algo == "SJF":
            self.sjf()
        elif algo == "SRTF":
            self.srtf()
        elif algo == "Round Robin":
            try:
                q = int(self.quantum_entry.get())
            except:
                messagebox.showerror("Error", "Enter valid quantum")
                return
            self.round_robin(q)
        elif algo == "MLFQ":
            try:
                time_slices = [int(e.get()) for e in self.ts_entries]
                allotments = [int(e.get()) for e in self.allot_entries]
            except:
                messagebox.showerror("Error", "Please enter valid MLFQ time slices and allotments")
                return
            self.mlfq(time_slices, allotments)

        self.progress["value"] = 100
        self.root.update_idletasks()

    def draw_gantt(self, schedule):
        self.gantt_canvas.delete("all")
        if not schedule:
            return

        total_time = schedule[-1][2]
        width = max(2000, total_time * 100)
        height = 2500
        x_scale = width / total_time
        color_map = {}
        color_palette = [
            "#66b3ff", "#99ff99", "#ffcc99", "#ff9999", "#c2c2f0",
            "#ffb3e6", "#aaffc3", "#ffd8b1", "#e6beff", "#ff6666"
        ]

        def get_color(pid):
            if "(" in pid:
                base_pid = pid.split("(")[0]
            else:
                base_pid = pid
            if base_pid not in color_map:
                color_map[base_pid] = color_palette[len(color_map) % len(color_palette)]
            return color_map[base_pid]

        for i, (pid, start, end) in enumerate(schedule):
            x1 = start * x_scale
            x2 = end * x_scale
            color = get_color(pid)
            self.gantt_canvas.create_rectangle(x1, 50, x2, 150, fill=color, outline="black")
            self.gantt_canvas.create_text((x1 + x2) / 2, 100, text=str(pid), font=self.font)
            self.gantt_canvas.create_text(x1, 160, text=str(start), font=("Arial", 10), anchor="n")
            if i == len(schedule) - 1 or schedule[i + 1][1] != end:
                self.gantt_canvas.create_text(x2, 160, text=str(end), font=("Arial", 10), anchor="n")

        self.gantt_canvas.config(scrollregion=(0, 0, width + 20, height))

    def calculate_metrics(self):
        total_tat = total_rt = total_wt = 0
        self.output.insert(tk.END, "\n\nPID\tAT\tBT\tCT\tTAT\tRT\tWT\n")
        for p in self.processes:
            p.turnaround = p.completion - p.arrival
            p.response = p.start_time - p.arrival
            wt = p.turnaround - p.burst
            total_tat += p.turnaround
            total_rt += p.response
            total_wt += wt
            self.output.insert(tk.END, f"{p.pid}\t{p.arrival}\t{p.burst}\t{p.completion}\t{p.turnaround}\t{p.response}\t{wt}\n")
        n = len(self.processes)
        self.output.insert(tk.END, f"\nAverage Turnaround Time: {total_tat / n:.2f}\n")
        self.output.insert(tk.END, f"Average Response Time: {total_rt / n:.2f}\n")
        self.output.insert(tk.END, f"Average Waiting Time: {total_wt / n:.2f}\n")

    def fcfs(self):
        time = 0
        schedule = []
        for p in sorted(self.processes, key=lambda x: x.arrival):
            if time < p.arrival:
                time = p.arrival
            p.start_time = time
            time += p.burst
            p.completion = time
            schedule.append((f"P{p.pid}", p.start_time, p.completion))
        self.draw_gantt(schedule)
        self.calculate_metrics()

    def sjf(self):
        time = 0
        completed = []
        schedule = []
        while len(completed) < len(self.processes):
            ready = [p for p in self.processes if p.arrival <= time and p not in completed]
            if ready:
                p = min(ready, key=lambda x: x.burst)
                p.start_time = time
                time += p.burst
                p.completion = time
                completed.append(p)
                schedule.append((f"P{p.pid}", p.start_time, p.completion))
            else:
                time += 1
        self.draw_gantt(schedule)
        self.calculate_metrics()

    def srtf(self):
        time = 0
        completed = 0
        schedule = []
        current_process = None
        start_time = None
        n = len(self.processes)
        # Reset remaining time for all processes
        for p in self.processes:
            p.remaining = p.burst
            p.start_time = None
            p.completion = None

        while completed != n:
            # Find all ready processes
            ready = [p for p in self.processes if p.arrival <= time and p.remaining > 0]
            if ready:
                # Pick process with shortest remaining time
                p = min(ready, key=lambda x: x.remaining)
                # If process changes, record the previous one
                if p != current_process:
                    if current_process is not None:
                        schedule.append((f"P{current_process.pid}", start_time, time))
                    current_process = p
                    start_time = time
                    if p.start_time is None:
                        p.start_time = time
                # Run process for 1 time unit
                p.remaining -= 1
                if p.remaining == 0:
                    p.completion = time + 1
                    completed += 1
                time += 1
            else:
                # CPU is idle
                if current_process is not None:
                    schedule.append((f"P{current_process.pid}", start_time, time))
                    current_process = None
                # Record idle time
                idle_start = time
                while True:
                    time += 1
                    ready = [p for p in self.processes if p.arrival <= time and p.remaining > 0]
                    if ready or completed == n:
                        break
                schedule.append(("IDLE", idle_start, time))
                # After idle, will pick up with new process
                current_process = None
                start_time = time
        # Add the last running process if any
        if current_process is not None:
            schedule.append((f"P{current_process.pid}", start_time, time))
        self.draw_gantt(schedule)
        self.calculate_metrics()

    def round_robin(self, quantum):
        time = 0
        schedule = []
        processes = sorted(self.processes, key=lambda x: x.arrival)
        queue = []
        n = len(processes)
        complete = 0
        seen = set()

        while complete < n:
            for p in processes:
                if p.arrival <= time and p not in queue and p.remaining > 0 and p.pid not in seen:
                    queue.append(p)
                    seen.add(p.pid)

            if not queue:
                future_arrivals = [p.arrival for p in processes if p.remaining > 0 and p.arrival > time]
                if future_arrivals:
                    time = min(future_arrivals)
                    continue
                else:
                    break

            p = queue.pop(0)

            if p.start_time is None:
                p.start_time = time

            exec_time = min(quantum, p.remaining)
            schedule.append((f"P{p.pid}", time, time + exec_time))
            time += exec_time
            p.remaining -= exec_time

            for new_p in processes:
                if new_p.arrival > time - exec_time and new_p.arrival <= time and new_p.remaining > 0 and new_p.pid not in seen:
                    queue.append(new_p)
                    seen.add(new_p.pid)

            if p.remaining > 0:
                queue.append(p)
            else:
                p.completion = time
                complete += 1

        self.draw_gantt(schedule)
        self.calculate_metrics()

    def mlfq(self, time_slices, allotments):
        time = 0
        schedule = []
        level_count = len(time_slices)
        queues = [[] for _ in range(level_count)]
        processes = sorted(self.processes, key=lambda p: p.arrival)
        arrived = set()
        allotment_used = {p.pid: 0 for p in processes}
        level_map = {p.pid: 0 for p in processes}  # Tracks current level per process

        while any(p.remaining > 0 for p in processes):
            # Enqueue newly arrived processes into Q0
            for p in processes:
                if p.arrival <= time and p.pid not in arrived:
                    queues[0].append(p)
                    level_map[p.pid] = 0
                    allotment_used[p.pid] = 0
                    arrived.add(p.pid)

            executed = False
            for level in range(level_count):
                if queues[level]:
                    p = queues[level].pop(0)
                    if p.start_time is None:
                        p.start_time = time

                    ts = time_slices[level]
                    allot = allotments[level]

                    exec_time = min(ts, p.remaining, allot - allotment_used[p.pid])
                    start = time
                    time += exec_time
                    p.remaining -= exec_time
                    allotment_used[p.pid] += exec_time
                    schedule.append((f"P{p.pid}(Q{level})", start, time))

                    # Check for any newly arrived processes during this time window
                    for np in processes:
                        if np.arrival > start and np.arrival <= time and np.pid not in arrived:
                            queues[0].append(np)
                            level_map[np.pid] = 0
                            allotment_used[np.pid] = 0
                            arrived.add(np.pid)

                    if p.remaining == 0:
                        p.completion = time
                    else:
                        if allotment_used[p.pid] >= allot:
                            if level + 1 < level_count:
                                queues[level + 1].append(p)
                                level_map[p.pid] = level + 1
                                allotment_used[p.pid] = 0
                            else:
                                queues[level].append(p)
                                allotment_used[p.pid] = 0
                        else:
                            queues[level].append(p)

                    executed = True
                    break

            if not executed:
                time += 1

        self.draw_gantt(schedule)
        self.calculate_metrics()


if __name__ == "__main__":
    root = tk.Tk()
    app = SchedulerGUI(root)
    root.mainloop()

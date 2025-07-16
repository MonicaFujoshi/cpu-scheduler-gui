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
        self.algo_menu = ttk.OptionMenu(frame, self.algo_var, "FCFS", *algo_options)  # <-- FIXED LINE
        self.algo_menu.grid(row=1, column=1, sticky="w")

        self.quantum_label = ttk.Label(frame, text="Quantum (RR):", font=self.font)
        self.quantum_label.grid(row=2, column=0, sticky="w")
        self.quantum_entry = ttk.Entry(frame, width=5, font=self.font)
        self.quantum_entry.grid(row=2, column=1, sticky="w", padx=5)

        self.quanta_labels = []
        self.quanta_entries = []
        for i in range(4):
            l = ttk.Label(frame, text=f"Q{i} Quantum:", font=self.font)
            e = ttk.Entry(frame, width=5, font=self.font)
            l.grid(row=3 + i, column=0, sticky="w")
            e.grid(row=3 + i, column=1, sticky="w", padx=5)
            self.quanta_labels.append(l)
            self.quanta_entries.append(e)

        ttk.Button(frame, text="Run Scheduler", command=self.run_scheduler).grid(row=8, column=0, columnspan=2, pady=10)

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
            quanta = []
            for e in self.quanta_entries:
                try:
                    quanta.append(int(e.get()))
                except:
                    messagebox.showerror("Error", "Fill all MLFQ quanta")
                    return
            self.mlfq(quanta)

        self.progress["value"] = 100
        self.root.update_idletasks()

    def draw_gantt(self, schedule):
        self.gantt_canvas.delete("all")
        total_time = schedule[-1][2]
        width = max(1200, total_time * 50)
        height = 250
        x = 0
        colors = ["#ffb3ba", "#bae1ff", "#baffc9", "#ffffba", "#ffdfba", "#c2c2f0"]

        for idx, s in enumerate(schedule):
            pid, start, end = s
            w = (end - start) * (width / total_time)
            color = colors[idx % len(colors)]
            self.gantt_canvas.create_rectangle(x, 50, x + w, 150, fill=color, outline="black")
            self.gantt_canvas.create_text(x + w / 2, 100, text=str(pid), font=self.font)
            x += w

        tick_interval = 1
        if total_time > 50:
            tick_interval = 2
        if total_time > 100:
            tick_interval = 5

        for t in range(0, total_time + 1, tick_interval):
            tick_x = (t / total_time) * width
            self.gantt_canvas.create_line(tick_x, 150, tick_x, 160, fill="black")
            self.gantt_canvas.create_text(tick_x, 170, text=str(t), font=("Arial", 10))

        self.gantt_canvas.config(scrollregion=(0, 0, width, height))

    def calculate_metrics(self):
        total_tat = total_rt = 0
        self.output.insert(tk.END, "\n\nPID\tAT\tBT\tCT\tTAT\tRT\n")
        for p in self.processes:
            p.turnaround = p.completion - p.arrival
            p.response = p.start_time - p.arrival
            total_tat += p.turnaround
            total_rt += p.response
            self.output.insert(tk.END, f"{p.pid}\t{p.arrival}\t{p.burst}\t{p.completion}\t{p.turnaround}\t{p.response}\n")
        n = len(self.processes)
        self.output.insert(tk.END, f"\nAverage Turnaround Time: {total_tat / n:.2f}\n")
        self.output.insert(tk.END, f"Average Response Time: {total_rt / n:.2f}\n")

    def fcfs(self):
        time = 0
        schedule = []
        total = len(self.processes)
        for idx, p in enumerate(sorted(self.processes, key=lambda x: x.arrival)):
            if time < p.arrival:
                time = p.arrival
            p.start_time = time
            time += p.burst
            p.completion = time
            schedule.append((f"P{p.pid}", p.start_time, p.completion))
            self.progress["value"] = ((idx + 1) / total) * 100
            self.root.update_idletasks()
            systime.sleep(0.3)
        self.draw_gantt(schedule)
        self.calculate_metrics()

    def sjf(self):
        time = 0
        completed = []
        schedule = []
        total = len(self.processes)
        while len(completed) < total:
            ready = [p for p in self.processes if p.arrival <= time and p not in completed]
            if ready:
                p = min(ready, key=lambda x: x.burst)
                p.start_time = time
                time += p.burst
                p.completion = time
                completed.append(p)
                schedule.append((f"P{p.pid}", p.start_time, p.completion))
                self.progress["value"] = (len(completed) / total) * 100
                self.root.update_idletasks()
                systime.sleep(0.3)
            else:
                time += 1
        self.draw_gantt(schedule)
        self.calculate_metrics()

    def srtf(self):
        time = 0
        completed = 0
        schedule = []
        last_pid = None
        total = len(self.processes)
        while completed != total:
            ready = [p for p in self.processes if p.arrival <= time and p.remaining > 0]
            if ready:
                p = min(ready, key=lambda x: x.remaining)
                if p.start_time is None:
                    p.start_time = time
                if p.pid != last_pid:
                    schedule.append((f"P{p.pid}", time))
                    last_pid = p.pid
                p.remaining -= 1
                if p.remaining == 0:
                    p.completion = time + 1
                    completed += 1
                    self.progress["value"] = (completed / total) * 100
                    self.root.update_idletasks()
                time += 1
            else:
                time += 1
        formatted = []
        for i in range(len(schedule)):
            pid, start = schedule[i]
            end = schedule[i + 1][1] if i < len(schedule) - 1 else time
            formatted.append((pid, start, end))
        self.draw_gantt(formatted)
        self.calculate_metrics()

    def round_robin(self, quantum):
        time = 0
        queue = []
        processes = sorted(self.processes, key=lambda x: x.arrival)
        queue.append(processes[0])
        arrived = processes[1:]
        schedule = []
        total = len(self.processes)
        done = 0
        while queue:
            p = queue.pop(0)
            if p.start_time is None:
                p.start_time = max(time, p.arrival)
                if time < p.arrival:
                    time = p.arrival
            exec_time = min(quantum, p.remaining)
            start = time
            time += exec_time
            p.remaining -= exec_time
            schedule.append((f"P{p.pid}", start, time))
            for np in arrived[:]:
                if np.arrival <= time:
                    queue.append(np)
                    arrived.remove(np)
            if p.remaining > 0:
                queue.append(p)
            else:
                p.completion = time
                done += 1
                self.progress["value"] = (done / total) * 100
                self.root.update_idletasks()
            systime.sleep(0.2)
        self.draw_gantt(schedule)
        self.calculate_metrics()

    def mlfq(self, quanta):
        time = 0
        queues = [[] for _ in range(4)]
        processes = sorted(self.processes, key=lambda x: x.arrival)
        arrived = []
        schedule = []
        total = len(self.processes)
        done = 0

        while any(p.remaining > 0 for p in self.processes):
            for p in processes:
                if p.arrival <= time and p not in arrived:
                    queues[0].append(p)
                    arrived.append(p)

            for level in range(4):
                if queues[level]:
                    p = queues[level].pop(0)
                    if p.start_time is None:
                        p.start_time = time

                    exec_time = min(p.remaining, quanta[level])
                    start = time
                    time += exec_time
                    p.remaining -= exec_time
                    schedule.append((f"P{p.pid}(Q{level})", start, time))

                    if p.remaining > 0:
                        if level + 1 < 4:
                            queues[level + 1].append(p)
                        else:
                            queues[level].append(p)
                    else:
                        p.completion = time
                        done += 1
                        self.progress["value"] = (done / total) * 100
                        self.root.update_idletasks()
                    systime.sleep(0.2)
                    break
            else:
                time += 1

        self.draw_gantt(schedule)
        self.calculate_metrics()


if __name__ == "__main__":
    root = tk.Tk()
    app = SchedulerGUI(root)
    root.mainloop()

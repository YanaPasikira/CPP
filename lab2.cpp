#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

struct Process {
    int id;
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int priority;
    int startTime = -1;
    int finishTime = 0;
    int waitingTime = 0;
};

vector<Process> generateProcesses(int n) {
    vector<Process> processes;
    for (int i = 0; i < n; i++) {
        Process p;
        p.id = i + 1;
        p.arrivalTime = rand() % 10;
        p.burstTime = 1 + rand() % 10;
        p.remainingTime = p.burstTime;
        p.priority = 1 + rand() % 5;
        processes.push_back(p);
    }
    return processes;
}

void FCFS(vector<Process> processes) {
    sort(processes.begin(), processes.end(),
         [](Process a, Process b) { return a.arrivalTime < b.arrivalTime; });

    int time = 0;
    double totalWait = 0;

    for (auto &p : processes) {
        if (time < p.arrivalTime) time = p.arrivalTime;
        p.startTime = time;
        p.finishTime = time + p.burstTime;
        p.waitingTime = p.startTime - p.arrivalTime;
        time = p.finishTime;
        totalWait += p.waitingTime;

        cout << p.id << " " << p.startTime << " " << p.finishTime << " " << p.waitingTime << endl;
    }

    cout << totalWait / processes.size() << endl;
}

void RoundRobin(vector<Process> processes, int quantum) {
    queue<int> q;
    int time = 0;
    int completed = 0;
    double totalWait = 0;

    vector<bool> inQueue(processes.size(), false);

    while (completed < processes.size()) {
        for (int i = 0; i < processes.size(); i++) {
            if (processes[i].arrivalTime <= time &&
                processes[i].remainingTime > 0 &&
                !inQueue[i]) {
                q.push(i);
                inQueue[i] = true;
            }
        }

        if (q.empty()) {
            time++;
            continue;
        }

        int idx = q.front();
        q.pop();

        Process &p = processes[idx];

        if (p.startTime == -1) p.startTime = time;

        int execTime = min(quantum, p.remainingTime);
        time += execTime;
        p.remainingTime -= execTime;

        cout << p.id << " " << p.remainingTime << endl;

        if (p.remainingTime > 0) {
            p.priority++;
            q.push(idx);
        } else {
            p.finishTime = time;
            p.waitingTime = p.finishTime - p.arrivalTime - p.burstTime;
            totalWait += p.waitingTime;
            completed++;
        }
    }

    cout << totalWait / processes.size() << endl;
}

void PriorityScheduling(vector<Process> processes) {
    sort(processes.begin(), processes.end(),
         [](Process a, Process b) { return a.priority > b.priority; });

    int time = 0;
    double totalWait = 0;

    for (auto &p : processes) {
        if (time < p.arrivalTime) time = p.arrivalTime;
        p.startTime = time;
        p.finishTime = time + p.burstTime;
        p.waitingTime = p.startTime - p.arrivalTime;
        time = p.finishTime;
        totalWait += p.waitingTime;

        cout << p.id << " " << p.burstTime << " " << p.priority << endl;
    }

    cout << totalWait / processes.size() << endl;
}

int main() {
    srand(time(nullptr));

    int n = 5;
    int quantum = 3;

    vector<Process> processes = generateProcesses(n);

    for (auto &p : processes) {
        cout << p.id << " " << p.arrivalTime << " " << p.burstTime << " " << p.priority << endl;
    }

    FCFS(processes);
    RoundRobin(processes, quantum);
    PriorityScheduling(processes);

    return 0;
}


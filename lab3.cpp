#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
#include <ctime>

using namespace std;

struct Process {
    int id;
    int arrival;
    int burst;
    int remaining;
    int priority;
    int start = -1;
    int finish = 0;
    int wait = 0;
};

vector<Process> generate(int n) {
    vector<Process> p;
    for (int i = 0; i < n; i++) {
        p.push_back({
            i + 1,
            rand() % 10,
            1 + rand() % 10,
            0,
            1 + rand() % 5
        });
        p.back().remaining = p.back().burst;
    }
    return p;
}

void stats(const vector<Process>& p, string name) {
    double aw = 0, at = 0;
    for (auto &x : p) {
        aw += x.wait;
        at += x.finish - x.arrival;
    }
    cout << name << setw(15)
         << aw / p.size() << setw(15)
         << at / p.size() << endl;
}

vector<Process> FCFS(vector<Process> p) {
    sort(p.begin(), p.end(), [](auto a, auto b){ return a.arrival < b.arrival; });
    int t = 0;
    for (auto &x : p) {
        if (t < x.arrival) t = x.arrival;
        x.start = t;
        x.finish = t + x.burst;
        x.wait = x.start - x.arrival;
        t = x.finish;
    }
    return p;
}

vector<Process> SJF(vector<Process> p) {
    int t = 0, done = 0;
    vector<bool> used(p.size(), false);
    while (done < p.size()) {
        int idx = -1;
        for (int i = 0; i < p.size(); i++)
            if (!used[i] && p[i].arrival <= t &&
                (idx == -1 || p[i].burst < p[idx].burst))
                idx = i;
        if (idx == -1) {
            t++;
            continue;
        }
        p[idx].start = t;
        p[idx].finish = t + p[idx].burst;
        p[idx].wait = p[idx].start - p[idx].arrival;
        t = p[idx].finish;
        used[idx] = true;
        done++;
    }
    return p;
}

vector<Process> PriorityAging(vector<Process> p) {
    int t = 0, done = 0;
    vector<bool> used(p.size(), false);
    while (done < p.size()) {
        int idx = -1;
        for (int i = 0; i < p.size(); i++) {
            if (!used[i] && p[i].arrival <= t) {
                if (idx == -1 || p[i].priority > p[idx].priority)
                    idx = i;
                p[i].priority++;
            }
        }
        if (idx == -1) {
            t++;
            continue;
        }
        p[idx].start = t;
        p[idx].finish = t + p[idx].burst;
        p[idx].wait = p[idx].start - p[idx].arrival;
        t = p[idx].finish;
        used[idx] = true;
        done++;
    }
    return p;
}

int main() {
    srand(time(nullptr));
    int n = 6;

    vector<Process> base = generate(n);

    cout << "ID  Arr  Burst  Prio\n";
    for (auto &p : base)
        cout << p.id << "   " << p.arrival << "     " << p.burst << "      " << p.priority << endl;

    auto fcfs = FCFS(base);
    auto sjf = SJF(base);
    auto prio = PriorityAging(base);

    cout << "\nAlgorithm      AvgWait        AvgTurn\n";
    stats(fcfs, "FCFS");
    stats(sjf, "SJF");
    stats(prio, "Priority+Aging");

    return 0;
}


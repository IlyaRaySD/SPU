#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

// Task struct
struct Task {
    int id;
    int duration;
    vector<int> next_tasks;
    vector<int> back_tasks;
    int earlyStart;
    int earlyFinish;
    int lateStart;
    int lateFinish;
    int slack;
};

class NetworkPlanning {
private:
    vector<Task> tasks;
    vector<int> earliestFinishTimes;
    int numTasks;
    int criticalPathLength;

public:
    NetworkPlanning(int num) : numTasks(num), earliestFinishTimes(num, 0), criticalPathLength(0) {
        tasks.resize(num);
        for (int i = 0; i < num; ++i) {
            tasks[i].id = i;
        }
    }

    // dependencies calculation method
    void setDependencies(const vector<vector<int>>& adjMatrix) {
        for (int i = 0; i < numTasks; ++i) {
            for (int j = 0; j < numTasks; ++j) {
                if (adjMatrix[i][j] == 1) {
                    tasks[j].next_tasks.push_back(i);
                    tasks[i].back_tasks.push_back(j);
                }
            }
        }
    }

    // set durations method
    void setDurations(const vector<int>& durations) {
        for (int i = 0; i < numTasks; ++i) {
            tasks[i].duration = durations[i];
        }
    }

    void calculateEarliestTimes() {
        for (auto& task : tasks) {
            int earliest = 0;
            for (int dep : task.next_tasks) {
                earliest = max(earliest, earliestFinishTimes[dep]);
            }
            task.earlyStart = earliest;
            task.earlyFinish = earliest + task.duration;
            earliestFinishTimes[task.id] = task.earlyFinish;
        }
    }

    void calculateLatestTimes() {
        int projectDuration = earliestFinishTimes.back();
        for (int i = numTasks - 1; i >= 0; --i) {
           
            Task& task = tasks[i];
            task.lateFinish = projectDuration;
            task.lateStart = projectDuration - task.duration;
            for (int dep : task.back_tasks) {
                task.lateStart = min(task.lateStart, tasks[dep].lateFinish - task.duration);
            }
            task.slack = task.lateFinish - task.earlyFinish;
        }
    }

    // checking the cycles
    bool hasCycle() {
        vector<int> color(numTasks, 0); // 0 - not visited, 1 - in process, 2 - visited
        for (int i = 0; i < numTasks; ++i) {
            if (color[i] == 0 && HasCycle(i, color)) {
                return true;
            }
        }
        return false;
    }

    bool HasCycle(int node, vector<int>& color) {
        color[node] = 1; 
        for (int dep : tasks[node].next_tasks) {
            if (color[dep] == 1) { 
                return true;
            }
            else if (color[dep] == 0 && HasCycle(dep, color)) {
                return true;
            }
        }
        color[node] = 2; 
        return false;
    }

    void calculateCriticalPath() {
        if (hasCycle()) {
            cout << "The graph contains a cycle. Cannot determine critical path.\n";
            return;
        }

        criticalPathLength = INT_MIN; 
        vector<int> criticalPathNodes;
        for (auto& task : tasks) {
            if (task.slack == 0) {
                criticalPathLength = max(criticalPathLength, task.earlyFinish);
                criticalPathNodes.push_back(task.id);
            }
        }

        if (criticalPathLength == INT_MIN) {
            cout << "\nNo critical path found.\n";
        }
        else {
            cout << "\nCritical path nodes: ";
            for (int node : criticalPathNodes) {
                cout << node << " ";
            }
            cout << endl;
        }
    }

    void printSchedule() {
        cout << "\nTask\tEarly Start\tEarly Finish\tLate Start\tLate Finish\tSlack\n";
        for (const auto& task : tasks) {
            cout << task.id << "\t" << task.earlyStart << "\t\t" << task.earlyFinish << "\t\t"
                << task.lateStart << "\t\t" << task.lateFinish << "\t\t" << task.slack << endl;
        }
        cout << "\nLength of critical path: " << criticalPathLength << endl;
    }
};

int main() {
    int numTasks;
    cout << "Enter the number of tasks: ";
    cin >> numTasks;

    vector<vector<int>> adjMatrix(numTasks, vector<int>(numTasks));
    cout << "\nEnter the matrix (only 0 or 1):\n";
    for (int i = 0; i < numTasks; ++i) {
        for (int j = 0; j < numTasks; ++j) {
            cin >> adjMatrix[i][j];
        }
    }

    vector<int> durations(numTasks);
    cout << "\nEnter the durations of tasks:\n";
    for (int i = 0; i < numTasks; ++i) {
        cin >> durations[i];
    }

    NetworkPlanning planner(numTasks);
    planner.setDependencies(adjMatrix);
    planner.setDurations(durations);

    planner.calculateEarliestTimes();
    planner.calculateLatestTimes();
    planner.calculateCriticalPath();
    planner.printSchedule();

    return 0;
}

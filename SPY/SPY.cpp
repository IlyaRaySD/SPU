#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;


struct Task {
    int id;
    int duration;
    vector<int> next_tasks;
    vector<int> back_tasks;
    int earlyStart = 0;
    int earlyFinish = 0;
    int MaxLength;
    int slack;
};

class NetworkPlanning {
private:
    vector<Task> tasks;
    int numTasks;
    int criticalPathLength;

public:
    NetworkPlanning(int num) : numTasks(num), criticalPathLength(0) {
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
        for (int i = 0; i < numTasks; ++i) {
            for (int j = 0; j < numTasks; ++j) {
                for (int dep : tasks[j].back_tasks) {
                    if (dep == i) {
                        if (tasks[i].earlyStart < tasks[j].duration + tasks[j].earlyStart)
                            tasks[i].earlyStart = tasks[j].duration + tasks[j].earlyStart;
                    }
                }
            }
        }

        for (int i = numTasks - 1; i >= 0; --i) {
            for (int j = numTasks - 1; j >= 0; --j) {
                for (int dep : tasks[j].next_tasks) {
                    if (dep == i) {
                        if (tasks[i].earlyFinish < tasks[j].duration + tasks[j].earlyFinish)
                            tasks[i].earlyFinish = tasks[j].duration + tasks[j].earlyFinish;
                    }
                }
            }
        }

        for (int i = 0; i < numTasks; ++i) {
            tasks[i].MaxLength = tasks[i].duration + tasks[i].earlyFinish + tasks[i].earlyStart;
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
        if(hasCycle()) {
            cout << "\nMatrix has a cycle. Can't calculate.";
            return;
        }
        int crit = 0;
        for (int l = 0; l < numTasks; ++l) {
            if (tasks[l].MaxLength > crit) crit = tasks[l].MaxLength;
        }

        criticalPathLength = crit;

        for (int i = 0; i < numTasks; ++i) {
            tasks[i].slack = criticalPathLength - tasks[i].MaxLength;
        }
        vector<int> crit_tasks;
        for (int i = 0; i < numTasks; ++i) if (tasks[i].MaxLength - crit == 0) crit_tasks.push_back(i);

        cout << "\nCritical path tasks: ";
        for (int i = 0; i < crit_tasks.size(); ++i) cout << crit_tasks[i] << " ";
    }

    void printSchedule() {
        cout << "\n\nTask\tEarly Start\tEarly Finish\tMax Length\tSlack\n";
        for (const auto& task : tasks) {
            cout << task.id << "\t" << task.earlyStart << "\t\t" << task.earlyFinish << "\t\t"
                << task.MaxLength << "\t\t" << task.slack << endl;
        }
        cout << "\nLength of critical path: " << criticalPathLength << endl;
    }
};

int main() {
    link:
    int numTasks;
    cout << "\nEnter the number of tasks: ";
    cin >> numTasks;

    vector<vector<int>> mat(numTasks, vector<int>(numTasks));
    cout << "\nEnter the matrix (only 0 or 1):\n";
    for (int i = 0; i < numTasks; ++i) {
        for (int j = 0; j < numTasks; ++j) {
            cin >> mat[i][j];
        }
    }

    vector<int> durations(numTasks);
    cout << "\nEnter the durations of tasks:\n";
    for (int i = 0; i < numTasks; ++i) {
        cin >> durations[i];
    }

    NetworkPlanning planner(numTasks);
    planner.setDependencies(mat);
    planner.setDurations(durations);

    planner.calculateEarliestTimes();
    planner.calculateCriticalPath();
    planner.printSchedule();

    int answer;
    cout << "\n\nAgain? (1 - yes, 0 - no)\n";
    cin >> answer;
    if (answer) goto link;
    return 0;
}

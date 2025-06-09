#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
class dooz {
public:
    std::string name;
    int progressCounter;
    int priority;
    bool alive = true;
    bool inProgress = false;
    std::string dueDate = "";  // Optional due date

    dooz(std::string name, int progressCounter, int priority, std::string dueDate = "")
        : name(name), progressCounter(progressCounter), priority(priority), dueDate(dueDate) {
    }

    void print() const {
        std::cout << "Task: " << name
                  << ", Progress: " << progressCounter
                  << ", Priority: " << priority;

        if (!dueDate.empty()) {
            std::cout << ", Due: " << dueDate;
        }

        std::string status;
        if (inProgress) {
            status = "In Progress";
        } else if (alive) {
            status = "Active";
        } else {
            status = "Inactive";
        }

        std::cout << ", Status: " << status << std::endl;
    }

    // Convert to string for saving
    std::string toString() const {
        return name + "," + std::to_string(progressCounter) + "," +
               std::to_string(priority) + "," + (alive ? "1" : "0") + "," +
               (inProgress ? "1" : "0") + "," + dueDate;
    }

    // Create from string for loading
    static dooz fromString(const std::string& line) {
        std::stringstream ss(line);
        std::string name, progressStr, priorityStr, aliveStr, inProgressStr, dueDate;

        std::getline(ss, name, ',');
        std::getline(ss, progressStr, ',');
        std::getline(ss, priorityStr, ',');
        std::getline(ss, aliveStr, ',');
        std::getline(ss, inProgressStr, ',');
        std::getline(ss, dueDate); // Get the rest of the line for due date

        dooz task(name, std::stoi(progressStr), std::stoi(priorityStr), dueDate);
        task.alive = (aliveStr == "1");
        task.inProgress = (inProgressStr == "1");
        return task;
    }
};

// Function to display help
void showHelp() {
    std::cout << "\n========== DOOZ TASK MANAGER HELP ==========\n";
    std::cout << "Available commands:\n\n";
    std::cout << "add       - Add a new task with optional due date\n";
    std::cout << "print     - Display all tasks\n";
    std::cout << "takeout   - Mark a task as 'In Progress'\n";
    std::cout << "putback   - Put a task back to 'Active' and update progress\n";
    std::cout << "save      - Manually save tasks to file\n";
    std::cout << "help      - Show this help menu\n";
    std::cout << "exit      - Save and exit the program\n";
    std::cout << "\nTask Status:\n";
    std::cout << "- Active: Available to work on\n";
    std::cout << "- In Progress: Currently being worked on\n";
    std::cout << "- Inactive: Completed or paused\n";
    std::cout << "============================================\n\n";
}

// Function to find task by name
int findTaskByName(const std::vector<dooz>& tasks, const std::string& taskName) {
    for (size_t i = 0; i < tasks.size(); ++i) {
        if (tasks[i].name == taskName) {
            return i;
        }
    }
    return -1;
}


// Function to save tasks to file
void saveTasks(const std::vector<dooz>& tasks, const std::string& filename = "tasks.dooz") {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (const auto& task : tasks) {
            file << task.toString() << std::endl;
        }
        file.close();
        std::cout << "Tasks saved successfully!" << std::endl;
    } else {
        std::cout << "Error: Could not save tasks to file." << std::endl;
    }
}

// Function to load tasks from file
std::vector<dooz> loadTasks(const std::string& filename = "tasks.dooz") {
    std::vector<dooz> tasks;
    std::ifstream file(filename);

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (!line.empty()) {
                try {
                    tasks.push_back(dooz::fromString(line));
                } catch (const std::exception& e) {
                    std::cout << "Warning: Could not parse line: " << line << std::endl;
                }
            }
        }
        file.close();
        if (!tasks.empty()) {
            std::cout << "Loaded " << tasks.size() << " tasks from file." << std::endl;
        }
    } else {
        std::cout << "No existing task file found. Starting with empty task list." << std::endl;
    }

    return tasks;
}

int main() {
    std::cout << "Welcome!" << std::endl;

    // Load tasks from file on startup
    std::vector<dooz> tasks = loadTasks();

    int breaker = 0;
    while (breaker == 0) {
        std::string input;
        std::cout << ">";
        std::cin >> input;

        if (input == "add") {
            std::string task, dueDate;
            int progress, priority;

            std::cout << "Enter Task: ";
            std::cin >> task;

            std::cout << "Enter Progress Counter: ";
            std::cin >> progress;

            std::cout << "Enter Priority: ";
            std::cin >> priority;

            std::cout << "Enter Due Date (optional, press Enter to skip): ";
            std::cin.ignore(); // Clear the newline from previous input
            std::getline(std::cin, dueDate);

            // Create new dooz instance and add to vector
            dooz newTask(task, progress, priority, dueDate);
            tasks.push_back(newTask);

            std::cout << "Task added successfully!" << std::endl;
        }
        else if (input == "takeout") {
            if (tasks.empty()) {
                std::cout << "No tasks available to take out." << std::endl;
            } else {
                std::string taskName;
                std::cout << "Enter task name to take out: ";
                std::cin >> taskName;

                int index = findTaskByName(tasks, taskName);
                if (index != -1) {
                    if (tasks[index].inProgress) {
                        std::cout << "Task '" << taskName << "' is already in progress." << std::endl;
                    } else {
                        tasks[index].inProgress = true;
                        tasks[index].alive = true; // Make sure it's active
                        std::cout << "Task '" << taskName << "' is now in progress!" << std::endl;
                    }
                } else {
                    std::cout << "Task '" << taskName << "' not found." << std::endl;
                }
            }
        }
        else if (input == "putback") {
            if (tasks.empty()) {
                std::cout << "No tasks available to put back." << std::endl;
            } else {
                std::string taskName;
                std::cout << "Enter task name to put back: ";
                std::cin >> taskName;

                int index = findTaskByName(tasks, taskName);
                if (index != -1) {
                    if (!tasks[index].inProgress) {
                        std::cout << "Task '" << taskName << "' is not currently in progress." << std::endl;
                    } else {
                        tasks[index].inProgress = false;

                        int newProgress;
                        std::cout << "Enter new progress level: ";
                        std::cin >> newProgress;
                        tasks[index].progressCounter = newProgress;

                        std::cout << "Task '" << taskName << "' put back with progress " << newProgress << "!" << std::endl;
                    }
                } else {
                    std::cout << "Task '" << taskName << "' not found." << std::endl;
                }
            }
        }
        else if (input == "help") {
            showHelp();
        }
        else if (input == "exit") {
            // Save tasks before exiting
            saveTasks(tasks);
            breaker = 1;
        }
        else if (input == "save") {
            // Manual save command
            saveTasks(tasks);
        }
        else if (input == "print") {
            if (tasks.empty()) {
                std::cout << "No tasks found." << std::endl;
            } else {
                std::cout << "\nAll Tasks:" << std::endl;
                for (size_t i = 0; i < tasks.size(); ++i) {
                    std::cout << i + 1 << ". ";
                    tasks[i].print();
                }
                std::cout << std::endl;
            }
        }
        else {
            std::cout << "Unknown command. Type 'help' to see available commands." << std::endl;
        }
    }
    
    return 0;
}
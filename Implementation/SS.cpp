#include <iostream>
#include <vector>
#include <algorithm>
int current_time = 0;
class Task {
public:
    int id;
    int execution_time; // Time required to complete the task
    int period;         // Period of the task
    int next_deadline;  // Next deadline of the task
    int remaining_time; // Remaining time to complete the current instance of the task
    int arrival_time;
    bool isPeriodic;

    Task(int id, int exec_time, int period,int arrival_time,bool isPeriodic)
        : id(id), execution_time(exec_time), period(period),
          next_deadline(period), remaining_time(exec_time),arrival_time(arrival_time),isPeriodic(isPeriodic) {}

    void reset() {
        if (isPeriodic){
        remaining_time = execution_time;
        next_deadline += period;
        }
    }
};
bool check_duplicate(std::vector<Task> list,Task task ){
    for (auto list_item: list) {
            if (list_item.id==task.id){
                return true;
            }
        }
    return false;
}
class SporadicServer{
    public:
    int capacity;
    int period;
    int replenishmentAmount;
    int replenishmentTime;
    bool lastStateActive;
    bool secondLastStateActive;
    SporadicServer(int capacity,int period)
    :capacity(capacity),period(period),replenishmentAmount(0),replenishmentTime(period),lastStateActive(false),secondLastStateActive(false){}
};
void schedule_tasks(std::vector<Task>& tasks_full, int total_time,std::vector<Task>& tasks,SporadicServer& server);
void execute_task(Task& task, int& current_time,SporadicServer& server);

int main() {
    std::vector<Task> tasks = {
        Task(0, 1, 5,1,false), // Aperiodic Task 0 with execution time 2 and period 5
        Task(1, 2, 10,0,true), // periodic Task 1 with execution time 2 and period 10
        Task(2, 6, 14,0,true),  // periodic Task 2 with execution time 3 and period 14
        Task(3, 1, 5,8,false)   // Aperiodic Task 3 with execution time 1 and period 5
    };
    std::vector<Task> tasks_temp;
    SporadicServer server = SporadicServer(1,5);
    // Total time to run the scheduler
    int total_time = 20;

    schedule_tasks(tasks, total_time,tasks_temp,server);

    return 0;
}
bool comparater(Task& a, Task& b){
    if (a.remaining_time > 0 && b.remaining_time > 0)
        // std::cout << "comparing results. A id is " << a.id << " b id is " << b.id << "\n";
        return a.period < b.period;
    return a.remaining_time > 0;
}
void schedule_tasks(std::vector<Task>& tasks_full, int total_time,std::vector<Task>& tasks,SporadicServer& server) {

    while (current_time < total_time) {
        // Select the highest priority task that is ready to run
        for(int i =0;i<tasks_full.size();i++){
            if(!check_duplicate(tasks,tasks_full[i])&&tasks_full[i].arrival_time<=current_time){
                tasks.push_back(tasks_full[i]);
            }
        }
        auto selected_task = std::min_element(tasks.begin(), tasks.end(),comparater);
        if(current_time==server.replenishmentTime&&server.replenishmentAmount){
            server.capacity += server.replenishmentAmount;
            std::cout << "server replinshed with " << server.replenishmentAmount << " at time " << current_time << "\n";
            server.replenishmentAmount=0;

        }
        if (selected_task != tasks.end() && selected_task->remaining_time > 0) {
            execute_task(*selected_task, current_time,server);
        } else {
            current_time++;
            std::cout<<"Idle at time = "<<current_time<<std::endl;
            server.secondLastStateActive=server.lastStateActive;
            server.lastStateActive=false;
            if (server.secondLastStateActive&&!server.lastStateActive){
                server.replenishmentAmount= 1-server.capacity;
                std::cout << "server will replenish with "<< server.replenishmentAmount << "\n";
        }
        }

        // Check if any task's period has expired and reset it
        for (auto& task : tasks) {
            if (current_time >= task.next_deadline) {
                if (task.remaining_time > 0) {
                    std::cout << "Task " << task.id << " missed its deadline at time " << current_time << "\n";
                }
                task.reset();
            }
        }
    }
}

void execute_task(Task& task, int& current_time,SporadicServer& server) {
    if(task.isPeriodic){
    task.remaining_time--;
    current_time++;
    std::cout << "Executed Periodic task " << task.id << " at time " << current_time << "\n";
    server.secondLastStateActive=server.lastStateActive;
    server.lastStateActive=false;
        if (server.secondLastStateActive&&!server.lastStateActive){
        server.replenishmentAmount= 1-server.capacity;
        std::cout << "server will replenish with "<< server.replenishmentAmount << "\n";
        }
    }
    else{
        task.remaining_time--;
        current_time++;
        std::cout << "Executed Aperiodic task " << task.id << " at time " << current_time << "\n";
        server.capacity--;
        server.secondLastStateActive=server.lastStateActive;
        server.lastStateActive=true;
        if (!server.secondLastStateActive&&server.lastStateActive){
            server.replenishmentTime=current_time+server.period;
            std::cout << "server will replenish  "<<" at time " << server.replenishmentTime << "\n";
        }
    }
}

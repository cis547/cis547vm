#ifndef TASK_H
#define TASK_H

#include <string>
#include <vector>

// Defining the Task class and its child classes
// Definitions of functions are made in the .cpp file in the src directory

enum Month {Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec};

std::string monthToString(Month);

class Misc;

class Bill;

class Task {
private:
    int ID;

protected:
    std::string description;

public:
    // constructor
    Task(std::string taskdesc, int tid) : description(taskdesc), ID(tid) {}

    // getters
    virtual std::string getDescription() = 0;
};

class Misc : public Task {
public:
    Misc(std::string taskdesc, int tid) : Task(taskdesc, tid) {}

    std::string getDescription();
};

class Bill : public Task {
private:
    float amount;
    std::string payee;
    Month deadline;

public:
    // constructor
    Bill(std::string billdesc, int bid, float billAmount, std::string payee, Month deadline) :
        Task(billdesc, bid), amount(billAmount), payee(payee), deadline(deadline) {}

    std::string getDescription();
    std::string getPayee();
    Month getDeadline();
    float getAmount();
};

#endif

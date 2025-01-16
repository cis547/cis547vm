#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>

#include "User.h"
#include "Task.h"

/**
 * Check if `user` exists in the map. If so, add `task` to the set of tasks for that user.
 * Otherwise, add a new entry for the user and a singleton set containing `task`.
 */
void addTask(std::map<User*, std::set<Task*>>& userTasks, User* user, Task* task) {
    /**
     * TODO: Enter your code here
     */
}

/**
 * Define the following function to iterate over all users in the usertasks map and print all tasks of each user.
 *
 * For each user, print the user's name and email in the following format:
 * User: `userName` (`userEmail`):
 *
 * For each task of the user, use the `dynamic_cast` function to check if the task is an object of the `Bill` class or
 * the `Misc` class and print using the formats:
 * For `Misc` objects:
 * - `taskDescription`
 *
 * For `Bill` objects:
 * - `taskDescription` of `amount` to be paid to `payee` by `deadline`
 * Get the string representation of Month objects using the `monthToString` function
 *
 *
 * Use a for loop with iterators to iterate over the `userTasks` map
 * Use a for each loop to iterate over the set of tasks for each user
 * use the `auto` keyword wherever possible
 */
void printUsersAndTasks(std::map<User*, std::set<Task*>> userTasks) {
    std::cout << "Printing out users and their tasks:\n";
    std::cout << "---------------------------------\n";

    /**
     * TODO: Enter your code here
     */

    std::cout << "=================================\n\n";
}

/**
 * Define the following function to iterate over all users in the `userTasks` map and print all tasks of each user
 *
 * For each user, print the user's name and email in the following format:
 * User: `userName` (`userEmail`):
 *
 * Only `Bill` objects have deadlines, so use the following format to print the task:
 * - `taskDescription` of `amount` to be paid to `payee` by `deadline`
 *
 * Get the string representation of `Month` objects using the `monthToString` function
 *
 *
 * Use a for loop with iterators to iterate over the `userTasks` map
 * Use a for each loop to iterate over the set of tasks for each user
 * use the `auto` keyword wherever possible
 */
void printTasksDue(std::map<User*, std::set<Task*>> userTasks, Month due) {
    std::cout << "Tasks due in " << monthToString(due) << ":\n";

    /**
     * TODO: Enter your code here
     */
}

/**
 * Define mapEquality to determine if the two user-task maps are equal or not.
 */
bool mapEquality(std::map<User*, std::set<Task*>> userTasksA, std::map<User*, std::set<Task*>> userTasksB) {
    /**
     * TODO: Enter your code here
     *
     * Returns `false` by default to prevent Illegal Instruction errors. Change this when defining the function.
     */

    return false;
}

int main() {
    User* user1;
    User* user2;
    std::vector<User*> users;

    /**
     * Using the `new` keyword
     * Initialize `user1` to have ID 1132, Name "A. Smith", Email "smith@gmail.com"
     * Initialize `user2` to have ID 1857, Name "J. Doe", Email "doe@outlook.com"
     *
     * TODO: Enter your code here
     */

    std::map<User*, std::set<Task*>> userTasks;

    /**
     * Define the `addTask` function and use it to add the following to the `userTasks` map:
     * 1. A task of type `Misc` with ID 112 and description "Do Laundry" for "A. Smith"
     * 2. A task of type `Bill` with ID 113, description "Electricity", amount 21.94, payee "PECO" due in Aug for "A. Smith"
     * 3. A task of type `Misc` with ID 141 and description "Get Groceries" for "J. Doe"
     * 4. A task of type `Bill` with ID 144, description "Phone", amount 40, payee "MobiComm" due in Oct for "J. Doe"
     *
     * TODO: Enter your code here
     */

    // Making a copy of `userTasks`
    auto userTasksCopy = userTasks;

    // Define the `printUsersAndTasks` function. The following line will print the map once the function is defined
    printUsersAndTasks(userTasks);

    // Define the `printTasksDue` function. The following line will print all the tasks due in `Aug` and their corresponding users
    printTasksDue(userTasks, Aug);

    /**
     * Remove `user2` from the map
     *
     * TODO: Enter your code here
     */

    // printing all users and tasks again to verify that `user2` is removed using the printUsersAndTasks function
    printUsersAndTasks(userTasks);

    // Define the `mapEquality` function. This will be used to check if removing `user2` affected `userTasksCopy`
    if(mapEquality(userTasksCopy, userTasks)) {
        std::cout << "Copy remains equal to the original\n";
    } else {
        std::cout << "Copy is no longer equal to the original\n";
    }

    return 0;
}

/*
Expected Output:

Printing out users and their tasks:
---------------------------------
User: A. Smith (smith@gmail.com):
- Miscellaneous Task: Do Laundry
- Bill: Electricity of $21.94 to be paid to PECO by Aug

User: J. Doe (doe@outlook.com):
- Miscellaneous Task: Get Groceries
- Bill: Phone of $40 to be paid to MobiComm by Oct

=================================

Tasks due in Aug:
User: A. Smith (smith@gmail.com):
- Bill: Electricity of $21.94 to be paid to PECO by Aug


Printing out users and their tasks:
---------------------------------
User: A. Smith (smith@gmail.com):
- Miscellaneous Task: Do Laundry
- Bill: Electricity of $21.94 to be paid to PECO by Aug

=================================

Copy is no longer equal to the original

*/

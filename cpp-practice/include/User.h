#ifndef USER_H
#define USER_H

#include <string>

// Declaring the User class
// Definitions of functions are made in the .cpp file under the src directory

class User {
private:
    std::string name;
    std::string email;
    int ID;

public:
    // constructor
    User(std::string username, std::string useremail, int uid) : name(username), email(useremail), ID(uid) {}

    // getters
    std::string getName();
    std::string getEmail();

    // overloading operator
    friend bool operator==(const User& user1, const User& user2);
};

#endif
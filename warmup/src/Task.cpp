#include "Task.h"

std::string monthToString(Month month) {
    switch(month) {
        case Jan:
            return "Jan";
        case Feb:
            return "Feb";
        case Mar:
            return "Mar";
        case Apr:
            return "Apr";
        case May:
            return "May";
        case Jun:
            return "Jun";
        case Jul:
            return "Jul";
        case Aug:
            return "Aug";
        case Sep:
            return "Sep";
        case Oct:
            return "Oct";
        case Nov:
            return "Nov";
        case Dec:
            return "Dec";
        default:
            return "Unsupported Month";
    }
}

std::string Misc::getDescription() {
    return "Miscellaneous Task: " + description;
}

std::string Bill::getDescription() {
    return "Bill: " + description;
}

std::string Bill::getPayee() {
    return payee;
}

Month Bill::getDeadline() {
    return deadline;
}

float Bill::getAmount() {
    return amount;
}

// #include <iostream>
// #include <fstream>
// #include <sstream>
// #include <vector>
// #include <map>
// #include <ctime>

/*
Assumptions :

1)The input file is a CSV file.
2)The data fields in each line are assumed to be separated by spaces,
and the order is assumed to be positionId, name, startTime, endTime.
3)The date and time formats in the file are assumed to follow the pattern "%m/%d/%Y %I:%M %p".
4)The program currently prints error messages to cerr when there are issues parsing time.
This assumes that error messages are sufficient for debugging and don't need to be handled programmatically.
5)The file path for the input CSV file is hard-coded in the main function. 
This assumes that the file is available at the specified location


*/
#include <bits/stdc++.h>
#include <algorithm>  
#include <iomanip>

using namespace std;

//defining structure for time entries 
struct TimeEntry {
    string positionId;
    tm startTime;
    tm endTime;
};

//defining structure for employees data
struct Employee {
    string name;
    vector<TimeEntry> timeEntries;
};

// Function to parse time with default values

bool parseTime(const string& timeStr, tm& resultTime, bool useDefault = false) {
    if (timeStr.empty()) {
        if (useDefault) {
            // Set default values (you can customize these)
            /*
            default values are set to collectively and correctly parse through the empty 
            field values in the dataset so that it doesnt raises any error in the time parsing
             */
            resultTime.tm_year = 0; // Default year
            resultTime.tm_mon = 0;  // Default month
            resultTime.tm_mday = 1; // Default day
            resultTime.tm_hour = 0; // Default hour
            resultTime.tm_min = 0;  // Default minute
            resultTime.tm_sec = 0;  // Default second
            return true;
        } else {
            cerr << "Error: Empty time string." << endl;
            return false;
        }
    }
    //parsing time string using stringstream -- more like a cin but for a file input values we use this for effectiveness

    istringstream timeStream(timeStr);
    if (!(timeStream >> get_time(&resultTime, "%m/%d/%Y %I:%M %p"))) {
       // cerr << "Error parsing time: " << timeStr << ". Invalid time format." << endl;  --debbuging statements
        return false;
    }

    // Check if the parsed time is valid
    if (mktime(&resultTime) == -1) {
       // cerr << "Error parsing time: " << timeStr << ". Invalid time values." << endl;  --debbuging statements
        return false;
    }

    return true;
}

// Function to print employees who meet specific criteria


void printEmployees(const vector<Employee>& employees, const string& criteria) {
    cout << "Employees who meet the criteria '" << criteria << "':" << endl;

    //iterating over each employee 
    for (const Employee& employee : employees) {
        bool meetsCriteria = false;

        // Check the specified criteria for each employee

        if (criteria == "SEVEN_CONSECUTIVE_DAYS") {
            meetsCriteria = (employee.timeEntries.size() >= 7);
            for (size_t i = 6; i < employee.timeEntries.size(); ++i) {
                tm currentDayCopy = employee.timeEntries[i].startTime;
                tm previousDayCopy = employee.timeEntries[i - 6].startTime;

                // Check if there are seven consecutive days of work

                if (difftime(mktime(&currentDayCopy), mktime(&previousDayCopy)) != 86400) {
                    meetsCriteria = false;
                    break;
                }
            }
        } else if (criteria == "LESS_THAN_10_HOURS_BETWEEN_SHIFTS") {
            meetsCriteria = (employee.timeEntries.size() >= 2);
            for (size_t i = 1; i < employee.timeEntries.size(); ++i) {
                tm currentEndTimeCopy = employee.timeEntries[i].endTime;
                tm previousStartTimeCopy = employee.timeEntries[i - 1].startTime;

                // Check if there are less than 10 hours between consecutive shifts

                auto diffSeconds = difftime(mktime(&currentEndTimeCopy), mktime(&previousStartTimeCopy));
                if (diffSeconds <= 3600 || diffSeconds >= 10 * 3600) {
                    meetsCriteria = false;
                    break;
                }
            }
        } else if (criteria == "MORE_THAN_14_HOURS_IN_SINGLE_SHIFT") {
            meetsCriteria = any_of(
                employee.timeEntries.begin(), employee.timeEntries.end(),
                [](const TimeEntry& entry) {
                    tm startTimeCopy = entry.startTime;
                    tm endTimeCopy = entry.endTime;

                    // Check if there is more than 14 hours in a single shift

                    auto diffSeconds = difftime(mktime(&endTimeCopy), mktime(&startTimeCopy));
                    return diffSeconds > 14 * 3600;
                }
            );
        }

        // Printing the result based on whether the employee meets the criteria

        if (meetsCriteria) {
            cout << "Name: " << employee.name << ", Criteria: " << criteria << endl;
        } else {
            cout << "Name: " << employee.name << ", Does not meet criteria: " << criteria << endl;
        }
    }
}


int main() {

    //filepath specified 
    string filePath = R"(C:\Users\utkar\OneDrive\Desktop\SDI\Assignment_Timecard.xlsx - Sheet1.csv)";
    ifstream file(filePath);
    //checkbox for the file is opened or not 
    if (!file.is_open()) {
        cerr << "Error: Unable to open file." << endl;
        return 1;
    }

    // Create vectors and maps to store employee data
    vector<Employee> employees;
    map<string, Employee> employeeMap;

    // Read the file line by line
    string line;
    getline(file, line); // Skip header line if present
    while (getline(file, line)) {
        istringstream iss(line);
        string name, positionId, startTime, endTime;

        // getline(iss, positionId, '\t');
        // getline(iss, name, '\t');
        // getline(iss, startTime, '\t');
        // getline(iss, endTime, '\t');

        // Extract data from the line using string stream
        iss >> positionId >> name >> startTime >> endTime;


        // Create a TimeEntry structure to store time data
        TimeEntry timeEntry;
        timeEntry.positionId = positionId;

        // Use default values for missing dates and times
        if (!parseTime(startTime, timeEntry.startTime, true) || !parseTime(endTime, timeEntry.endTime, true)) {
           // cerr << "Skipping line due to time parsing error: " << line << endl; --debugging statements 
            continue; // Skip entry with parsing error
        }

        // Update employee data in the map
        if (employeeMap.find(name) == employeeMap.end()) {
            Employee newEmployee;
            newEmployee.name = name;
            newEmployee.timeEntries.push_back(timeEntry);
            employeeMap[name] = newEmployee;
        } else {
            employeeMap[name].timeEntries.push_back(timeEntry);
        }
    }
    // Transfer data from the map to the vector
    for (const auto& entry : employeeMap) {
        employees.push_back(entry.second);
    }

    // Remove  whitespaces from criteria
    string criteria1 = "SEVEN_CONSECUTIVE_DAYS";
    string criteria2 = "LESS_THAN_10_HOURS_BETWEEN_SHIFTS";
    string criteria3 = "MORE_THAN_14_HOURS_IN_SINGLE_SHIFT";

    printEmployees(employees, criteria1);
    printEmployees(employees, criteria2);
    printEmployees(employees, criteria3);

    return 0;
}

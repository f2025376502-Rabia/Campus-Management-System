#include "attendance.h"
#include "filehandler.h"
#include "course_ops.h"
#include <iostream>
using namespace std;

string ATT_PATH = "attendance_log.txt";
string ATT_HEAD = "roll,course_code,date,status";

// A snapshot of the file taken before marking, used to undo a session.
vector<vector<string> > savedSnapshot;
bool snapshotReady = false;

// Marks P/A/L for every enrolled student and appends to the log.
void markAttendance(string courseCode, string semester, string date) {
    // take a backup of the current file before any changes
    savedSnapshot = readTXT(ATT_PATH);
    snapshotReady = true;

    vector<string> rolls = listEnrolledStudents(courseCode, semester);

    for (int i = 0; i < (int)rolls.size(); i++) {
        string mark;
        cout << "Mark for " << rolls[i] << " (P/A/L): ";
        cin >> mark;

        // accept only P, A, or L; anything else defaults to A
        if (mark != "P" && mark != "A" && mark != "L") {
            mark = "A";
        }

        vector<string> row;
        row.push_back(rolls[i]);
        row.push_back(courseCode);
        row.push_back(date);
        row.push_back(mark);
        appendTXT(ATT_PATH, row);
    }
    cout << "Attendance recorded for " << date << "\n";
}

// Percentage where a late (L) counts as half a present.
double getAttendancePct(string roll, string courseCode) {
    vector<vector<string> > rows = readTXT(ATT_PATH);

    double present = 0;
    double late = 0;
    double total = 0;

    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i][0] == roll && rows[i][1] == courseCode) {
            total = total + 1;
            if (rows[i][3] == "P") present = present + 1;
            if (rows[i][3] == "L") late = late + 1;
        }
    }

    if (total == 0) return 0.0;
    return (present + 0.5 * late) / total * 100.0;
}

// Rolls of enrolled students who are below 75%.
vector<string> getShortageList(string courseCode, string semester) {
    vector<string> rolls = listEnrolledStudents(courseCode, semester);
    vector<string> low;
    for (int i = 0; i < (int)rolls.size(); i++) {
        if (getAttendancePct(rolls[i], courseCode) < 75.0) {
            low.push_back(rolls[i]);
        }
    }
    return low;
}

// Restores the file from the saved snapshot.
bool undoLastSession() {
    if (!snapshotReady) return false;
    writeTXT(ATT_PATH, ATT_HEAD, savedSnapshot);
    snapshotReady = false;
    return true;
}

// Prints each enrolled student's status for a particular date.
void printDailySheet(string courseCode, string semester, string date) {
    vector<string> rolls = listEnrolledStudents(courseCode, semester);
    vector<vector<string> > rows = readTXT(ATT_PATH);

    cout << "\n--- Daily Sheet: " << courseCode << " on " << date << " ---\n";
    cout << "Roll\t\tStatus\n";

    for (int i = 0; i < (int)rolls.size(); i++) {
        string mark = "-";
        for (int j = 0; j < (int)rows.size(); j++) {
            if (rows[j][0] == rolls[i] &&
                rows[j][1] == courseCode &&
                rows[j][2] == date) {
                mark = rows[j][3];
            }
        }
        cout << rolls[i] << "\t" << mark << "\n";
    }
}
#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <string>
#include <vector>
using namespace std;

// === Attendance handling ===

// Marks attendance (P/A/L) for each enrolled student on a date.
// Takes a backup of the file first so the session can be undone later.
void markAttendance(string courseCode, string semester, string date);

// Returns the attendance percentage: (present + 0.5*late) / total * 100.
double getAttendancePct(string roll, string courseCode);

// Returns rolls of students whose attendance in a course is below 75%.
vector<string> getShortageList(string courseCode, string semester);

// Restores the file from the last backup. Returns false if no backup exists.
bool undoLastSession();

// Prints a formatted table of enrolled students and their status for a date.
void printDailySheet(string courseCode, string semester, string date);

#endif
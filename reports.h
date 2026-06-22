#ifndef REPORTS_H
#define REPORTS_H

#include <string>
using namespace std;

// === Formatted reports ===

// Active students ranked by CGPA (highest first) with a rank column.
void printMeritList();

// Students whose course attendance is below 75%, with roll, name, course, %.
void printAttendanceDefaulters();

// Fee defaulters listed with their outstanding amount.
void printFeeDefaulters();

// Full result sheet with borders: GPA and pass/fail status per student.
void printSemesterResult(string semester);

// Department-wise summary: count, average CGPA, and pass rate.
void printDepartmentSummary();

// Writes a chosen report to a txt file by redirecting cout, then restores it.
void exportReportToFile(int reportChoice, string semester);

#endif
#ifndef STUDENT_OPS_H
#define STUDENT_OPS_H

#include <string>
#include <vector>
using namespace std;

// === Student record operations ===

// Adds a student after checking roll format, duplicates, name, and cgpa range.
// Returns a message describing success or the reason for failure.
string addStudent(string roll, string name, string dept, double cgpa);

// Returns the matching student row, or empty if the roll does not exist.
vector<string> searchByRoll(string roll);

// Returns every student whose name contains the given text.
vector<vector<string> > searchByName(string partialName);

// Changes one field (by column) of a student. The roll itself cannot change.
bool updateStudent(string roll, int fieldIndex, string newValue);

// Soft delete: marks the student's status as 'inactive' (keeps the row).
bool softDelete(string roll);

// Returns active students sorted by roll number using selection sort.
vector<vector<string> > listActiveStudents();

#endif
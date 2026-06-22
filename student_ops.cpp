#include "student_ops.h"
#include "filehandler.h"
#include <iostream>
using namespace std;

// students.txt columns: roll,name,dept,cgpa,status
string STUDENTS_PATH = "students.txt";
string STUDENTS_HEAD = "roll,name,dept,cgpa,status";

// Checks roll pattern BSAI-YY-XXX (e.g. BSAI-24-101) with character checks.
bool rollIsValid(string roll) {
    if (roll.length() != 11) return false;            // must be 11 chars long
    if (roll.substr(0, 5) != "BSAI-") return false;   // must start with BSAI-

    // positions 5 and 6 are the two year digits
    if (roll[5] < '0' || roll[5] > '9') return false;
    if (roll[6] < '0' || roll[6] > '9') return false;

    if (roll[7] != '-') return false;                 // separating dash

    // positions 8, 9, 10 are the three roll digits
    for (int i = 8; i <= 10; i++) {
        if (roll[i] < '0' || roll[i] > '9') return false;
    }
    return true;
}

// Returns true only if the name has no digit characters.
bool nameIsClean(string name) {
    for (int i = 0; i < (int)name.length(); i++) {
        if (name[i] >= '0' && name[i] <= '9') return false;
    }
    return true;
}

// Adds a new student record after all validations pass.
string addStudent(string roll, string name, string dept, double cgpa) {
    if (!rollIsValid(roll)) {
        return "ERROR: Roll must follow BSAI-YY-XXX";
    }
    if (rowExists(STUDENTS_PATH, 0, roll)) {
        return "ERROR: A student with this roll already exists";
    }
    if (!nameIsClean(name)) {
        return "ERROR: Name should not contain digits";
    }
    if (cgpa < 0.0 || cgpa > 4.0) {
        return "ERROR: CGPA must lie between 0.0 and 4.0";
    }

    vector<string> record;
    record.push_back(roll);
    record.push_back(name);
    record.push_back(dept);
    record.push_back(to_string(cgpa));
    record.push_back("active");           // new students begin as active

    appendTXT(STUDENTS_PATH, record);
    return "SUCCESS: New student saved";
}

// Looks up a student by roll number (column 0).
vector<string> searchByRoll(string roll) {
    return findRow(STUDENTS_PATH, 0, roll);
}

// Returns true if 'whole' contains 'part' anywhere inside it.
bool hasSubstring(string whole, string part) {
    if (part.length() > whole.length()) return false;
    for (int i = 0; i + (int)part.length() <= (int)whole.length(); i++) {
        if (whole.substr(i, part.length()) == part) return true;
    }
    return false;
}

// Returns all students whose name contains the search text (column 1).
vector<vector<string> > searchByName(string partialName) {
    vector<vector<string> > all = readTXT(STUDENTS_PATH);
    vector<vector<string> > hits;
    for (int i = 0; i < (int)all.size(); i++) {
        if (hasSubstring(all[i][1], partialName)) {
            hits.push_back(all[i]);
        }
    }
    return hits;
}

// Updates one field of a student and rewrites the file. Roll cannot change.
bool updateStudent(string roll, int fieldIndex, string newValue) {
    if (fieldIndex == 0) return false;    // protect the roll column

    vector<vector<string> > rows = readTXT(STUDENTS_PATH);
    bool done = false;
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i][0] == roll) {
            rows[i][fieldIndex] = newValue;
            done = true;
        }
    }
    if (done) writeTXT(STUDENTS_PATH, STUDENTS_HEAD, rows);
    return done;
}

// Soft delete sets the status column (index 4) to 'inactive'.
bool softDelete(string roll) {
    return updateStudent(roll, 4, "inactive");
}

// Returns active students sorted by roll using selection sort.
vector<vector<string> > listActiveStudents() {
    vector<vector<string> > rows = readTXT(STUDENTS_PATH);
    vector<vector<string> > active;

    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i][4] == "active") active.push_back(rows[i]);
    }

    // selection sort on roll (column 0)
    for (int i = 0; i < (int)active.size(); i++) {
        int minPos = i;
        for (int j = i + 1; j < (int)active.size(); j++) {
            if (active[j][0] < active[minPos][0]) minPos = j;
        }
        vector<string> hold = active[i];
        active[i] = active[minPos];
        active[minPos] = hold;
    }
    return active;
}
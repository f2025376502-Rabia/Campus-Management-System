#include "course_ops.h"
#include "filehandler.h"
#include <iostream>
using namespace std;

// courses.txt     -> code,title,credits,seats,prereq   (prereq at col 4)
// enrollments.txt -> roll,course_code,semester,status  (status at col 3)
string COURSES_PATH = "courses.txt";
string ENROLL_PATH = "enrollments.txt";
string ENROLL_HEAD = "roll,course_code,semester,status";

// Converts a digit string like "3" into an integer manually.
int toInteger(string s) {
    int value = 0;
    for (int i = 0; i < (int)s.length(); i++) {
        if (s[i] >= '0' && s[i] <= '9') {
            value = value * 10 + (s[i] - '0');
        }
    }
    return value;
}

// Sums credit hours of a student's enrolled courses for a semester.
// Nested loop: enrollments outer, courses inner.
int getCreditLoad(string roll, string semester) {
    vector<vector<string> > enrolls = readTXT(ENROLL_PATH);
    vector<vector<string> > courses = readTXT(COURSES_PATH);
    int sum = 0;

    for (int i = 0; i < (int)enrolls.size(); i++) {
        if (enrolls[i][0] == roll &&
            enrolls[i][2] == semester &&
            enrolls[i][3] == "enrolled") {

            string code = enrolls[i][1];
            for (int j = 0; j < (int)courses.size(); j++) {
                if (courses[j][0] == code) {
                    sum = sum + toInteger(courses[j][2]);  // credits at col 2
                }
            }
        }
    }
    return sum;
}

// Checks the prerequisite. NONE means no requirement; otherwise a non-F grade.
bool checkPrerequisite(string roll, string courseCode) {
    vector<string> course = findRow(COURSES_PATH, 0, courseCode);
    if (course.size() == 0) return false;

    string need = course[4];                 // prereq column
    if (need == "NONE") return true;

    // look in grades.txt: roll,course_code,grade,semester
    vector<vector<string> > grades = readTXT("grades.txt");
    for (int i = 0; i < (int)grades.size(); i++) {
        if (grades[i].size() >= 3 &&
            grades[i][0] == roll &&
            grades[i][1] == need &&
            grades[i][2] != "F") {
            return true;
        }
    }
    return false;
}

// Counts how many students are actively enrolled in a course.
int howManyEnrolled(string courseCode, string semester) {
    vector<vector<string> > enrolls = readTXT(ENROLL_PATH);
    int count = 0;
    for (int i = 0; i < (int)enrolls.size(); i++) {
        if (enrolls[i][1] == courseCode &&
            enrolls[i][2] == semester &&
            enrolls[i][3] == "enrolled") {
            count++;
        }
    }
    return count;
}

// Runs all enrollment checks in order and returns the result code.
EnrollResult enrollStudent(string roll, string courseCode, string semester) {
    // 1. student exists and is active
    vector<string> student = findRow("students.txt", 0, roll);
    if (student.size() == 0 || student[4] != "active") {
        return FAIL_STUDENT_INACTIVE;
    }
    // 2. course exists
    vector<string> course = findRow(COURSES_PATH, 0, courseCode);
    if (course.size() == 0) {
        return FAIL_COURSE_MISSING;
    }
    // 3. seats available
    int seats = toInteger(course[3]);
    if (howManyEnrolled(courseCode, semester) >= seats) {
        return FAIL_NO_SEATS;
    }
    // 4. not already enrolled
    vector<vector<string> > enrolls = readTXT(ENROLL_PATH);
    for (int i = 0; i < (int)enrolls.size(); i++) {
        if (enrolls[i][0] == roll &&
            enrolls[i][1] == courseCode &&
            enrolls[i][2] == semester &&
            enrolls[i][3] == "enrolled") {
            return FAIL_DUPLICATE;
        }
    }
    // 5. credit load stays within 21
    int load = getCreditLoad(roll, semester);
    int add = toInteger(course[2]);
    if (load + add > 21) {
        return FAIL_CREDIT_LIMIT;
    }
    // 6. prerequisite satisfied
    if (!checkPrerequisite(roll, courseCode)) {
        return FAIL_PREREQ;
    }

    vector<string> row;
    row.push_back(roll);
    row.push_back(courseCode);
    row.push_back(semester);
    row.push_back("enrolled");
    appendTXT(ENROLL_PATH, row);
    return ENROLL_OK;
}

// Drops a course only when no attendance row exists for it.
bool dropCourse(string roll, string courseCode, string semester) {
    vector<vector<string> > att = readTXT("attendance_log.txt");
    for (int i = 0; i < (int)att.size(); i++) {
        if (att[i][0] == roll && att[i][1] == courseCode) {
            return false;            // attendance present -> cannot drop
        }
    }

    vector<vector<string> > enrolls = readTXT(ENROLL_PATH);
    bool changed = false;
    for (int i = 0; i < (int)enrolls.size(); i++) {
        if (enrolls[i][0] == roll &&
            enrolls[i][1] == courseCode &&
            enrolls[i][2] == semester &&
            enrolls[i][3] == "enrolled") {
            enrolls[i][3] = "dropped";
            changed = true;
        }
    }
    if (changed) writeTXT(ENROLL_PATH, ENROLL_HEAD, enrolls);
    return changed;
}

// Returns rolls of all active enrolled students in a course.
vector<string> listEnrolledStudents(string courseCode, string semester) {
    vector<vector<string> > enrolls = readTXT(ENROLL_PATH);
    vector<string> rolls;
    for (int i = 0; i < (int)enrolls.size(); i++) {
        if (enrolls[i][1] == courseCode &&
            enrolls[i][2] == semester &&
            enrolls[i][3] == "enrolled") {
            rolls.push_back(enrolls[i][0]);
        }
    }
    return rolls;
}

// Converts a result code into words.
string describeResult(EnrollResult r) {
    if (r == ENROLL_OK)             return "Enrollment successful";
    if (r == FAIL_STUDENT_INACTIVE) return "Student not found or inactive";
    if (r == FAIL_COURSE_MISSING)   return "Course does not exist";
    if (r == FAIL_NO_SEATS)         return "No seats available";
    if (r == FAIL_DUPLICATE)        return "Already enrolled in this course";
    if (r == FAIL_CREDIT_LIMIT)     return "Credit load would exceed 21 hours";
    if (r == FAIL_PREREQ)           return "Prerequisite not met";
    return "Unknown result";
}
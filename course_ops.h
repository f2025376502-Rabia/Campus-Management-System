#ifndef COURSE_OPS_H
#define COURSE_OPS_H

#include <string>
#include <vector>
using namespace std;

// Outcome of an enrollment attempt: success or a specific failure reason.
enum EnrollResult {
    ENROLL_OK,
    FAIL_STUDENT_INACTIVE,
    FAIL_COURSE_MISSING,
    FAIL_NO_SEATS,
    FAIL_DUPLICATE,
    FAIL_CREDIT_LIMIT,
    FAIL_PREREQ
};

// Adds up credit hours of all enrolled courses for a student in a semester.
int getCreditLoad(string roll, string semester);

// Returns true if the course prerequisite is met (or is NONE).
bool checkPrerequisite(string roll, string courseCode);

// Attempts to enroll a student and returns an EnrollResult code.
EnrollResult enrollStudent(string roll, string courseCode, string semester);

// Drops a course only if no attendance exists for it. Returns true on success.
bool dropCourse(string roll, string courseCode, string semester);

// Returns rolls of students actively enrolled in a course.
vector<string> listEnrolledStudents(string courseCode, string semester);

// Turns an EnrollResult code into a readable sentence.
string describeResult(EnrollResult r);

#endif
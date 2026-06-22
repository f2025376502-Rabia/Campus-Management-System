#include <iostream>
#include <string>
#include <vector>
#include "filehandler.h"
#include "student_ops.h"
#include "course_ops.h"
#include "attendance.h"
#include "grades.h"
#include "fee_tracker.h"
#include "reports.h"
using namespace std;

// ----- Student sub-menu -----
void studentMenu() {
    int pick = 0;
    while (pick != 6) {
        cout << "\n--- STUDENT MANAGEMENT ---\n";
        cout << "1. Add Student\n";
        cout << "2. Search by Roll\n";
        cout << "3. Search by Name\n";
        cout << "4. Soft Delete Student\n";
        cout << "5. List Active Students\n";
        cout << "6. Back\n";
        cout << "Choice: ";
        cin >> pick;

        if (pick == 1) {
            string roll, name, dept; double cgpa;
            cout << "Roll (BSAI-YY-XXX): "; cin >> roll;
            cout << "Name: "; cin.ignore(); getline(cin, name);
            cout << "Department: "; cin >> dept;
            cout << "CGPA: "; cin >> cgpa;
            cout << addStudent(roll, name, dept, cgpa) << "\n";
        }
        else if (pick == 2) {
            string roll; cout << "Roll: "; cin >> roll;
            vector<string> s = searchByRoll(roll);
            if (s.size() == 0) cout << "Not found.\n";
            else { for (int i = 0; i < (int)s.size(); i++) cout << s[i] << "  "; cout << "\n"; }
        }
        else if (pick == 3) {
            string name; cout << "Name part: "; cin >> name;
            vector<vector<string> > m = searchByName(name);
            for (int i = 0; i < (int)m.size(); i++) cout << m[i][0] << " - " << m[i][1] << "\n";
        }
        else if (pick == 4) {
            string roll; cout << "Roll to delete: "; cin >> roll;
            if (softDelete(roll)) cout << "Marked inactive.\n";
            else cout << "Student not found.\n";
        }
        else if (pick == 5) {
            vector<vector<string> > list = listActiveStudents();
            cout << "\nActive Students (by roll):\n";
            for (int i = 0; i < (int)list.size(); i++)
                cout << list[i][0] << " - " << list[i][1] << "\n";
        }
    }
}

// ----- Course sub-menu -----
void courseMenu() {
    int pick = 0;
    while (pick != 3) {
        cout << "\n--- COURSE MANAGEMENT ---\n";
        cout << "1. Enroll Student\n";
        cout << "2. Drop Course\n";
        cout << "3. Back\n";
        cout << "Choice: ";
        cin >> pick;

        if (pick == 1) {
            string roll, code, sem;
            cout << "Roll: "; cin >> roll;
            cout << "Course code: "; cin >> code;
            cout << "Semester: "; cin >> sem;
            EnrollResult r = enrollStudent(roll, code, sem);
            cout << describeResult(r) << "\n";
        }
        else if (pick == 2) {
            string roll, code, sem;
            cout << "Roll: "; cin >> roll;
            cout << "Course code: "; cin >> code;
            cout << "Semester: "; cin >> sem;
            if (dropCourse(roll, code, sem)) cout << "Course dropped.\n";
            else cout << "Cannot drop (attendance exists or not enrolled).\n";
        }
    }
}

// ----- Attendance sub-menu -----
void attendanceMenu() {
    int pick = 0;
    while (pick != 4) {
        cout << "\n--- ATTENDANCE ---\n";
        cout << "1. Mark Attendance\n";
        cout << "2. View Attendance %\n";
        cout << "3. Undo Last Session\n";
        cout << "4. Back\n";
        cout << "Choice: ";
        cin >> pick;

        if (pick == 1) {
            string code, sem, date;
            cout << "Course code: "; cin >> code;
            cout << "Semester: "; cin >> sem;
            cout << "Date (DD-MM-YYYY): "; cin >> date;
            markAttendance(code, sem, date);
        }
        else if (pick == 2) {
            string roll, code;
            cout << "Roll: "; cin >> roll;
            cout << "Course code: "; cin >> code;
            cout << "Attendance: " << getAttendancePct(roll, code) << "%\n";
        }
        else if (pick == 3) {
            if (undoLastSession()) cout << "Last session undone.\n";
            else cout << "No backup to undo.\n";
        }
    }
}

// ----- Grades sub-menu -----
void gradesMenu() {
    int pick = 0;
    while (pick != 3) {
        cout << "\n--- GRADES ---\n";
        cout << "1. Weighted Total + Letter Grade\n";
        cout << "2. Compute GPA\n";
        cout << "3. Back\n";
        cout << "Choice: ";
        cin >> pick;

        if (pick == 1) {
            double quiz, asgn, mid, fin;
            cout << "Quiz avg: "; cin >> quiz;
            cout << "Assignment avg: "; cin >> asgn;
            cout << "Mid (0-100): "; cin >> mid;
            cout << "Final (0-100): "; cin >> fin;
            double total = computeWeightedTotal(quiz, asgn, mid, fin);
            cout << "Total: " << total << "  Grade: " << getLetterGrade(total) << "\n";
        }
        else if (pick == 2) {
            string roll, sem;
            cout << "Roll: "; cin >> roll;
            cout << "Semester: "; cin >> sem;
            cout << "GPA: " << computeGPA(roll, sem) << "\n";
        }
    }
}

// ----- Fee sub-menu -----
void feeMenu() {
    int pick = 0;
    while (pick != 3) {
        cout << "\n--- FEE TRACKER ---\n";
        cout << "1. Record Payment\n";
        cout << "2. Generate Receipt\n";
        cout << "3. Back\n";
        cout << "Choice: ";
        cin >> pick;

        if (pick == 1) {
            string roll, sem, date; double amount;
            cout << "Roll: "; cin >> roll;
            cout << "Semester: "; cin >> sem;
            cout << "Amount: "; cin >> amount;
            cout << "Paid date (DD-MM-YYYY): "; cin >> date;
            if (recordPayment(roll, sem, amount, date)) cout << "Payment recorded.\n";
            else cout << "Could not record payment.\n";
        }
        else if (pick == 2) {
            string roll, sem;
            cout << "Roll: "; cin >> roll;
            cout << "Semester: "; cin >> sem;
            generateReceipt(roll, sem);
        }
    }
}

// ----- Reports sub-menu -----
void reportsMenu() {
    int pick = 0;
    while (pick != 6) {
        cout << "\n--- REPORTS ---\n";
        cout << "1. Merit List\n";
        cout << "2. Attendance Defaulters\n";
        cout << "3. Fee Defaulters\n";
        cout << "4. Semester Result\n";
        cout << "5. Department Summary\n";
        cout << "6. Back\n";
        cout << "Choice: ";
        cin >> pick;

        if (pick == 1) printMeritList();
        else if (pick == 2) printAttendanceDefaulters();
        else if (pick == 3) printFeeDefaulters();
        else if (pick == 4) { string sem; cout << "Semester: "; cin >> sem; printSemesterResult(sem); }
        else if (pick == 5) printDepartmentSummary();
    }
}

// ----- Main menu -----
int main() {
    int pick = 0;
    while (pick != 7) {
        cout << "\n========================================\n";
        cout << "     CAMPUS MANAGEMENT SYSTEM\n";
        cout << "========================================\n";
        cout << "1. Student Management\n";
        cout << "2. Course Management\n";
        cout << "3. Attendance\n";
        cout << "4. Grades\n";
        cout << "5. Fee Tracker\n";
        cout << "6. Reports\n";
        cout << "7. Exit\n";
        cout << "Choice: ";
        cin >> pick;

        if (pick == 1) studentMenu();
        else if (pick == 2) courseMenu();
        else if (pick == 3) attendanceMenu();
        else if (pick == 4) gradesMenu();
        else if (pick == 5) feeMenu();
        else if (pick == 6) reportsMenu();
        else if (pick == 7) cout << "Goodbye!\n";
        else cout << "Invalid choice.\n";
    }
    return 0;
}
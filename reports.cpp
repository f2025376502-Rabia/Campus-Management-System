#include "reports.h"
#include "filehandler.h"
#include "student_ops.h"
#include "grades.h"
#include "attendance.h"
#include "fee_tracker.h"
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

// Manual string-to-double helper.
double asDouble(string s) {
    double result = 0;
    int i = 0;
    while (i < (int)s.length() && s[i] != '.') {
        if (s[i] >= '0' && s[i] <= '9') result = result * 10 + (s[i] - '0');
        i++;
    }
    if (i < (int)s.length() && s[i] == '.') {
        i++;
        double place = 0.1;
        while (i < (int)s.length()) {
            if (s[i] >= '0' && s[i] <= '9') { result = result + (s[i]-'0')*place; place /= 10; }
            i++;
        }
    }
    return result;
}

// Active students ranked by CGPA (descending).
void printMeritList() {
    vector<vector<string> > rows = readTXT("students.txt");
    vector<vector<string> > active;
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i][4] == "active") active.push_back(rows[i]);
    }

    // bubble sort by CGPA (col 3), highest first
    for (int i = 0; i < (int)active.size() - 1; i++) {
        for (int j = 0; j < (int)active.size() - 1 - i; j++) {
            if (asDouble(active[j][3]) < asDouble(active[j + 1][3])) {
                vector<string> t = active[j];
                active[j] = active[j + 1];
                active[j + 1] = t;
            }
        }
    }

    cout << "\n========== MERIT LIST ==========\n";
    cout << left << setw(6) << "Rank" << setw(15) << "Roll"
         << setw(20) << "Name" << "CGPA\n";
    cout << "--------------------------------------------------\n";
    for (int i = 0; i < (int)active.size(); i++) {
        cout << left << setw(6) << (i + 1)
             << setw(15) << active[i][0]
             << setw(20) << active[i][1]
             << active[i][3] << "\n";
    }
}

// Students below 75% attendance in any course.
void printAttendanceDefaulters() {
    vector<vector<string> > att = readTXT("attendance_log.txt");
    vector<vector<string> > students = readTXT("students.txt");

    cout << "\n===== ATTENDANCE DEFAULTERS (<75%) =====\n";
    cout << left << setw(15) << "Roll" << setw(20) << "Name"
         << setw(12) << "Course" << "Pct\n";
    cout << "--------------------------------------------------\n";

    vector<string> done;
    for (int i = 0; i < (int)att.size(); i++) {
        string roll = att[i][0];
        string course = att[i][1];
        string key = roll + "|" + course;

        bool seen = false;
        for (int k = 0; k < (int)done.size(); k++) {
            if (done[k] == key) seen = true;
        }
        if (seen) continue;
        done.push_back(key);

        double pct = getAttendancePct(roll, course);
        if (pct < 75.0) {
            string name = "Unknown";
            for (int s = 0; s < (int)students.size(); s++) {
                if (students[s][0] == roll) name = students[s][1];
            }
            cout << left << setw(15) << roll << setw(20) << name
                 << setw(12) << course << fixed << setprecision(1) << pct << "%\n";
        }
    }
}

// Fee defaulters with their outstanding balance.
void printFeeDefaulters() {
    vector<string> rolls = getDefaulters();
    vector<vector<string> > fees = readTXT("fees.txt");

    cout << "\n========== FEE DEFAULTERS ==========\n";
    cout << left << setw(15) << "Roll" << setw(15) << "Outstanding" << "\n";
    cout << "-----------------------------------\n";
    for (int i = 0; i < (int)rolls.size(); i++) {
        for (int j = 0; j < (int)fees.size(); j++) {
            if (fees[j][0] == rolls[i]) {
                double bal = asDouble(fees[j][2]) - asDouble(fees[j][3]);
                cout << left << setw(15) << rolls[i] << setw(15) << bal << "\n";
            }
        }
    }
}

// Full semester result sheet with borders.
void printSemesterResult(string semester) {
    vector<vector<string> > students = readTXT("students.txt");

    cout << "\n";
    cout << setfill('#') << setw(50) << "" << "\n" << setfill(' ');
    cout << "        SEMESTER RESULT - " << semester << "\n";
    cout << setfill('#') << setw(50) << "" << "\n" << setfill(' ');
    cout << left << setw(15) << "Roll" << setw(20) << "Name"
         << setw(8) << "GPA" << "Status\n";
    cout << "--------------------------------------------------\n";

    for (int i = 0; i < (int)students.size(); i++) {
        if (students[i][4] != "active") continue;
        string roll = students[i][0];
        double gpa = computeGPA(roll, semester);
        string status = (gpa >= 1.0) ? "PASS" : "FAIL";
        cout << left << setw(15) << roll << setw(20) << students[i][1]
             << setw(8) << fixed << setprecision(2) << gpa << status << "\n";
    }
    cout << setfill('#') << setw(50) << "" << "\n" << setfill(' ');
}

// Department-wise summary using parallel arrays.
void printDepartmentSummary() {
    vector<vector<string> > students = readTXT("students.txt");

    vector<string> names;
    vector<int> counts;
    vector<double> cgpaSums;
    vector<int> passCounts;

    for (int i = 0; i < (int)students.size(); i++) {
        if (students[i][4] != "active") continue;
        string dept = students[i][2];
        double cgpa = asDouble(students[i][3]);

        int idx = -1;
        for (int d = 0; d < (int)names.size(); d++) {
            if (names[d] == dept) idx = d;
        }
        if (idx == -1) {
            names.push_back(dept);
            counts.push_back(0);
            cgpaSums.push_back(0);
            passCounts.push_back(0);
            idx = names.size() - 1;
        }
        counts[idx] = counts[idx] + 1;
        cgpaSums[idx] = cgpaSums[idx] + cgpa;
        if (cgpa >= 2.0) passCounts[idx] = passCounts[idx] + 1;
    }

    cout << "\n========== DEPARTMENT SUMMARY ==========\n";
    cout << left << setw(10) << "Dept" << setw(8) << "Count"
         << setw(12) << "Avg CGPA" << "Pass Rate\n";
    cout << "--------------------------------------------------\n";
    for (int d = 0; d < (int)names.size(); d++) {
        double avg = cgpaSums[d] / counts[d];
        double passRate = (double)passCounts[d] / counts[d] * 100.0;
        cout << left << setw(10) << names[d]
             << setw(8) << counts[d]
             << setw(12) << fixed << setprecision(2) << avg
             << setprecision(0) << passRate << "%\n";
    }
}

// Sends a chosen report to a txt file by redirecting cout.
void exportReportToFile(int reportChoice, string semester) {
    ofstream outFile("report_output.txt");

    // remember where cout normally goes, then point it at the file
    streambuf* original = cout.rdbuf();
    cout.rdbuf(outFile.rdbuf());

    if (reportChoice == 1) printMeritList();
    else if (reportChoice == 2) printAttendanceDefaulters();
    else if (reportChoice == 3) printFeeDefaulters();
    else if (reportChoice == 4) printSemesterResult(semester);
    else if (reportChoice == 5) printDepartmentSummary();

    // put cout back to the screen
    cout.rdbuf(original);
    outFile.close();

    cout << "Report written to report_output.txt\n";
}
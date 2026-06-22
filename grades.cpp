#include "grades.h"
#include "filehandler.h"
#include "attendance.h"
#include <iostream>
using namespace std;

// Averages the best three quizzes by removing the two lowest scores.
double bestThreeOfFive(vector<double> quizzes) {
    int n = quizzes.size();

    // edge case: fewer than three quizzes -> average what we have
    if (n < 3) {
        if (n == 0) return 0.0;
        double s = 0;
        for (int i = 0; i < n; i++) s = s + quizzes[i];
        return s / n;
    }

    vector<double> temp = quizzes;

    // drop the lowest value twice (search each time, no sort)
    for (int pass = 0; pass < 2; pass++) {
        int low = 0;
        for (int i = 1; i < (int)temp.size(); i++) {
            if (temp[i] < temp[low]) low = i;
        }
        for (int i = low; i < (int)temp.size() - 1; i++) {
            temp[i] = temp[i + 1];
        }
        temp.pop_back();
    }

    double sum = 0;
    for (int i = 0; i < (int)temp.size(); i++) sum = sum + temp[i];
    return sum / temp.size();
}

// Applies the assessment weights and returns a score out of 100.
double computeWeightedTotal(double quiz, double asgn, double mid, double final_) {
    return quiz * 0.10 + asgn * 0.10 + mid * 0.30 + final_ * 0.50;
}

// Maps a numeric total to its letter grade.
string getLetterGrade(double total) {
    if (total >= 85) return "A";
    if (total >= 80) return "B+";
    if (total >= 70) return "B";
    if (total >= 65) return "C+";
    if (total >= 60) return "C";
    if (total >= 50) return "D";
    return "F";
}

// Converts a letter grade into GPA points.
double gradeToPoints(string grade) {
    if (grade == "A")  return 4.0;
    if (grade == "B+") return 3.5;
    if (grade == "B")  return 3.0;
    if (grade == "C+") return 2.5;
    if (grade == "C")  return 2.0;
    if (grade == "D")  return 1.0;
    return 0.0;
}

// Manual string-to-int for course credits.
int digitsToInt(string s) {
    int v = 0;
    for (int i = 0; i < (int)s.length(); i++) {
        if (s[i] >= '0' && s[i] <= '9') v = v * 10 + (s[i] - '0');
    }
    return v;
}

// Credit-weighted GPA across all of a student's graded courses in a semester.
// grades.txt columns: roll,course_code,grade,semester
double computeGPA(string roll, string semester) {
    vector<vector<string> > grades = readTXT("grades.txt");
    vector<vector<string> > courses = readTXT("courses.txt");

    double weightedPoints = 0;
    double creditTotal = 0;

    for (int i = 0; i < (int)grades.size(); i++) {
        if (grades[i].size() >= 4 &&
            grades[i][0] == roll &&
            grades[i][3] == semester) {

            string code = grades[i][1];
            double pts = gradeToPoints(grades[i][2]);

            for (int j = 0; j < (int)courses.size(); j++) {
                if (courses[j][0] == code) {
                    int cr = digitsToInt(courses[j][2]);
                    weightedPoints = weightedPoints + pts * cr;
                    creditTotal = creditTotal + cr;
                }
            }
        }
    }

    if (creditTotal == 0) return 0.0;
    return weightedPoints / creditTotal;
}

// Computes highest, lowest, mean, and median of the given marks.
Stats computeClassStats(vector<double> marks) {
    Stats s;
    s.highest = 0; s.lowest = 0; s.mean = 0; s.median = 0;

    int n = marks.size();
    if (n == 0) return s;

    s.highest = marks[0];
    s.lowest = marks[0];
    double sum = 0;
    for (int i = 0; i < n; i++) {
        if (marks[i] > s.highest) s.highest = marks[i];
        if (marks[i] < s.lowest)  s.lowest = marks[i];
        sum = sum + marks[i];
    }
    s.mean = sum / n;

    // median needs sorted data -> bubble sort a copy
    vector<double> ordered = marks;
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            if (ordered[j] > ordered[j + 1]) {
                double t = ordered[j];
                ordered[j] = ordered[j + 1];
                ordered[j + 1] = t;
            }
        }
    }
    if (n % 2 == 1) s.median = ordered[n / 2];
    else s.median = (ordered[n / 2 - 1] + ordered[n / 2]) / 2.0;

    return s;
}

// Forces the grade to F when attendance is below 75%.
string applyAttendancePenalty(string roll, string courseCode, string grade) {
    if (getAttendancePct(roll, courseCode) < 75.0) return "F";
    return grade;
}
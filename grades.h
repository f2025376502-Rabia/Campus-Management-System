#ifndef GRADES_H
#define GRADES_H

#include <string>
#include <vector>
using namespace std;

// Holds class statistics. We use a struct (no class — OOP not allowed).
struct Stats {
    double highest;
    double lowest;
    double mean;
    double median;
};

// Averages the best 3 of up to 5 quizzes by dropping the two lowest.
double bestThreeOfFive(vector<double> quizzes);

// Weighted total out of 100: quiz*0.10 + asgn*0.10 + mid*0.30 + final*0.50.
double computeWeightedTotal(double quiz, double asgn, double mid, double final_);

// Maps a total (0-100) to a letter grade.
string getLetterGrade(double total);

// Converts a letter grade to GPA points (A=4.0 ... F=0.0).
double gradeToPoints(string grade);

// Credit-weighted GPA across a student's courses in a semester.
double computeGPA(string roll, string semester);

// Returns highest, lowest, mean, and median for a list of marks.
Stats computeClassStats(vector<double> marks);

// If attendance is below 75%, the grade is forced to F.
string applyAttendancePenalty(string roll, string courseCode, string grade);

#endif
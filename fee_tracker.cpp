#include "fee_tracker.h"
#include "filehandler.h"
#include <iostream>
#include <iomanip>
using namespace std;

// fees.txt columns: roll,semester,amount_due,amount_paid,due_date,paid_date
string FEES_PATH = "fees.txt";
string FEES_HEAD = "roll,semester,amount_due,amount_paid,due_date,paid_date";

// Manual string-to-double (handles a decimal point too).
double textToDouble(string s) {
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

// Splits a DD-MM-YYYY string into day, month, year (by reference).
void splitDate(string date, int &day, int &month, int &year) {
    day   = (date[0] - '0') * 10 + (date[1] - '0');
    month = (date[3] - '0') * 10 + (date[4] - '0');
    year  = (date[6] - '0') * 1000 + (date[7] - '0') * 100
          + (date[8] - '0') * 10   + (date[9] - '0');
}

// Leap year rule.
bool leapYear(int y) {
    if (y % 400 == 0) return true;
    if (y % 100 == 0) return false;
    if (y % 4 == 0)   return true;
    return false;
}

// Converts a date into "total days since year 0" using a month-length array.
long daysSinceZero(int day, int month, int year) {
    int monthLen[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    long total = 0;

    for (int y = 0; y < year; y++) {
        if (leapYear(y)) total = total + 366;
        else total = total + 365;
    }
    for (int m = 1; m < month; m++) {
        total = total + monthLen[m];
        if (m == 2 && leapYear(year)) total = total + 1;
    }
    total = total + day;
    return total;
}

// Difference in days between two DD-MM-YYYY dates.
int daysBetween(string date1, string date2) {
    int d1,m1,y1,d2,m2,y2;
    splitDate(date1, d1, m1, y1);
    splitDate(date2, d2, m2, y2);

    long a = daysSinceZero(d1, m1, y1);
    long b = daysSinceZero(d2, m2, y2);

    long diff = b - a;
    if (diff < 0) diff = -diff;
    return (int)diff;
}

// Simple format check for DD-MM-YYYY.
bool dateLooksValid(string date) {
    if (date.length() != 10) return false;
    if (date[2] != '-' || date[5] != '-') return false;
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (date[i] < '0' || date[i] > '9') return false;
    }
    return true;
}

// Records a payment: adds the amount and stores the paid date.
bool recordPayment(string roll, string semester, double amount, string paidDate) {
    if (!dateLooksValid(paidDate)) {
        cout << "ERROR: Date must be DD-MM-YYYY\n";
        return false;
    }

    vector<vector<string> > rows = readTXT(FEES_PATH);
    bool found = false;
    for (int i = 0; i < (int)rows.size(); i++) {
        if (rows[i][0] == roll && rows[i][1] == semester) {
            double already = textToDouble(rows[i][3]);
            double updated = already + amount;
            rows[i][3] = to_string((long)updated);
            rows[i][5] = paidDate;
            found = true;
        }
    }
    if (found) writeTXT(FEES_PATH, FEES_HEAD, rows);
    return found;
}

// Late fine: 2% of the amount due per complete week late.
double computeLateFine(string roll, string semester) {
    vector<string> row = findRow(FEES_PATH, 0, roll);
    if (row.size() == 0) return 0.0;
    if (row[1] != semester) return 0.0;

    string dueDate = row[4];
    string paidDate = row[5];
    if (paidDate == "NONE") return 0.0;

    int dd,dm,dy,pd,pm,py;
    splitDate(dueDate, dd, dm, dy);
    splitDate(paidDate, pd, pm, py);

    // only fine if paid after the due date
    if (daysSinceZero(pd, pm, py) <= daysSinceZero(dd, dm, dy)) {
        return 0.0;
    }

    int lateDays = daysBetween(dueDate, paidDate);
    int weeks = lateDays / 7;                 // complete weeks
    double due = textToDouble(row[2]);
    return due * 0.02 * weeks;
}

// Prints a formatted fee receipt.
void generateReceipt(string roll, string semester) {
    vector<string> row = findRow(FEES_PATH, 0, roll);
    if (row.size() == 0) {
        cout << "No fee record for " << roll << "\n";
        return;
    }

    double due  = textToDouble(row[2]);
    double paid = textToDouble(row[3]);
    double fine = computeLateFine(roll, semester);
    double total = due + fine;
    double balance = total - paid;

    cout << "\n";
    cout << setfill('-') << setw(40) << "" << "\n" << setfill(' ');
    cout << "             FEE RECEIPT\n";
    cout << setfill('-') << setw(40) << "" << "\n" << setfill(' ');
    cout << left << setw(20) << "Roll:"        << roll << "\n";
    cout << left << setw(20) << "Semester:"    << semester << "\n";
    cout << left << setw(20) << "Tuition Due:" << due << "\n";
    cout << left << setw(20) << "Late Fine:"   << fine << "\n";
    cout << left << setw(20) << "Total Due:"   << total << "\n";
    cout << left << setw(20) << "Amount Paid:" << paid << "\n";
    cout << left << setw(20) << "Balance:"     << balance << "\n";
    cout << setfill('-') << setw(40) << "" << "\n" << setfill(' ');
}

// Defaulters (balance > 0), sorted by amount using bubble sort.
vector<string> getDefaulters() {
    vector<vector<string> > rows = readTXT(FEES_PATH);

    vector<string> rolls;
    vector<double> amounts;

    for (int i = 0; i < (int)rows.size(); i++) {
        double balance = textToDouble(rows[i][2]) - textToDouble(rows[i][3]);
        if (balance > 0) {
            rolls.push_back(rows[i][0]);
            amounts.push_back(balance);
        }
    }

    // bubble sort, largest balance first
    for (int i = 0; i < (int)rolls.size() - 1; i++) {
        for (int j = 0; j < (int)rolls.size() - 1 - i; j++) {
            if (amounts[j] < amounts[j + 1]) {
                double ta = amounts[j]; amounts[j] = amounts[j+1]; amounts[j+1] = ta;
                string tr = rolls[j]; rolls[j] = rolls[j+1]; rolls[j+1] = tr;
            }
        }
    }
    return rolls;
}
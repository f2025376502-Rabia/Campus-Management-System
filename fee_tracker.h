#ifndef FEE_TRACKER_H
#define FEE_TRACKER_H

#include <string>
#include <vector>
using namespace std;

// === Fee handling ===

// Records a payment after validating the date; updates amount_paid & paid_date.
bool recordPayment(string roll, string semester, double amount, string paidDate);

// Counts the number of days between two DD-MM-YYYY dates (no ctime).
int daysBetween(string date1, string date2);

// Late fine: 2% of the amount due for each complete week the payment is late.
double computeLateFine(string roll, string semester);

// Prints a formatted receipt (tuition, fine, total, paid, balance).
void generateReceipt(string roll, string semester);

// Returns rolls with a positive balance, sorted by amount using bubble sort.
vector<string> getDefaulters();

#endif
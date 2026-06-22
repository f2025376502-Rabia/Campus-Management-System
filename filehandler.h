#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <string>
#include <vector>
using namespace std;

// === File input/output helpers for the campus system ===

// Loads a txt file, ignores the first (header) line, and returns the data rows.
// Each row is split into its comma-separated fields.
vector<vector<string> > readTXT(string fileName);

// Rewrites the whole file: header line first, then every row.
// A field that contains a comma is enclosed in double quotes.
void writeTXT(string fileName, string header, vector<vector<string> > rows);

// Appends just one row to the end of the file (does not reload it).
void appendTXT(string fileName, vector<string> row);

// Linear search: returns the first row matching keyValue at column keyCol,
// or an empty vector when no match is found.
vector<string> findRow(string fileName, int keyCol, string keyValue);

// Returns true when at least one row holds 'value' at column colIndex.
bool rowExists(string fileName, int colIndex, string value);

#endif
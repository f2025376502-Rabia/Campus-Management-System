#include "filehandler.h"
#include <fstream>
#include <iostream>
using namespace std;

// Splits a single line into fields, reading one character at a time.
// Quoted fields are handled so a comma inside quotes does not split.
vector<string> breakIntoFields(string line) {
    vector<string> fields;
    string piece = "";
    bool inQuotes = false;

    for (int pos = 0; pos < (int)line.length(); pos++) {
        char ch = line[pos];

        if (ch == '"') {
            inQuotes = !inQuotes;          // toggle quote mode
        }
        else if (ch == ',' && inQuotes == false) {
            fields.push_back(piece);       // a comma ends the current field
            piece = "";
        }
        else {
            piece = piece + ch;            // keep building the field
        }
    }
    fields.push_back(piece);               // add the final field
    return fields;
}

// Reads a file, drops the header line, and returns the remaining data rows.
vector<vector<string> > readTXT(string fileName) {
    vector<vector<string> > data;
    ifstream in(fileName.c_str());

    if (!in.is_open()) {                   // file not found -> empty result
        return data;
    }

    string line;
    bool firstLine = true;
    while (getline(in, line)) {
        if (firstLine) {                   // skip the header
            firstLine = false;
            continue;
        }
        if (line.length() == 0) continue;  // ignore empty lines
        data.push_back(breakIntoFields(line));
    }
    in.close();
    return data;
}

// Wraps a field in quotes only when it contains a comma.
string guardField(string field) {
    bool commaInside = false;
    for (int i = 0; i < (int)field.length(); i++) {
        if (field[i] == ',') commaInside = true;
    }
    if (commaInside) return "\"" + field + "\"";
    return field;
}

// Overwrites the file with the header and all rows.
void writeTXT(string fileName, string header, vector<vector<string> > rows) {
    ofstream out(fileName.c_str());        // truncates the old file
    out << header << "\n";

    for (int r = 0; r < (int)rows.size(); r++) {
        for (int c = 0; c < (int)rows[r].size(); c++) {
            out << guardField(rows[r][c]);
            if (c < (int)rows[r].size() - 1) out << ",";
        }
        out << "\n";
    }
    out.close();
}

// Appends a single row using append mode (ios::app).
void appendTXT(string fileName, vector<string> row) {
    ofstream out(fileName.c_str(), ios::app);
    for (int c = 0; c < (int)row.size(); c++) {
        out << guardField(row[c]);
        if (c < (int)row.size() - 1) out << ",";
    }
    out << "\n";
    out.close();
}

// Returns the first row whose column keyCol equals keyValue.
vector<string> findRow(string fileName, int keyCol, string keyValue) {
    vector<vector<string> > rows = readTXT(fileName);
    for (int i = 0; i < (int)rows.size(); i++) {
        if (keyCol < (int)rows[i].size() && rows[i][keyCol] == keyValue) {
            return rows[i];
        }
    }
    vector<string> none;
    return none;
}

// Returns true if any row holds 'value' at column colIndex.
bool rowExists(string fileName, int colIndex, string value) {
    vector<vector<string> > rows = readTXT(fileName);
    for (int i = 0; i < (int)rows.size(); i++) {
        if (colIndex < (int)rows[i].size() && rows[i][colIndex] == value) {
            return true;
        }
    }
    return false;
}
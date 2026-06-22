# Campus-Management-System
C++ Programming Fundamentals Project - University campus system


## Features
- **Student Management** — add, search (by roll/name), soft-delete, list active students
- **Course Management** — enroll/drop with prerequisite & credit-load checks (max 21 hrs)
- **Attendance** — mark P/A/L, compute %, flag shortage (<75%), undo last session
- **Grades** — best 3 of 5 quizzes, weighted total, letter grade, GPA, class statistics
- **Fee Tracker** — record payments, late fines (manual date arithmetic), receipts
- **Reports** — merit list, attendance/fee defaulters, semester result, department summary, export to file

## How to Compile
Open a terminal in the project folder and run:

\`\`\`
g++ main.cpp filehandler.cpp student_ops.cpp course_ops.cpp attendance.cpp grades.cpp fee_tracker.cpp reports.cpp -o campus
\`\`\`

## How to Run
\`\`\`
./campus
\`\`\`

## Data Files
The system reads from and writes to these txt files: students.txt, courses.txt, enrollments.txt, attendance_log.txt, fees.txt, grades.txt

## Sample Run
\`\`\`
========================================
     CAMPUS ANALYTICS ENGINE
========================================
1. Student Management
2. Course Management
3. Attendance
4. Grades
5. Fee Tracker
6. Reports
7. Exit
Enter choice: 1
\`\`\`

## Author
Rabia (f2025376502)

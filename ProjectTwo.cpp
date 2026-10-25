#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Course {
    string courseNumber;
    string courseTitle;
    vector<string> prerequisites;
};

struct Node {
    Course course;
    Node* left;
    Node* right;
};

Node* root = nullptr;

string makeUpper(string text) {
    for (int i = 0; i < text.size(); i++) {
        text[i] = toupper(text[i]);
    }
    return text;
}

string cleanText(string text) {
    while (text.size() > 0 && (text.back() == '\r' || text.back() == ' ' || text.back() == '\n')) {
        text.pop_back();
    }
    while (text.size() > 0 && text.front() == ' ') {
        text.erase(0, 1);
    }
    return text;
}

void addCourse(Node*& node, Course course) {
    if (node == nullptr) {
        node = new Node;
        node->course = course;
        node->left = nullptr;
        node->right = nullptr;
    }
    else if (course.courseNumber < node->course.courseNumber) {
        addCourse(node->left, course);
    }
    else {
        addCourse(node->right, course);
    }
}

Course* findCourse(Node* node, string courseNumber) {
    while (node != nullptr) {
        if (node->course.courseNumber == courseNumber) {
            return &node->course;
        }
        else if (courseNumber < node->course.courseNumber) {
            node = node->left;
        }
        else {
            node = node->right;
        }
    }
    return nullptr;
}

void printInOrder(Node* node) {
    if (node == nullptr) {
        return;
    }
    printInOrder(node->left);
    cout << node->course.courseNumber << ", " << node->course.courseTitle << endl;
    printInOrder(node->right);
}

void deleteTree(Node* node) {
    if (node == nullptr) {
        return;
    }
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}

bool loadCourses(string fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Error: could not open file " << fileName << endl;
        return false;
    }

    deleteTree(root);
    root = nullptr;

    string line;
    int lineNumber = 0;
    int count = 0;

    while (getline(file, line)) {
        lineNumber++;
        line = cleanText(line);
        if (line == "") {
            continue;
        }

        vector<string> pieces;
        stringstream ss(line);
        string piece;
        while (getline(ss, piece, ',')) {
            pieces.push_back(cleanText(piece));
        }

        if (pieces.size() < 2 || pieces[0] == "" || pieces[1] == "") {
            cout << "Error: bad line " << lineNumber << ", skipping it" << endl;
            continue;
        }

        Course course;
        course.courseNumber = makeUpper(pieces[0]);
        course.courseTitle = pieces[1];
        for (int i = 2; i < pieces.size(); i++) {
            if (pieces[i] != "") {
                course.prerequisites.push_back(makeUpper(pieces[i]));
            }
        }

        addCourse(root, course);
        count++;
    }

    file.close();
    cout << count << " courses loaded." << endl;
    return true;
}

void printCourse(string courseNumber) {
    Course* course = findCourse(root, courseNumber);
    if (course == nullptr) {
        cout << "Course " << courseNumber << " not found." << endl;
        return;
    }

    cout << course->courseNumber << ", " << course->courseTitle << endl;

    if (course->prerequisites.size() == 0) {
        cout << "Prerequisites: none" << endl;
    }
    else {
        cout << "Prerequisites:" << endl;
        for (int i = 0; i < course->prerequisites.size(); i++) {
            Course* prereq = findCourse(root, course->prerequisites[i]);
            if (prereq != nullptr) {
                cout << "  " << prereq->courseNumber << ", " << prereq->courseTitle << endl;
            }
            else {
                cout << "  " << course->prerequisites[i] << endl;
            }
        }
    }
}

int main() {
    bool loaded = false;
    string choice = "";

    cout << "Welcome to the course planner." << endl;

    while (choice != "9") {
        cout << endl;
        cout << "1. Load Data Structure." << endl;
        cout << "2. Print Course List." << endl;
        cout << "3. Print Course." << endl;
        cout << "9. Exit" << endl;
        cout << "What would you like to do? ";
        getline(cin, choice);
        choice = cleanText(choice);

        if (choice == "1") {
            string fileName;
            cout << "Enter the file name: ";
            getline(cin, fileName);
            fileName = cleanText(fileName);
            if (loadCourses(fileName)) {
                loaded = true;
            }
        }
        else if (choice == "2") {
            if (loaded == false) {
                cout << "Please load the data first (option 1)." << endl;
            }
            else {
                cout << "Here is a sample schedule:" << endl;
                printInOrder(root);
            }
        }
        else if (choice == "3") {
            if (loaded == false) {
                cout << "Please load the data first (option 1)." << endl;
            }
            else {
                string courseNumber;
                cout << "What course do you want to know about? ";
                getline(cin, courseNumber);
                courseNumber = makeUpper(cleanText(courseNumber));
                printCourse(courseNumber);
            }
        }
        else if (choice == "9") {
            cout << "Thank you for using the course planner!" << endl;
        }
        else {
            cout << choice << " is not a valid option." << endl;
        }
    }

    deleteTree(root);
    return 0;
}
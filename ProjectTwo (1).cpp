#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <limits>
#include <stdexcept>

using namespace std;

// Course class to hold course information
class Course {
public:
    string courseNumber; // Course number (e.g., CS101)
    string courseTitle;  // Title of the course (e.g., Introduction to Computer Science)
    vector<string> prerequisites; // List of prerequisite course numbers

    // Default constructor
    Course() {}

    // Parameterized constructor to initialize course object
    Course(const string& number, const string& title, const vector<string>& prereqs)
        : courseNumber(number), courseTitle(title), prerequisites(prereqs) {}
};

// Utility functions for string manipulation
namespace StringUtils {
    // Function to split a string by a delimiter (used for parsing CSV lines)
    vector<string> split(const string& s, char delimiter) {
        vector<string> tokens; // Vector to store split tokens
        string token; // Temporary token holder
        istringstream tokenStream(s); // Stream to read the input string

        while (getline(tokenStream, token, delimiter)) {
            tokens.push_back(token); // Add each token to the vector
        }
        return tokens;
    }

    // Function to remove surrounding double quotes from a string (if present)
    string removeQuotes(const string& input) {
        if (!input.empty() && input.front() == '"' && input.back() == '"') {
            return input.substr(1, input.length() - 2);
        }
        return input; // Return the original string if no quotes
    }

    // Function to trim whitespace from a string
    string trim(const string& str) {
        size_t start = str.find_first_not_of(" \t");
        size_t end = str.find_last_not_of(" \t");
        return (start == string::npos) ? "" : str.substr(start, end - start + 1);
    }
}

// Function to display error messages
void displayError(const string& message) {
    cout << "Error: " << message << endl; // Output error message
}

// Function to validate user input for menu choices
int getValidatedInput() {
    int choice; // Variable to store user choice
    while (true) {
        cout << "What would you like to do? "; // Prompt for input
        cin >> choice;

        // Check for valid input
        if (!cin.fail()) {
            cin.clear(); // Clear error flags
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore leftover input
            return choice; // Return valid input
        }
        else {
            displayError("Invalid input. Please enter a number."); // Error message for invalid input
            cin.clear(); // Clear error flags
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore leftover input
        }
    }
}

// Function to load course data from a CSV file into a map
void loadCourses(const string& filename, unordered_map<string, Course>& courses) {
    ifstream file(filename); // Open the file

    if (!file.is_open()) {
        throw runtime_error("Could not open the file '" + filename + "'. Please check the file path and try again.");
    }

    string line; // Variable to hold each line read from the file
    while (getline(file, line)) {
        vector<string> tokens = StringUtils::split(line, ','); // Split line into tokens using commas
        if (tokens.size() < 2) {
            cout << "Warning: Skipping malformed line: " << line << " (Expected at least 2 tokens)" << endl; // Warn for malformed lines
            continue; // Skip to the next line
        }

        string courseNumber = StringUtils::trim(StringUtils::removeQuotes(tokens[0]));
        string courseTitle = StringUtils::trim(StringUtils::removeQuotes(tokens[1]));
        vector<string> prerequisites;

        for (size_t i = 2; i < tokens.size(); ++i) {
            prerequisites.push_back(StringUtils::trim(StringUtils::removeQuotes(tokens[i]))); // Add each prerequisite
        }

        // Check for duplicates before adding to the map
        if (courses.find(courseNumber) == courses.end()) {
            Course course(courseNumber, courseTitle, prerequisites); // Create a new Course object
            courses[courseNumber] = course; // Add to the map
        }
        else {
            cout << "Warning: Course " << courseNumber << " is already loaded." << endl; // Warn if duplicate found
        }
    }

    file.close(); // Close the file after reading

    if (courses.empty()) {
        cout << "Warning: No valid data was loaded. The file may be empty or malformed." << endl;
    }
    else {
        cout << "Data loaded successfully from '" << filename << "'." << endl; // Success message
    }
}

// Function to save course data back to a CSV file
void saveCourses(const string& filename, const unordered_map<string, Course>& courses) {
    ofstream file(filename); // Open the file for writing

    if (!file.is_open()) {
        throw runtime_error("Could not open the file '" + filename + "' for writing.");
    }

    for (const auto& entry : courses) {
        const Course& course = entry.second;
        file << "\"" << course.courseNumber << "\", \"" << course.courseTitle << "\"";
        for (const auto& prereq : course.prerequisites) {
            file << ", \"" << prereq << "\""; // Write prerequisites
        }
        file << endl; // New line after each course
    }

    file.close(); // Close the file after writing

    // Check if the file has been written successfully
    ifstream checkFile(filename);
    if (!checkFile.is_open()) {
        throw runtime_error("Error: Could not verify the saved data. The file may be corrupted or inaccessible.");
    }

    cout << "Data saved successfully to '" << filename << "'." << endl; // Success message
}



void printCourseList(const unordered_map<string, Course>& courses) {
    if (courses.empty()) {
        cout << "No courses available. Please load the data structure first." << endl;
        return;
    }

    vector<const Course*> coursePointers;
    for (const auto& course : courses) {
        coursePointers.push_back(&course.second);
    }

    // Sort courses by course number
    sort(coursePointers.begin(), coursePointers.end(), [](const Course* a, const Course* b) {
        return a->courseNumber < b->courseNumber;
        });

    cout << "Course List:" << endl;
    for (const auto* coursePtr : coursePointers) {
        cout << coursePtr->courseNumber << ", " << coursePtr->courseTitle;

        // Check if prerequisites are empty
        if (coursePtr->prerequisites.empty() ||
            (coursePtr->prerequisites.size() == 1 && coursePtr->prerequisites[0] == "")) {
            cout << " (Prerequisites: None)";  // If prerequisites are empty or only an empty string, print "None"
        }
        else {
            // Otherwise, print the valid prerequisites
            cout << " (Prerequisites: ";
            for (size_t i = 0; i < coursePtr->prerequisites.size(); ++i) {
                if (!coursePtr->prerequisites[i].empty()) {  // Skip empty prerequisites
                    if (i > 0) cout << ", ";  // Add comma between prerequisites
                    cout << coursePtr->prerequisites[i];
                }
            }
            cout << ")";
        }

        cout << endl;  // New line after each course
    }
}



void printCourseInfo(const unordered_map<string, Course>& courses, const string& courseNumber) {
    if (courseNumber.empty()) {
        displayError("Course number cannot be empty."); // Use the displayError function
        return; // Exit function if course number is empty
    }

    auto it = courses.find(courseNumber); // Look for the course in the map
    if (it != courses.end()) { // Check if the course exists
        const Course& course = it->second; // Reference to the found course
        cout << course.courseNumber << ", " << course.courseTitle << endl; // Output course details

        // Print prerequisites
        if (!course.prerequisites.empty()) {
            cout << "Prerequisites: ";
            for (size_t i = 0; i < course.prerequisites.size(); ++i) {
                if (i > 0) cout << ", "; // Add comma separator for multiple prerequisites
                cout << course.prerequisites[i]; // Output prerequisite course numbers
            }
            cout << endl; // New line after listing prerequisites
        }
        else {
            cout << "Prerequisites: None" << endl; // Changed to "None" when there are no prerequisites
        }
    }
    else {
        displayError("Course '" + courseNumber + "' not found."); // Use the displayError function
    }
}


// Main menu for the program
void displayMenu() {
    cout << "\nMain Menu:" << endl;
    cout << "1. Load Data Structure" << endl; // Option to load course data
    cout << "2. Print Course List" << endl; // Option to print all courses
    cout << "3. Print Course Information" << endl; // Option to print details of a specific course
    cout << "4. Insert Course" << endl; // New option for inserting a course
    cout << "5. Save Data Structure" << endl; // Option to save course data
    cout << "9. Exit" << endl; // Option to exit the program
}

// Function to get the file name from the user
string getFileName() {
    string filename; // Variable to hold filename input
    cout << "Enter the filename: "; // Prompt for filename
    getline(cin >> ws, filename); // Read filename input
    filename = StringUtils::trim(filename); // Trim whitespace from input

    // Remove surrounding quotes if present
    if (!filename.empty() && filename.front() == '"' && filename.back() == '"') {
        filename = filename.substr(1, filename.length() - 2); // Remove quotes
    }

    return filename; // Return the cleaned filename
}

// Function to get a valid course number from the user
string getCourseNumber() {
    string input; // Input buffer for course numbers
    cout << "Enter the course number: ";
    getline(cin >> ws, input); // Use getline to read the course number with spaces and trim whitespace
    if (input.empty()) {
        throw invalid_argument("Course number cannot be empty."); // Throw error if empty
    }
    return StringUtils::trim(input); // Trim whitespace from input
}

// Function to insert a new course into the course map
void insertCourse(unordered_map<string, Course>& courses) {
    string courseNumber = getCourseNumber(); // Get course number
    string courseTitle;
    cout << "Enter the course title: "; // Prompt for course title
    getline(cin >> ws, courseTitle); // Use getline to allow spaces

    vector<string> prerequisites; // Vector to hold prerequisites
    int numPrereqs; // Number of prerequisites
    cout << "How many prerequisites does the course have? ";
    cin >> numPrereqs;

    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer before taking further input

    for (int i = 0; i < numPrereqs; ++i) {
        string prereq;
        cout << "Enter prerequisite " << (i + 1) << ": "; // Prompt for each prerequisite
        getline(cin >> ws, prereq); // Use getline to allow spaces
        prerequisites.push_back(StringUtils::trim(prereq)); // Add trimmed prerequisite to vector
    }

    // Create a new Course object and insert it into the map
    Course newCourse(courseNumber, courseTitle, prerequisites);
    if (courses.find(courseNumber) == courses.end()) { // Check for duplicates
        courses[courseNumber] = newCourse; // Add to the courses map
        cout << "Course " << courseNumber << " has been successfully added." << endl; // Confirmation message
    }
    else {
        displayError("Course " + courseNumber + " already exists."); // Error for duplicate courses
    }
}



// Main function
int main() {
    unordered_map<string, Course> courses; // Map to hold courses
    int choice;

    do {
        displayMenu(); // Display the main menu
        choice = getValidatedInput(); // Get validated menu choice

        switch (choice) {
        case 1: {
            try {
                string filename = getFileName(); // Get filename from user
                loadCourses(filename, courses); // Load course data
            }
            catch (const exception& e) {
                displayError(e.what()); // Handle exceptions with error messages
            }
            break;
        }
        case 2: {
            printCourseList(courses); // Print the list of courses
            break;
        }
        case 3: {
            string courseNumber = getCourseNumber(); // Get course number from user
            printCourseInfo(courses, courseNumber); // Print detailed information for the course
            break;
        }
        case 4: {
            insertCourse(courses); // Call the insert function
            break;
        }
        case 5: {
            try {
                string filename = getFileName(); // Get filename from user
                saveCourses(filename, courses); // Save course data
            }
            catch (const exception& e) {
                displayError(e.what()); // Handle exceptions with error messages
            }
            break;
        }
        case 9:
            cout << "Exiting program." << endl; // Exit message
            break;
        default:
            cout << "Invalid choice. Please select a valid option." << endl; // Error for invalid choice
        }
    } while (choice != 9); // Loop until user chooses to exit

    return 0; // Return success
}

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

using namespace std;

// A simple hash function for passwords
string hashPassword(const string& password) {
    hash<string> hasher;
    size_t hashedValue = hasher(password);
    stringstream ss;
    ss << hex << hashedValue; // Convert to hexadecimal
    return ss.str();
}

// Validate username
bool isValidUsername(const string& username) {
    return !username.empty() && username.length() >= 3; // Minimum length of 3
}

// Validate password complexity
bool isStrongPassword(const string& password) {
    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;

    for (char ch : password) {
        if (isupper(ch)) hasUpper = true;
        if (islower(ch)) hasLower = true;
        if (isdigit(ch)) hasDigit = true;
        if (!isalnum(ch)) hasSpecial = true;
    }

    return password.length() >= 8 && hasUpper && hasLower && hasDigit && hasSpecial;
}

// Load users from file into memory
void loadUsers(unordered_map<string, string>& users, unordered_map<string, bool>& emailVerified) {
    ifstream inFile("users.txt");
    if (inFile.is_open()) {
        string username, hashedPassword, verified;
        while (inFile >> username >> hashedPassword >> verified) {
            users[username] = hashedPassword;
            emailVerified[username] = (verified == "true");
        }
        inFile.close();
    }
}

// Save users to file
void saveUsers(const unordered_map<string, string>& users, const unordered_map<string, bool>& emailVerified) {
    ofstream outFile("users.txt");
    if (outFile.is_open()) {
        for (const auto& user : users) {
            outFile << user.first << " " << user.second << " "
                << (emailVerified.at(user.first) ? "true" : "false") << "\n";
        }
        outFile.close();
    }
    else {
        cout << "Error: Unable to save user data.\n";
    }
}

// Register a new user
void registerUser(unordered_map<string, string>& users, unordered_map<string, bool>& emailVerified) {
    string username, password, email;

    // Username validation
    while (true) {
        cout << "Enter a username (minimum 3 characters): ";
        cin >> username;

        if (isValidUsername(username)) {
            if (users.find(username) != users.end()) {
                cout << "Username already exists! Please try again.\n";
            }
            else {
                break; // Valid username
            }
        }
        else {
            cout << "Invalid username! Username must be at least 3 characters long.\n";
        }
    }

    // Password validation with retry mechanism
    while (true) {
        cout << "Enter a password (must be at least 8 characters long, include uppercase, lowercase, digits, and one special character): ";
        cin >> password;

        if (isStrongPassword(password)) {
            break; // Valid password
        }
        else {
            cout << "Password does not meet the criteria. Please try again.\n";
            cout << "Requirements:\n";
            cout << "- At least 8 characters long\n";
            cout << "- At least one uppercase letter\n";
            cout << "- At least one lowercase letter\n";
            cout << "- At least one digit\n";
            cout << "- At least one special character\n";
        }
    }

    // Email input
    cout << "Enter your email: ";
    cin >> email;

    // Simulate email verification
    cout << "A verification link has been sent to " << email << ". Please verify your email.\n";
    emailVerified[username] = false; // Mark as unverified initially

    // Hash the password and store the user
    string hashedPassword = hashPassword(password);
    users[username] = hashedPassword;

    saveUsers(users, emailVerified);
    cout << "Registration successful! Please verify your email.\n";
}

// Login a user
void loginUser(const unordered_map<string, string>& users, const unordered_map<string, bool>& emailVerified,
    unordered_map<string, int>& failedAttempts) {
    string username, password;
    cout << "Enter your username: ";
    cin >> username;

    auto it = users.find(username);
    if (it == users.end()) {
        cout << "Username not found! Please register or try again.\n";
        return;
    }

    // Check for account lockout
    if (failedAttempts[username] >= 3) {
        cout << "Account locked due to too many failed attempts. Please contact support.\n";
        return;
    }

    // Check email verification
    if (!emailVerified.at(username)) {
        cout << "Email not verified! Please verify your email before logging in.\n";
        return;
    }

    cout << "Enter your password: ";
    cin >> password;

    string hashedPassword = hashPassword(password);
    if (it->second == hashedPassword) {
        cout << "Login successful! Welcome, " << username << ".\n";
        failedAttempts[username] = 0; // Reset failed attempts on successful login
    }
    else {
        cout << "Incorrect password! Please try again.\n";
        failedAttempts[username]++;
    }
}

// Reset a user's password
void resetPassword(unordered_map<string, string>& users, unordered_map<string, bool>& emailVerified) {
    string username, oldPassword, newPassword;
    cout << "Enter your username: ";
    cin >> username;

    auto it = users.find(username);
    if (it == users.end()) {
        cout << "Username not found! Please try again.\n";
        return;
    }

    // Check email verification
    if (!emailVerified.at(username)) {
        cout << "Email not verified! Please verify your email before resetting your password.\n";
        return;
    }

    cout << "Enter your current password: ";
    cin >> oldPassword;

    string hashedOldPassword = hashPassword(oldPassword);
    if (it->second != hashedOldPassword) {
        cout << "Incorrect current password! Password reset failed.\n";
        return;
    }

    // New password validation with retry mechanism
    while (true) {
        cout << "Enter your new password (must be at least 8 characters long, include uppercase, lowercase, digits, and one special character): ";
        cin >> newPassword;

        if (isStrongPassword(newPassword)) {
            break; // Valid password
        }
        else {
            cout << "Password does not meet the criteria. Please try again.\n";
            cout << "Requirements:\n";
            cout << "- At least 8 characters long\n";
            cout << "- At least one uppercase letter\n";
            cout << "- At least one lowercase letter\n";
            cout << "- At least one digit\n";
            cout << "- At least one special character\n";
        }
    }

    string hashedNewPassword = hashPassword(newPassword);
    it->second = hashedNewPassword;

    saveUsers(users, emailVerified);
    cout << "Password reset successful!\n";
}

// Simulate email verification
void verifyEmail(unordered_map<string, bool>& emailVerified) {
    string username;
    cout << "Enter your username to verify your email: ";
    cin >> username;

    if (emailVerified.find(username) == emailVerified.end()) {
        cout << "Username not found! Please try again.\n";
        return;
    }

    emailVerified[username] = true;
    cout << "Email verified successfully!\n";
}

int main() {
    unordered_map<string, string> users;         // In-memory storage for users
    unordered_map<string, bool> emailVerified;   // Track email verification status
    unordered_map<string, int> failedAttempts;   // Track failed login attempts

    loadUsers(users, emailVerified);             // Load existing users from file

    while (true) {
        cout << "\n--- Login and Registration System ---\n";
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "3. Reset Password\n";
        cout << "4. Verify Email\n";
        cout << "5. Exit\n";
        cout << "Choose an option: ";

        int choice;
        cin >> choice;

        switch (choice) {
        case 1:
            registerUser(users, emailVerified);
            break;
        case 2:
            loginUser(users, emailVerified, failedAttempts);
            break;
        case 3:
            resetPassword(users, emailVerified);
            break;
        case 4:
            verifyEmail(emailVerified);
            break;
        case 5:
            cout << "Exiting the system. Goodbye!\n";
            return 0;
        default:
            cout << "Invalid option! Please try again.\n";
        }
    }

    return 0;
}
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <limits>
#include <iomanip>

struct Payment {
    double amount;
    std::string date;
};

struct User {
    std::string username;
    std::string password;
    std::vector<Payment> paymentHistory;
};

const std::string validUsername = "Mark";
const std::string validPassword = "12345";

bool login(const std::string& username, const std::string& password) {
    return username == validUsername && password == validPassword;
}

bool processPayment(double amount) {
    if (amount != 1500) {
        std::cerr << "Error: Please Enter an exact amount of $1500" << std::endl;
        return false;
    }
    return true;
}

void displayReceipt(double amount) {
    std::cout << "\n========= Receipt =========" << std::endl;
    std::cout << "Amount: $" << std::fixed << std::setprecision(2) << amount << std::endl;
    std::cout << "Thank you for your payment!" << std::endl;
    std::cout << "===========================" << std::endl;
}

void viewPaymentHistory(const User& user) {
    if (user.paymentHistory.empty()) {
        std::cout << "**********************************" << std::endl;
        std::cout << "*      No payments made yet.     *" << std::endl;
        std::cout << "**********************************" << std::endl;
    } else {
        std::cout << "\n========== Payment History =========" << std::endl;
        for (size_t i = 0; i < user.paymentHistory.size(); ++i) {
            const Payment& payment = user.paymentHistory[i];
            std::cout << "Payment " << (i + 1) << ": $"
                      << std::fixed << std::setprecision(2) << payment.amount
                      << " on " << payment.date << std::endl;
        }
        std::cout << "====================================" << std::endl;
    }
}

void loadPaymentHistory(std::vector<User>& users) {
    std::ifstream inFile("payment_data.txt");
    if (!inFile) {
        std::cerr << "Error: Could not open file for reading payment data." << std::endl;
        return;
    }

    std::string line;
    User user;

    while (std::getline(inFile, line)) {
        if (line.find("User: ") == 0) {
            if (!user.username.empty()) {
                users.push_back(user);
            }
            user.username = line.substr(6);
            user.paymentHistory.clear();
        } else if (line.find("Amount: $") == 0) {
            Payment payment;
            std::string amount_str = line.substr(9);
            std::stringstream(amount_str) >> payment.amount;
            std::getline(inFile, line);
            payment.date = line.substr(6);
            user.paymentHistory.push_back(payment);
        }
    }
    if (!user.username.empty()) {
        users.push_back(user);
    }

    inFile.close();
}
void savePaymentHistory(const std::vector<User>& users) {
    std::ofstream outFile("payment_data.txt", std::ofstream::trunc);
    if (!outFile) {
        std::cerr << "Error: Could not open file for saving payment data." << std::endl;
        return;
    }
    for (const auto& user : users) {
        outFile << "User: " << user.username << "\n";
        for (const auto& payment : user.paymentHistory) {
            outFile << "==============================\n";
            outFile << "Amount: $" << std::fixed << std::setprecision(2) << payment.amount << "\n";
            outFile << "Date: " << payment.date << "\n";
            outFile << "--------------------------------\n";
        }
    }

    outFile.close();
    std::cout << "Payment history saved to payment_data.txt successfully!" << std::endl;
}

int main() {
    std::string username, password;
    int loginAttempts = 0;
    const int maxLoginAttempts = 3;

    std::vector<User> users;
    loadPaymentHistory(users);

    User* loggedInUser = nullptr;
    if (!users.empty()) {
        loggedInUser = &users[0];
    }
    if (!loggedInUser) {
        User newUser;
        newUser.username = validUsername;
        newUser.password = validPassword;
        users.push_back(newUser);
        loggedInUser = &users.back();
    }
    while (loginAttempts < maxLoginAttempts) {
        std::cout << "--------------------------------------" << std::endl;
        std::cout << "Log in account to enter Rental Payment" << std::endl;
        std::cout << "--------------------------------------" << std::endl;
        std::cout << "Enter username: ";
        std::cin >> username;

        std::cout << "Enter password: ";
        std::cin >> password;

        if (login(username, password)) {
            std::cout << "Login successful!" << std::endl;
            break;
        } else {
            loginAttempts++;
            std::cerr << "Invalid username or password. Attempt " << loginAttempts << " of " << maxLoginAttempts << "." << std::endl;
            if (loginAttempts == maxLoginAttempts) {
                std::cerr << "Maximum login attempts reached. Access denied." << std::endl;
                return 1;
            }
        }
    }


    bool exitSystem = false;
    while (!exitSystem) {
        std::cout << "\n=== Welcome to the Rental Payment ===" << std::endl;
        std::cout << "1. Make a Payment" << std::endl;
        std::cout << "2. View Payment History" << std::endl;
        std::cout << "3. Exit" << std::endl;
        std::cout << "Please select an option (1-3): ";

        int choice;
        std::cin >> choice;


        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cerr << "Invalid choice. Please select a valid option." << std::endl;
            continue;
        }

        switch (choice) {
            case 1: {
                double amount;
                std::string date;
                bool paymentSuccessful = false;

                while (!paymentSuccessful) {
                    std::cout << "Enter payment amount: $";
                    std::cin >> amount;

                    if (std::cin.fail()) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::cerr << "Invalid input! Please enter a valid numeric value for the payment amount." << std::endl;
                    } else {
                        if (processPayment(amount)) {
                            std::cout << "Enter date of payment (MM/DD/YYYY): ";
                            std::cin >> date;
                            Payment newPayment = {amount, date};
                            loggedInUser->paymentHistory.push_back(newPayment);
                            displayReceipt(amount);

                            savePaymentHistory(users);

                            paymentSuccessful = true;
                        }
                    }
                }
                break;
            }
            case 2: {
                viewPaymentHistory(*loggedInUser);
                break;
            }
            case 3: {
                std::cout << "Exiting the system.......!" << std::endl;
                savePaymentHistory(users);
                exitSystem = true;
                break;
            }
            default:
                std::cerr << "Invalid choice. Please select a valid option." << std::endl;
        }
    }

    return 0;
}

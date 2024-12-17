#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <iomanip>

using namespace std;

class User {
public:
    string username;
    string password;
    double balance;

    User() : balance(0.0) {}  // Default constructor for deserialization

    User(string uname, string pwd) : username(uname), password(pwd), balance(0.0) {}

    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            cout << "Deposited $" << fixed << setprecision(2) << amount << ". New balance: $" << balance << endl;
        } else {
            cout << "Amount must be greater than zero." << endl;
        }
    }

    void withdraw(double amount) {
        if (amount <= balance) {
            balance -= amount;
            cout << "Withdrew $" << fixed << setprecision(2) << amount << ". New balance: $" << balance << endl;
        } else {
            cout << "Insufficient balance." << endl;
        }
    }

    void checkBalance() const {
        cout << "Balance: $" << fixed << setprecision(2) << balance << endl;
    }

    string closeAccount() const {
        return "Account " + username + " closed.";
    }
};

class BankingSystem {
private:
    unordered_map<string, User> users;

    void loadUsers() {
        ifstream infile("users.dat", ios::binary);
        if (infile.is_open()) {
            size_t size;
            infile.read(reinterpret_cast<char*>(&size), sizeof(size));
            for (size_t i = 0; i < size; ++i) {
                User user;
                size_t unameLen, pwdLen;
                infile.read(reinterpret_cast<char*>(&unameLen), sizeof(unameLen));
                user.username.resize(unameLen);
                infile.read(&user.username[0], unameLen);

                infile.read(reinterpret_cast<char*>(&pwdLen), sizeof(pwdLen));
                user.password.resize(pwdLen);
                infile.read(&user.password[0], pwdLen);

                infile.read(reinterpret_cast<char*>(&user.balance), sizeof(user.balance));

                users[user.username] = user;
            }
            infile.close();
        }
    }

    void saveUsers() {
        ofstream outfile("users.dat", ios::binary);
        if (outfile.is_open()) {
            size_t size = users.size();
            outfile.write(reinterpret_cast<const char*>(&size), sizeof(size));
            for (const auto& pair : users) {
                const User& user = pair.second;
                size_t unameLen = user.username.size();
                size_t pwdLen = user.password.size();

                outfile.write(reinterpret_cast<const char*>(&unameLen), sizeof(unameLen));
                outfile.write(user.username.c_str(), unameLen);

                outfile.write(reinterpret_cast<const char*>(&pwdLen), sizeof(pwdLen));
                outfile.write(user.password.c_str(), pwdLen);

                outfile.write(reinterpret_cast<const char*>(&user.balance), sizeof(user.balance));
            }
            outfile.close();
        }
    }

public:
    BankingSystem() {
        loadUsers();
    }

    ~BankingSystem() {
        saveUsers();
    }

    void registerUser(const string& username, const string& password) {
        if (users.find(username) != users.end()) {
            cout << "Username already exists!" << endl;
        } else {
            users[username] = User(username, password);
            saveUsers();
            cout << "User " << username << " registered successfully!" << endl;
        }
    }

    User* login(const string& username, const string& password) {
        auto it = users.find(username);
        if (it != users.end() && it->second.password == password) {
            cout << "Welcome back, " << username << "!" << endl;
            return &it->second;
        } else {
            cout << "Invalid username or password." << endl;
            return nullptr;
        }
    }

    void deleteUser(const string& username) {
        users.erase(username);
        saveUsers();
    }
};

void displayMainMenu() {
    cout << "\nBanking System" << endl;
    cout << "1. Register" << endl;
    cout << "2. Login" << endl;
    cout << "3. Exit" << endl;
    cout << "Choose an option: ";
}

void displayUserMenu() {
    cout << "\n1. Deposit" << endl;
    cout << "2. Withdraw" << endl;
    cout << "3. Check Balance" << endl;
    cout << "4. Close Account" << endl;
    cout << "5. Logout" << endl;
    cout << "Choose an action: ";
}

int main() {
    BankingSystem bankingSystem;
    string username, password;

    while (true) {
        displayMainMenu();
        int choice;
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            cout << "Enter username: ";
            getline(cin, username);
            cout << "Enter password: ";
            getline(cin, password);
            bankingSystem.registerUser(username, password);
        } else if (choice == 2) {
            cout << "Enter username: ";
            getline(cin, username);
            cout << "Enter password: ";
            getline(cin, password);
            User* user = bankingSystem.login(username, password);

            if (user) {
                while (true) {
                    displayUserMenu();
                    int action;
                    cin >> action;
                    cin.ignore();

                    if (action == 1) {
                        double amount;
                        cout << "Enter amount to deposit: $";
                        cin >> amount;
                        user->deposit(amount);
                    } else if (action == 2) {
                        double amount;
                        cout << "Enter amount to withdraw: $";
                        cin >> amount;
                        user->withdraw(amount);
                    } else if (action == 3) {
                        user->checkBalance();
                    } else if (action == 4) {
                        cout << user->closeAccount() << endl;
                        bankingSystem.deleteUser(user->username);
                        break;
                    } else if (action == 5) {
                        cout << "Logged out successfully." << endl;
                        break;
                    } else {
                        cout << "Invalid option." << endl;
                    }
                }
            }
        } else if (choice == 3) {
            cout << "Exiting system..." << endl;
            break;
        } else {
            cout << "Invalid choice." << endl;
        }
    }

    return 0;
}
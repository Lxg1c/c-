#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <algorithm>
#include <stdexcept>

// Исключения
class InvalidInputException : public std::runtime_error {
public:
    InvalidInputException(const std::string& msg) : std::runtime_error(msg) {}
};

class FileException : public std::runtime_error {
public:
    FileException(const std::string& msg) : std::runtime_error(msg) {}
};

class User {
protected:
    std::string name;
    int id;
    int accessLevel;

    void validate() const {
        if (name.empty()) throw InvalidInputException("User name cannot be empty");
        if (id < 0) throw InvalidInputException("User ID cannot be negative");
        if (accessLevel < 0) throw InvalidInputException("Access level cannot be negative");
    }

public:
    User(const std::string& name, int id, int accessLevel)
        : name(name), id(id), accessLevel(accessLevel) {
        validate();
    }

    virtual void displayInfo() const {
        std::cout << "Name: " << name << ", ID: " << id
            << ", Access Level: " << accessLevel;
    }

    virtual void saveToFile(std::ofstream& out) const {
        out << "User " << name << " " << id << " " << accessLevel << "\n";
    }

    std::string getName() const { return name; }
    int getId() const { return id; }
    int getAccessLevel() const { return accessLevel; }

    virtual ~User() = default;
};

class Student : public User {
private:
    int group;

public:
    Student(const std::string& name, int id, int accessLevel, int group)
        : User(name, id, accessLevel), group(group) {
    }

    void displayInfo() const override {
        User::displayInfo();
        std::cout << ", Group: " << group << " (Student)" << std::endl;
    }

    void saveToFile(std::ofstream& out) const override {
        out << "Student " << name << " " << id << " " << accessLevel << " " << group << "\n";
    }
};

class Teacher : public User {
private:
    std::string department;

public:
    Teacher(const std::string& name, int id, int accessLevel, const std::string& department)
        : User(name, id, accessLevel), department(department) {
    }

    void displayInfo() const override {
        User::displayInfo();
        std::cout << ", Department: " << department << " (Teacher)" << std::endl;
    }

    void saveToFile(std::ofstream& out) const override {
        out << "Teacher " << name << " " << id << " " << accessLevel << " " << department << "\n";
    }
};

class Administrator : public User {
private:
    std::string adminKey;

public:
    Administrator(const std::string& name, int id, int accessLevel, const std::string& key)
        : User(name, id, accessLevel), adminKey(key) {
    }

    void displayInfo() const override {
        User::displayInfo();
        std::cout << " (Administrator)" << std::endl;
    }

    void saveToFile(std::ofstream& out) const override {
        out << "Administrator " << name << " " << id << " " << accessLevel << " " << adminKey << "\n";
    }
};

class Resource {
private:
    std::string name;
    int requiredAccessLevel;

    void validate() const {
        if (name.empty()) throw InvalidInputException("Resource name cannot be empty");
        if (requiredAccessLevel < 0) throw InvalidInputException("Required access level cannot be negative");
    }

public:
    Resource(const std::string& name, int requiredAccessLevel)
        : name(name), requiredAccessLevel(requiredAccessLevel) {
        validate();
    }

    bool checkAccess(const User& user) const {
        return user.getAccessLevel() >= requiredAccessLevel;
    }

    void saveToFile(std::ofstream& out) const {
        out << "Resource " << name << " " << requiredAccessLevel << "\n";
    }

    std::string getName() const { return name; }
    int getRequiredAccessLevel() const { return requiredAccessLevel; }
};

template<typename T>
class AccessControlSystem {
private:
    std::vector<std::unique_ptr<User>> users;
    std::vector<T> resources;

public:
    void addUser(std::unique_ptr<User> user) {
        users.push_back(std::move(user));
    }

    void addResource(const T& resource) {
        resources.push_back(resource);
    }

    bool checkAccess(int userId, const std::string& resourceName) const {
        auto userIt = std::find_if(users.begin(), users.end(),
            [userId](const auto& user) { return user->getId() == userId; });

        auto resIt = std::find_if(resources.begin(), resources.end(),
            [resourceName](const auto& res) { return res.getName() == resourceName; });

        if (userIt == users.end()) throw std::runtime_error("User not found");
        if (resIt == resources.end()) throw std::runtime_error("Resource not found");

        return resIt->checkAccess(**userIt);
    }

    void displayAllUsers() const {
        for (const auto& user : users) {
            user->displayInfo();
        }
    }

    void displayAllResources() const {
        for (const auto& res : resources) {
            std::cout << "Resource: " << res.getName()
                << ", Required Access: " << res.getRequiredAccessLevel() << std::endl;
        }
    }

    void saveToFile(const std::string& filename) const {
        std::ofstream out(filename);
        if (!out) throw FileException("Cannot open file for writing");

        for (const auto& user : users) {
            user->saveToFile(out);
        }

        for (const auto& res : resources) {
            res.saveToFile(out);
        }
    }

    void loadFromFile(const std::string& filename) {
        std::ifstream in(filename);
        if (!in) throw FileException("Cannot open file for reading");

        users.clear();
        resources.clear();

        std::string type;
        while (in >> type) {
            if (type == "User") {
                std::string name;
                int id, accessLevel;
                in >> name >> id >> accessLevel;
                users.push_back(std::make_unique<User>(name, id, accessLevel));
            }
            else if (type == "Student") {
                std::string name;
                int id, accessLevel, group;
                in >> name >> id >> accessLevel >> group;
                users.push_back(std::make_unique<Student>(name, id, accessLevel, group));
            }
            else if (type == "Teacher") {
                std::string name, department;
                int id, accessLevel;
                in >> name >> id >> accessLevel >> department;
                users.push_back(std::make_unique<Teacher>(name, id, accessLevel, department));
            }
            else if (type == "Administrator") {
                std::string name, key;
                int id, accessLevel;
                in >> name >> id >> accessLevel >> key;
                users.push_back(std::make_unique<Administrator>(name, id, accessLevel, key));
            }
            else if (type == "Resource") {
                std::string name;
                int requiredAccessLevel;
                in >> name >> requiredAccessLevel;
                resources.emplace_back(name, requiredAccessLevel);
            }
        }
    }

    std::vector<User*> findUsersByName(const std::string& name) const {
        std::vector<User*> result;
        for (const auto& user : users) {
            if (user->getName() == name) {
                result.push_back(user.get());
            }
        }
        return result;
    }

    User* findUserById(int id) const {
        auto it = std::find_if(users.begin(), users.end(),
            [id](const auto& user) { return user->getId() == id; });
        return it != users.end() ? it->get() : nullptr;
    }

    void sortUsersByAccessLevel() {
        std::sort(users.begin(), users.end(),
            [](const auto& a, const auto& b) {
                return a->getAccessLevel() < b->getAccessLevel();
            });
    }

    void sortUsersById() {
        std::sort(users.begin(), users.end(),
            [](const auto& a, const auto& b) {
                return a->getId() < b->getId();
            });
    }
};

int main() {
    try {
        AccessControlSystem<Resource> system;

        // Добавление пользователей
        system.addUser(std::make_unique<Student>("Nick Teran", 1, 1, 101));
        system.addUser(std::make_unique<Teacher>("Ms. Brown", 2, 3, "Computer Science"));
        system.addUser(std::make_unique<Administrator>("Mr. Smith", 3, 5, "admin123"));

        // Добавление ресурсов
        system.addResource(Resource("Classroom 101", 1));
        system.addResource(Resource("Computer Lab", 3));
        system.addResource(Resource("Main Library", 2));
        system.addResource(Resource("Server Room", 5));

        // Демонстрация полиморфизма
        std::cout << "=== All Users ===" << std::endl;
        system.displayAllUsers();

        std::cout << "\n=== All Resources ===" << std::endl;
        system.displayAllResources();

        // Проверка доступа
        std::cout << "\n=== Access Checks ===" << std::endl;
        std::cout << "User 1 access to Computer Lab: "
            << (system.checkAccess(1, "Computer Lab") ? "Granted" : "Denied") << std::endl;
        std::cout << "User 2 access to Server Room: "
            << (system.checkAccess(2, "Server Room") ? "Granted" : "Denied") << std::endl;

        // Поиск пользователей
        std::cout << "\n=== Search ===" << std::endl;
        auto users = system.findUsersByName("Nick");
        if (!users.empty()) {
            std::cout << "Found users with name 'Nick':" << std::endl;
            for (auto user : users) {
                user->displayInfo();
            }
        }

        // Сортировка
        std::cout << "\n=== Sorted by Access Level ===" << std::endl;
        system.sortUsersByAccessLevel();
        system.displayAllUsers();

        // Файловый ввод-вывод
        std::cout << "\n=== File I/O ===" << std::endl;
        system.saveToFile("system_data.txt");

        AccessControlSystem<Resource> newSystem;
        newSystem.loadFromFile("system_data.txt");
        std::cout << "Loaded system:" << std::endl;
        newSystem.displayAllUsers();
        newSystem.displayAllResources();

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
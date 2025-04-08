#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <stdexcept>

// Базовый класс сущности
class Entity {
public:
    virtual std::string getName() const = 0;
    virtual int getHealth() const = 0;
    virtual int getLevel() const = 0;
    virtual void displayInfo() const = 0;
    virtual ~Entity() = default;
};

// Пример класса Player
class Player : public Entity {
    std::string name;
    int health;
    int level;
public:
    Player(const std::string& name, int health, int level)
        : name(name), health(health), level(level) {
    }

    std::string getName() const override {
        return name;
    }

    int getHealth() const override {
        return health;
    }

    int getLevel() const override {
        return level;
    }

    void displayInfo() const override {
        std::cout << "Player: " << name << ", Health: " << health << ", Level: " << level << '\n';
    }
};

// Шаблонный класс GameManager
template <typename T>
class GameManager {
    std::vector<T> entities;
public:
    void addEntity(T entity) {
        entities.push_back(entity);
    }

    void displayAll() const {
        for (const auto& entity : entities) {
            entity->displayInfo();
        }
    }

    const std::vector<T>& getEntities() const {
        return entities;
    }
};

// Сохранение данных в файл
void saveToFile(const GameManager<std::shared_ptr<Entity>>& manager, const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to open file for writing.");
    }

    for (const auto& entity : manager.getEntities()) {
        file << entity->getName() << " " << entity->getHealth() << " " << entity->getLevel() << "\n";
    }
}

// Загрузка данных из файла
void loadFromFile(GameManager<std::shared_ptr<Entity>>& manager, const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to open file for reading.");
    }

    std::string name;
    int health, level;

    while (file >> name >> health >> level) {
        manager.addEntity(std::make_shared<Player>(name, health, level));
    }
}

int main() {
    try {
        // Создание менеджера и добавление персонажей
        GameManager<std::shared_ptr<Entity>> manager;
        manager.addEntity(std::make_shared<Player>("Hero", 100, 1));
        manager.addEntity(std::make_shared<Player>("Villain", 50, 2));

        // Сохранение данных в файл
        saveToFile(manager, "game_save.txt");

        // Создание нового менеджера для загрузки данных
        GameManager<std::shared_ptr<Entity>> loadedManager;

        // Загрузка данных из файла
        loadFromFile(loadedManager, "game_save.txt");

        // Отображение загруженных персонажей
        std::cout << "Loaded Entities:\n";
        loadedManager.displayAll();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

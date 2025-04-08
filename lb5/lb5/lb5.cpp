#include <vector>
#include <iostream>
#include <memory>

// Базовый класс сущности
class Entity {
public:
    virtual void displayInfo() const = 0;
    virtual ~Entity() = default;
};

// Пример класса Player
class Player : public Entity {
    std::string name;
    int health;
    int score;
public:
    Player(const std::string& name, int health, int score)
        : name(name), health(health), score(score) {
    }
    void displayInfo() const override {
        std::cout << "Player: " << name << ", Health: " << health << ", Score: " << score << '\n';
    }
};

// Пример класса Enemy
class Enemy : public Entity {
    std::string name;
    int health;
    std::string type;
public:
    Enemy(const std::string& name, int health, const std::string& type)
        : name(name), health(health), type(type) {
    }
    void displayInfo() const override {
        std::cout << "Enemy: " << name << ", Health: " << health << ", Type: " << type << '\n';
    }
};

// Шаблонный класс очереди
template <typename T>
class Queue {
    std::vector<T> entities;
public:
    void addEntity(const T& entity) {
        entities.push_back(entity);
    }
    void popEntity() {
        if (!entities.empty()) {
            entities.erase(entities.begin());
        }
    }
    void displayAll() const {
        for (const auto& entity : entities) {
            entity->displayInfo();
        }
    }
};

int main() {
    Queue<std::shared_ptr<Entity>> manager;
    manager.addEntity(std::make_shared<Player>("Hero", 100, 0));
    manager.addEntity(std::make_shared<Enemy>("Goblin", 50, "Goblin"));
    manager.displayAll();

    return 0;
}

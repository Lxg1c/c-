#include <iostream>
#include <vector>
#include <stdexcept>

// Базовый класс сущности
class Entity {
public:
    virtual int getHealth() const = 0;
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

    int getHealth() const override {
        return health;
    }
};

// Шаблонный класс GameManager
template <typename T>
class GameManager {
    std::vector<T> entities;
public:
    void addEntity(T entity) {
        if (entity->getHealth() <= 0) {
            throw std::invalid_argument("Entity has invalid health");
        }
        entities.push_back(entity);
    }
};

int main() {
    try {
        GameManager<Entity*> manager;
        manager.addEntity(new Player("Hero", -100, 0)); // Вызовет исключение
    }
    catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

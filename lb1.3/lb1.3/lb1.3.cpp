#include <iostream>
#include <string>
#include <cstdlib> // для rand() и srand()
#include <ctime>   // для time()

class Entity {
protected:
    std::string name;
    int health;
    int attack;
    int defense;

public:
    Entity(const std::string& n, int h, int a, int d)
        : name(n), health(h), attack(a), defense(d) {
    }

    // Виртуальный метод для атаки
    virtual void attackEnemy(Entity& target) {
        int damage = attack - target.defense;
        if (damage > 0) {
            target.health -= damage;
            std::cout << name << " attacks " << target.name << " for " << damage << " damage!\n";
        }
        else {
            std::cout << name << " attacks " << target.name << ", but it has no effect!\n";
        }
    }

    // Виртуальный метод для вывода информации
    virtual void displayInfo() const {
        std::cout << "Name: " << name << ", HP: " << health
            << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }

    // Геттеры
    int getDefence() const { return defense; }
    std::string getName() const { return name; }

    // Сеттер для получения урона
    void takeDamage(int damage) { health -= damage; }

    // Виртуальный метод для лечения
    virtual void heal(int amount) {
        health += amount;
        std::cout << name << " heals for " << amount << " HP. Current HP: " << health << std::endl;
    }

    // Виртуальный деструктор
    virtual ~Entity() {}
};

class Character : public Entity {
public:
    Character(const std::string& n, int h, int a, int d)
        : Entity(n, h, a, d) {
    }

    // Переопределение метода attack
    void attackEnemy(Entity& target) override {
        int damage = attack - target.getDefence();
        if (damage > 0) {
            // Шанс на критический удар (20%)
            if (rand() % 100 < 20) {
                damage *= 2;
                std::cout << "Critical hit! ";
            }
            target.takeDamage(damage);
            std::cout << name << " attacks " << target.getName() << " for " << damage << " damage!\n";
        }
        else {
            std::cout << name << " attacks " << target.getName() << ", but it has no effect!\n";
        }
    }

    // Переопределение метода displayInfo
    void displayInfo() const override {
        std::cout << "Character: " << name << ", HP: " << health
            << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }

    // Переопределение метода heal
    void heal(int amount) override {
        health += amount;
        std::cout << name << " uses healing spell and restores " << amount << " HP. Current HP: " << health << std::endl;
    }
};

class Monster : public Entity {
public:
    Monster(const std::string& n, int h, int a, int d)
        : Entity(n, h, a, d) {
    }

    // Переопределение метода attack
    void attackEnemy(Entity& target) override {
        int damage = attack - target.getDefence();
        if (damage > 0) {
            // Шанс на ядовитую атаку (30%)
            if (rand() % 100 < 30) {
                damage += 5; // Дополнительный урон от яда
                std::cout << "Poisonous attack! ";
            }
            target.takeDamage(damage);
            std::cout << name << " attacks " << target.getName() << " for " << damage << " damage!\n";
        }
        else {
            std::cout << name << " attacks " << target.getName() << ", but it has no effect!\n";
        }
    }

    // Переопределение метода displayInfo
    void displayInfo() const override {
        std::cout << "Monster: " << name << ", HP: " << health
            << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }
};

class Boss : public Monster {
public:
    Boss(const std::string& n, int h, int a, int d) : Monster(n, h, a, d) {}

    void FireStrike(Entity& target) {
        int damage = attack - target.getDefence();
        if (damage > 0) {
            // Шанс на ядовитую атаку (30%)
            if (rand() % 100 < 30) {
                damage += 5; // Дополнительный урон от яда
                std::cout << "Poisonous attack! ";
            }
            target.takeDamage(damage);
            std::cout << name << " attacks by Fire Strike " << target.getName() << " for " << damage << " damage!\n";
        }
        else {
            std::cout << name << " attacks " << target.getName() << ", but enemy is Dodged!\n";
        }
    }
};

int main() {
    srand(static_cast<unsigned>(time(0))); // Инициализация генератора случайных чисел

    // Создание объектов
    Character hero("Hero", 100, 20, 10);
    Monster goblin("Goblin", 50, 15, 5);
    Monster dragon("Dragon", 150, 25, 20);
    Boss boss("Bob", 200, 50, 40);

    // Массив указателей на базовый класс
    Entity* entities[] = { &hero, &goblin, &dragon, &boss };

    // Полиморфное поведение
    for (auto& entity : entities) {
        entity->displayInfo(); // Вывод информации о сущности
    }

    // Бой между персонажем и монстрами
    hero.attackEnemy(goblin);
    goblin.attackEnemy(hero);
    dragon.attackEnemy(hero);
    boss.attackEnemy(hero);
    boss.FireStrike(hero);

    // Персонаж лечится
    hero.heal(30);

    return 0;
}
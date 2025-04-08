#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <memory>
#include <ctime>
#include <iomanip>
#include <chrono>

// Шаблонный класс Logger для записи логов в файл
template<typename T>
class Logger {
public:
    Logger(const std::string& filename) : logFile(filename, std::ios::app) {
        if (!logFile.is_open()) {
            throw std::runtime_error("Failed to open log file");
        }
    }

    void log(const T& message) {
        auto now = std::chrono::system_clock::now();
        auto now_time = std::chrono::system_clock::to_time_t(now);

        // Безопасная версия с localtime_s
        std::tm tm;
        localtime_s(&tm, &now_time);

        logFile << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " - " << message << std::endl;
    }

    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }

private:
    std::ofstream logFile;
};

// Базовый класс для всех существ
class Entity {
protected:
    std::string name;
    int maxHealth;
    int health;
    int attack;
    int defense;

public:
    Entity(const std::string& n, int h, int a, int d)
        : name(n), maxHealth(h), health(h), attack(a), defense(d) {
    }

    virtual void attackEnemy(Entity& enemy, Logger<std::string>& logger) {
        try {
            int damage = attack - enemy.getDefense();
            if (damage > 0) {
                enemy.takeDamage(damage);
                std::string msg = name + " attacks " + enemy.getName() + " for " + std::to_string(damage) + " damage!";
                logger.log(msg);
                std::cout << msg << std::endl;
            }
            else {
                std::string msg = name + " attacks " + enemy.getName() + ", but it has no effect!";
                logger.log(msg);
                std::cout << msg << std::endl;
            }
        }
        catch (const std::runtime_error& e) {
            logger.log(e.what());
            std::cout << e.what() << std::endl;
        }
    }

    virtual void takeDamage(int damage) {
        if (health - damage <= 0) {
            health = 0;
            throw std::runtime_error(name + " has been defeated!");
        }
        health -= damage;
    }

    virtual void heal(int amount) {
        if (health + amount > maxHealth) {
            health = maxHealth;
        }
        else {
            health += amount;
        }
    }

    std::string getName() const { return name; }
    int getHealth() const { return health; }
    int getAttack() const { return attack; }
    int getDefense() const { return defense; }
    int getMaxHealth() const { return maxHealth; }

    virtual void displayInfo() const {
        std::cout << "Name: " << name << ", HP: " << health
            << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }

    virtual ~Entity() = default;
};

// Класс предмета
class Item {
public:
    virtual ~Item() = default;
    virtual void use(Entity& target) = 0;
    virtual std::string getName() const = 0;
    virtual std::string getType() const = 0;
};

// Класс оружия
class Weapon : public Item {
public:
    Weapon(const std::string& name, int damage) : name(name), damage(damage) {}

    void use(Entity& target) override {
        target.takeDamage(damage);
    }

    std::string getName() const override { return name; }
    std::string getType() const override { return "Weapon"; }
    int getDamage() const { return damage; }

private:
    std::string name;
    int damage;
};

// Класс зелья
class Potion : public Item {
public:
    Potion(const std::string& name, int healAmount) : name(name), healAmount(healAmount) {}

    void use(Entity& target) override {
        target.heal(healAmount);
    }

    std::string getName() const override { return name; }
    std::string getType() const override { return "Potion"; }

private:
    std::string name;
    int healAmount;
};

// Класс инвентаря
class Inventory {
private:
    std::vector<std::unique_ptr<Item>> items;

public:
    void addItem(std::unique_ptr<Item> item) {
        items.push_back(std::move(item));
    }

    void dropItem(const std::string& itemName) {
        auto it = std::find_if(items.begin(), items.end(),
            [&itemName](const auto& item) { return item->getName() == itemName; });

        if (it != items.end()) {
            items.erase(it);
            std::cout << "Dropped: " << itemName << std::endl;
        }
        else {
            throw std::runtime_error("Item not found: " + itemName);
        }
    }

    void useItem(const std::string& itemName, Entity& target) {
        auto it = std::find_if(items.begin(), items.end(),
            [&itemName](const auto& item) { return item->getName() == itemName; });

        if (it != items.end()) {
            (*it)->use(target);
            items.erase(it);
        }
        else {
            throw std::runtime_error("Item not found: " + itemName);
        }
    }

    void showItems() const {
        std::cout << "Inventory:\n";
        for (const auto& item : items) {
            std::cout << "- " << item->getName() << " (" << item->getType() << ")\n";
        }
    }

    bool hasItem(const std::string& itemName) const {
        return std::any_of(items.begin(), items.end(),
            [&itemName](const auto& item) { return item->getName() == itemName; });
    }
};

// Класс персонажа
class Character : public Entity {
private:
    int level;
    int experience;
    Inventory inventory;

public:
    Character(const std::string& n, int h, int a, int d)
        : Entity(n, h, a, d), level(1), experience(0) {
    }

    void heal(int amount, Logger<std::string>& logger) {
        int oldHealth = health;
        Entity::heal(amount);
        int healed = health - oldHealth;
        logger.log(name + " heals for " + std::to_string(healed) + " HP!");
    }

    void gainExperience(int exp, Logger<std::string>& logger) {
        experience += exp;
        if (experience >= 100) {
            level++;
            experience -= 100;
            attack += 2;
            defense += 1;
            maxHealth += 10;
            health = maxHealth;
            logger.log(name + " leveled up to level " + std::to_string(level) + "!");
        }
    }

    void addItem(std::unique_ptr<Item> item) {
        inventory.addItem(std::move(item));
    }

    void dropItem(const std::string& item) {
        inventory.dropItem(item);
    }

    void useItem(const std::string& item) {
        inventory.useItem(item, *this);
    }

    void showInventory() const {
        inventory.showItems();
    }

    int getLevel() const { return level; }
    int getExperience() const { return experience; }
};

// Класс монстра
class Monster : public Entity {
public:
    Monster(const std::string& n, int h, int a, int d) : Entity(n, h, a, d) {}
};

// Производные классы монстров
class Goblin : public Monster {
public:
    Goblin() : Monster("Goblin", 30, 10, 2) {}
};

class Dragon : public Monster {
public:
    Dragon() : Monster("Dragon", 150, 40, 10) {}
};

// Класс скелета с сопротивлением
class Skeleton : public Monster {
public:
    Skeleton(const std::string& name, int health, int attack, int defense, bool isResistant = true)
        : Monster(name, health, attack, defense), isResistant(isResistant) {
    }

    void takeDamage(int damage) override {
        if (isResistant) {
            damage /= 2;
            std::cout << name << " resists some damage!\n";
        }
        Monster::takeDamage(damage);
    }

private:
    bool isResistant;
};

// Класс игры с улучшенной системой сохранения
class Game {
public:
    void saveGame(const Character& character, const std::string& filename = "savegame.txt") {
        std::ofstream file(filename);
        if (!file) {
            throw std::runtime_error("Failed to open save file");
        }

        file << character.getName() << "\n"
            << character.getHealth() << "\n"
            << character.getMaxHealth() << "\n"
            << character.getAttack() << "\n"
            << character.getDefense() << "\n"
            << character.getLevel() << "\n"
            << character.getExperience() << "\n";

        // Здесь можно добавить сохранение инвентаря
        file.close();
    }

    Character loadGame(const std::string& filename = "savegame.txt") {
        std::ifstream file(filename);
        if (!file) {
            throw std::runtime_error("Failed to open save file");
        }

        std::string name;
        int health, maxHealth, attack, defense, level, experience;

        std::getline(file, name);
        file >> health >> maxHealth >> attack >> defense >> level >> experience;
        file.ignore();

        Character character(name, maxHealth, attack, defense);
        character.heal(health - character.getHealth(), *logger); // Восстанавливаем здоровье
        for (int i = 1; i < level; ++i) {
            character.gainExperience(100, *logger); // Имитируем получение уровней
        }
        character.gainExperience(experience, *logger); // Добавляем оставшийся опыт

        // Здесь можно добавить загрузку инвентаря
        return character;
    }

    void setLogger(std::shared_ptr<Logger<std::string>> newLogger) {
        logger = newLogger;
    }

private:
    std::shared_ptr<Logger<std::string>> logger;
};

int main() {
    try {
        // Инициализация логгера
        auto logger = std::make_shared<Logger<std::string>>("game_log.txt");
        logger->log("=== Game session started ===");

        // Инициализация игровых объектов
        Game game;
        game.setLogger(logger);

        // Создание персонажа
        Character hero("Sir Lancelot", 120, 25, 15);
        logger->log("Player created: " + hero.getName());

        // Создание монстров
        Skeleton skeleton1("Bony", 60, 12, 8, true);
        Skeleton skeleton2("Rusty", 55, 10, 7);
        Dragon dragon;
        logger->log("Enemies spawned: " + skeleton1.getName() + ", " + skeleton2.getName() + ", " + dragon.getName());

        // Добавление предметов в инвентарь
        hero.addItem(std::make_unique<Weapon>("Excalibur", 35));
        hero.addItem(std::make_unique<Weapon>("Steel Dagger", 15));
        hero.addItem(std::make_unique<Potion>("Health Elixir", 50));
        hero.addItem(std::make_unique<Potion>("Mana Potion", 30));
        logger->log("Items added to inventory");

        // Демонстрация инвентаря
        std::cout << "\n=== Initial Hero State ===\n";
        hero.displayInfo();
        std::cout << "Inventory:\n";
        hero.showInventory();

        // Бой со скелетом 1
        std::cout << "\n=== Battle with " << skeleton1.getName() << " ===\n";
        hero.attackEnemy(skeleton1, *logger);
        skeleton1.attackEnemy(hero, *logger);
        hero.attackEnemy(skeleton1, *logger);

        // Использование зелья
        std::cout << "\n=== Using Health Potion ===\n";
        hero.useItem("Health Elixir");
        hero.showInventory();

        // Бой со скелетом 2
        std::cout << "\n=== Battle with " << skeleton2.getName() << " ===\n";
        hero.attackEnemy(skeleton2, *logger);
        skeleton2.attackEnemy(hero, *logger);
        hero.attackEnemy(skeleton2, *logger);

        // Получение опыта
        std::cout << "\n=== Gaining Experience ===\n";
        hero.gainExperience(75, *logger);
        hero.gainExperience(50, *logger); // Должен повысить уровень
        hero.displayInfo();

        // Бой с драконом
        std::cout << "\n=== Epic Battle with " << dragon.getName() << " ===\n";
        for (int i = 0; i < 3; ++i) {
            hero.attackEnemy(dragon, *logger);
            dragon.attackEnemy(hero, *logger);
        }

        // Сохранение игры
        std::cout << "\n=== Saving Game ===\n";
        game.saveGame(hero, "hero_save.txt");
        logger->log("Game saved");

        // Загрузка игры
        std::cout << "\n=== Loading Game ===\n";
        Character loadedHero = game.loadGame("hero_save.txt");
        loadedHero.displayInfo();
        loadedHero.showInventory();

        // Демонстрация обработки исключений
        std::cout << "\n=== Exception Handling Demo ===\n";
        try {
            loadedHero.useItem("Nonexistent Item");
        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
            logger->log(std::string("Exception: ") + e.what());
        }

        // Создание нового персонажа и демонстрация инвентаря
        std::cout << "\n=== New Character Demo ===\n";
        Character mage("Gandalf", 80, 15, 10);
        mage.addItem(std::make_unique<Potion>("Mega Potion", 100));
        mage.addItem(std::make_unique<Weapon>("Magic Staff", 20));
        mage.displayInfo();
        mage.showInventory();

        logger->log("=== Game session ended ===\n");
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
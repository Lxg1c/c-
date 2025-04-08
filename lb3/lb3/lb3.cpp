#include <iostream>


class Weapon {
    std::string name;
    int damage;
    int weight;

public:
    Weapon(const std::string& name, int damage, int weight) : name(name), damage(damage), weight(weight) {}

    // Геттер
    int getDamage() const { return damage; }

    void getInfo() {
        std::cout << "Name: " << name << "\nDamage: " << damage << "\nWeight: " << weight << std::endl;
    }

    // Перегрузка оператора +
    Weapon operator+(const Weapon& other) const {
        return Weapon(name + " & " + other.name, damage + other.damage, weight + other.weight);
    }

    // Перегрузка оператора >
    bool operator>(const Weapon& other) const {
        return damage > other.damage;
    }

    ~Weapon() {
        std::cout << "Weapon " << name << " destroyed.\n";
    }
};

int main() {
    Weapon sword("Sword", 10, 5);
    sword.getInfo();
    Weapon axe("Axe", 15, 7);
    axe.getInfo();
    Weapon mergedWeapon = sword + axe;
    mergedWeapon.getInfo();

    std::cout << "\nComparing weapons: \n";
    if (axe > sword) {
        std::cout << "Axe is stronger than Sword!\n";
    }
    else {
        std::cout << "Sword is stronger than Axe!\n";
    }

    return 0;
}

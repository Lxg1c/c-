#include <iostream>
#include <string>

class Character {
private:
    std::string name;  // ��������� ����: ��� ���������
    int health;        // ��������� ����: ������� ��������
    int attack;        // ��������� ����: ������� �����
    int defense;       // ��������� ����: ������� ������

public:
    // ����������� ��� ������������� ������
    Character(const std::string& n, int h, int a, int d)
        : name(n), health(h), attack(a), defense(d) {
    }

    // ����� ��� ��������� ������ ��������
    int getHealth() const {
        return health;
    }

    // ����� ��� ������ ���������� � ���������
    void displayInfo() const {
        std::cout << "Name: " << name << ", HP: " << health
            << ", Attack: " << attack << ", Defense: " << defense << std::endl;
    }

    // ����� ��� ����� ������� ���������
    void attackEnemy(Character& enemy) {
        int damage = attack - enemy.defense;
        if (damage > 0) {
            enemy.health -= damage;
            std::cout << name << " attacks " << enemy.name << " for " << damage << " damage!" << std::endl;
        }
        else {
            std::cout << name << " attacks " << enemy.name << ", but it has no effect!" << std::endl;
        }
    }

    void heal(int heal_value) {
        if (heal_value + health > 100) {
            std::cout << "Heal is full" << std::endl;
            health = 100;
        }
        else {
            health += heal_value;
        }
    }

    void takeDamage(int damage) {
        if (health - damage < 0) {
            health = 0;
            std::cout << "You died" << std::endl;
        }
        else {
            health -= damage;
        }
    }
};

int main() {
    // ������� ������� ����������
    Character hero("Hero", 100, 20, 10);
    Character monster("Goblin", 50, 15, 5);

    // ������� ���������� � ����������
    hero.displayInfo();
    monster.displayInfo();

    // ����� ������� �������
    hero.attackEnemy(monster);
    monster.displayInfo();

    hero.takeDamage(20);
    std::cout << "Your health " << hero.getHealth() << std::endl;

    hero.heal(30);
    std::cout << "Your health " << hero.getHealth() << std::endl;

    hero.takeDamage(200);
    return 0;
}

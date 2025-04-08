#include <iostream>
#include <memory>
#include <string>

class Inventory {
private:
    std::unique_ptr<std::string[]> items;
    size_t capacity;
    size_t size;

public:
    Inventory(size_t cap = 10) : capacity(cap), size(0), items(std::make_unique<std::string[]>(cap)) {}

    void addItem(const std::string& item) {
        if (size < capacity) {
            items[size++] = item;
        }
        else {
            std::cout << "Inventory is full!" << std::endl;
        }
    }

    void displayInventory() const {
        std::cout << "Inventory items:" << std::endl;
        for (size_t i = 0; i < size; ++i) {
            std::cout << "- " << items[i] << std::endl;
        }
    }
};

int main() {
    Inventory inventory(5);
    inventory.addItem("Sword");
    inventory.addItem("Health Potion");
    inventory.addItem("Shield");

    inventory.displayInventory();

    return 0;
}

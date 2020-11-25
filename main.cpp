#include <iostream>
#include <list>

using namespace std;

class Reading {
private:
    long meterNumber;
    unsigned char time;
};

class Customer {
private:
    long meterNumber;
    list<Reading> readings;
    long totalKwhUsed;
    float balance;

public:
    void addReading() {}
    void setBalance() {}
    [[nodiscard]] float getBalance() const {
        return balance;
    }
};

class TOUCustomer : public Customer {

};

class TIERCustomer : public Customer {

};



int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}

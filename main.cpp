#include <iostream>
#include <list>

using namespace std;

class Reading {
private:
    int meterNumber;
    unsigned char time;

public:
    Reading(int meterNumber, unsigned char time) : meterNumber(meterNumber), time(time) {}
    ~Reading() = default;
};

class Customer {
protected:
    int meterNumber;
    list<Reading> readings;
    int totalKwhUsed;
    double balance;

public:
    Customer() {
        meterNumber = 0;
        totalKwhUsed = 0;
        balance = 0;
    }
    Customer(int newMeterNumber, int newTotalKwhUsed, double newBalance) {
        meterNumber = newMeterNumber;
        totalKwhUsed = newTotalKwhUsed;
        balance = newBalance;
    }
    virtual ~Customer() = default;

    void addReading(Reading reading) {
        readings.push_back(reading);
    }
    void setBalance(double newBalance) {
        balance = newBalance;
    }
    [[nodiscard]] double getBalance() const {
        return balance;
    }
    virtual void computeBalance() = 0;
};

class TOUCustomer : public Customer {
public:
    TOUCustomer() = default;
    TOUCustomer(int newMeterNumber, int newTotalKwhUsed, double newBalance) : Customer(
            newMeterNumber, newTotalKwhUsed, newBalance) {}

    void computeBalance() final {}
};

class TIERCustomer : public Customer {
public:
    TIERCustomer() = default;
    TIERCustomer(int newMeterNumber, int newTotalKwhUsed, double newBalance) : Customer(
            newMeterNumber, newTotalKwhUsed, newBalance) {}

    void computeBalance() final {}
};

int main() {
    Customer* cust = new TOUCustomer;
    cust->computeBalance();
    return 0;
}

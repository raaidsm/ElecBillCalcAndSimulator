#include <iostream>
#include <list>
#include <vector>
#include <random>

using namespace std;

class Reading {
private:
    double meterReading;
    unsigned char time;

public:
    Reading(double meterReading, unsigned char time) : meterReading(meterReading), time(time) {}
    ~Reading() = default;
};

class Customer {
protected:
    int meterNumber;
    list<Reading> readings;
    double totalKwhUsed;
    double balance;

public:
    Customer() {
        meterNumber = 0;
        totalKwhUsed = 0;
        balance = 0;
    }
    Customer(int newMeterNumber, double newTotalKwhUsed) : Customer() {
        meterNumber = newMeterNumber;
        totalKwhUsed = newTotalKwhUsed;
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

class TOUCustomer final : public Customer {
public:
    TOUCustomer() = default;
    TOUCustomer(int newMeterNumber, double newTotalKwhUsed) : Customer(
            newMeterNumber, newTotalKwhUsed) {}
    ~TOUCustomer() final = default;

    void computeBalance() final {}
};

class TIERCustomer final : public Customer {
public:
    TIERCustomer() = default;
    TIERCustomer(int newMeterNumber, double newTotalKwhUsed) : Customer(
            newMeterNumber, newTotalKwhUsed) {}
    ~TIERCustomer() final = default;

    void computeBalance() final {}
};

class Simulation {
    vector<TOUCustomer> touCustomerVector;
    vector<TIERCustomer> tierCustomerVector;

    static void generateCustomers() {
        //TODO: 1) Randomly generate 1000 TOUCustomers and 1000 TIERCustomers.
        //Generate 1000 TOUCustomers
        for (int i = 0; i < 1000; i++) {
            //
        }
        //Generate 1000 TIERCustomers
        for (int i = 0; i < 1000; i++) {
            //
        }
        //TODO: 2) Randomly generate meter readings for each hour.
        //TODO: 2.1) Meter readings should be between 0.05 and 2.00 kWh.
        //TODO: 3) Generate meter readings for 30 days.
    }
public:
    Simulation() {
        generateCustomers();
    }

    void printResult() {}
};

int main() {
    auto* sim = new Simulation();
    sim->printResult();
    delete sim;
    return 0;
}

#include <iostream>
#include <list>
#include <vector>

using namespace std;

class Reading {
private:
    double meterNumber;
    unsigned char time;

public:
    Reading(double meterNumber, unsigned char time) : meterNumber(meterNumber), time(time) {}
    ~Reading() = default;
};

class Customer {
protected:
    double meterNumber;
    list<Reading> readings;
    double totalKwhUsed;
    double balance;

public:
    Customer() {
        meterNumber = 0;
        totalKwhUsed = 0;
        balance = 0;
    }
    Customer(double newMeterNumber, double newTotalKwhUsed) : Customer() {
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

class TOUCustomer : public Customer {
public:
    TOUCustomer() = default;
    TOUCustomer(double newMeterNumber, double newTotalKwhUsed) : Customer(
            newMeterNumber, newTotalKwhUsed) {}

    void computeBalance() final {}
};

class TIERCustomer : public Customer {
public:
    TIERCustomer() = default;
    TIERCustomer(double newMeterNumber, double newTotalKwhUsed) : Customer(
            newMeterNumber, newTotalKwhUsed) {}

    void computeBalance() final {}
};

class Simulation {
    //TODO: Randomly generate 1000 TOUCustomers and 1000 TIERCustomers.
    //TODO: Randomly generate meter readings for each hour.
        //TODO: Meter readings should be between 0.05 and 2.00 kWh.
    //TODO: Generate meter readings for 30 days.
    vector<TOUCustomer> touCustomerVector;
    vector<TIERCustomer> tierCustomerVector;
public:
    void printResult() {}
};

int main() {
    auto* sim = new Simulation();
    sim->printResult();
    delete sim;
    return 0;
}

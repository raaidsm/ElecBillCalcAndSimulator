#include <iostream>
#include <list>
#include <vector>
#include <random>
#include <chrono>

#define NUM_CUSTOMERS_SIMULATED 1000
#define NUM_DAYS_SIMULATED 30
#define READINGS_PER_DAY 24
#define STARTING_METER_NUMBER 111111
#define MIN_READING_INT 5
#define MAX_READING_INT 200

using namespace std;

class Reading {
private:
    double meterReading;
    unsigned char day;
    unsigned char hour;

public:
    Reading(double newMeterReading, unsigned char newDay, unsigned char newHour) {
        meterReading = newMeterReading;
        day = newDay;
        hour = newHour;
    }
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
    explicit Customer(int newMeterNumber) : Customer() {
        meterNumber = newMeterNumber;
    }
    virtual ~Customer() = default;

    void addReading(Reading reading) {
        readings.push_back(reading);
    }
    void setTotalKwhUsed(double newTotalKwhUsed) {
        totalKwhUsed = newTotalKwhUsed;
    }
    [[nodiscard]] double getTotalKwhUsed() const {
        return totalKwhUsed;
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
    //These values are in cents multiplied by 10 to be represented as integral types
    unsigned char offPeakPrice;
    unsigned char midPeakPrice;
    unsigned char onPeakPrice;
    void init() {
        offPeakPrice = 105;
        midPeakPrice = 150;
        onPeakPrice = 217;
    }

public:
    TOUCustomer() { init(); }
    explicit TOUCustomer(int newMeterNumber) : Customer(newMeterNumber) { init(); }
    ~TOUCustomer() final = default;

    void computeBalance() final {}
};

class TIERCustomer final : public Customer {
    //These values are in cents multiplied by 10 to be represented as integral types
    unsigned char tier1Price;
    unsigned char tier2Price;
    void init() {
        tier1Price = 126;
        tier2Price = 146;
    }

public:
    TIERCustomer() { init(); }
    explicit TIERCustomer(int newMeterNumber) : Customer(newMeterNumber) { init(); }
    ~TIERCustomer() final = default;

    void computeBalance() final {}
};

class Simulation {
    vector<TOUCustomer> touCustomerVector;
    vector<TIERCustomer> tierCustomerVector;

    void generateCustomers() {
        default_random_engine engine = generateRandomEngine();

        //Generate 1000 TOUCustomers
        for (int i = 0; i < NUM_CUSTOMERS_SIMULATED; i++) {
            double totalKwhUsed;

            auto* customer = new TOUCustomer(STARTING_METER_NUMBER + i);
            generateReadings(engine, customer, totalKwhUsed);

            customer->setTotalKwhUsed(totalKwhUsed);
            customer->computeBalance();

            touCustomerVector.push_back(*customer);
            delete customer;
        }
        //Generate 1000 TIERCustomers
        for (int i = 0; i < NUM_CUSTOMERS_SIMULATED; i++) {
            double totalKwhUsed;

            auto* customer = new TIERCustomer(STARTING_METER_NUMBER + NUM_CUSTOMERS_SIMULATED + i);
            generateReadings(engine, customer, totalKwhUsed);

            customer->setTotalKwhUsed(totalKwhUsed);
            customer->computeBalance();

            tierCustomerVector.push_back(*customer);
            delete customer;
        }
    }

public:
    Simulation() {
        generateCustomers();
    }

    static default_random_engine generateRandomEngine() {
        unsigned int seed = chrono::steady_clock::now().time_since_epoch().count();
        default_random_engine engine(seed);
        return engine;
    }
    static void generateReadings(default_random_engine engine, Customer* customer, double& totalKwhUsed) {
        uniform_int_distribution<int> distribution(MIN_READING_INT, MAX_READING_INT);
        //For each day
        for (int i = 1; i <= NUM_DAYS_SIMULATED; i++) {
            //For each hour
            for (int j = 1; j < READINGS_PER_DAY; j++) {
                double meterReading = distribution(engine);
                meterReading /= 100;
                customer->addReading(Reading(meterReading, i, j));

                totalKwhUsed += meterReading;
            }
        }
    }
    void printResult() {}
};

int main() {
    auto* sim = new Simulation();
    sim->printResult();
    delete sim;
    return 0;
}

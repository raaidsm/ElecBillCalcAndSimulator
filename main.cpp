#include <iostream>
#include <list>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>

#define NUM_CUSTOMERS_SIMULATED 1000
#define NUM_DAYS_SIMULATED 30
#define READINGS_PER_DAY 24
#define STARTING_METER_NUMBER 111111
#define MIN_READING_INT 5
#define MAX_READING_INT 200
#define CENT_MUL_TEN_TO_DOLLAR_CONVERSION_RATE 1000
#define TIER2_THRESHOLD 1000

using namespace std;

class Reading {
    double meterReading;
    //For this assignment, the chosen 30-day period is considered to be November 1st to November 30th (inclusive)
    unsigned char day;
    unsigned char hour;

public:
    Reading(double newMeterReading, unsigned char newDay, unsigned char newHour) {
        meterReading = newMeterReading;
        day = newDay;
        hour = newHour;
    }
    ~Reading() = default;

    [[nodiscard]] double getMeterReading() const {
        return meterReading;
    }
    [[nodiscard]] unsigned char getDay() const {
        return day;
    }
    [[nodiscard]] unsigned char getHour() const {
        return hour;
    }
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

    void setTotalKwhUsed(double newTotalKwhUsed) {
        totalKwhUsed = newTotalKwhUsed;
    }
    [[nodiscard]] double getTotalKwhUsed() const {
        return totalKwhUsed;
    }
    void setBalance(double newBalance) {
        //Inputted value is in cents*10, so divide by 1000 to get the dollar amount
        balance = newBalance / CENT_MUL_TEN_TO_DOLLAR_CONVERSION_RATE;
    }
    [[nodiscard]] double getBalance() const {
        //Outputted value is used in cents*10 format, so return balance*1000
        return balance * CENT_MUL_TEN_TO_DOLLAR_CONVERSION_RATE;
    }

    void addReading(Reading reading) {
        readings.push_back(reading);
    }
    virtual void computeBalance() = 0;
};

class TOUCustomer final : public Customer {
    //These values are in cents multiplied by 10 to be represented as integral types
    unsigned char offPeakPrice;
    unsigned char midPeakPrice;
    unsigned char onPeakPrice;

public:
    TOUCustomer() {
        offPeakPrice = 105;
        midPeakPrice = 150;
        onPeakPrice = 217;}
    explicit TOUCustomer(int newMeterNumber) : Customer(newMeterNumber) {
        offPeakPrice = 105;
        midPeakPrice = 150;
        onPeakPrice = 217;}
    ~TOUCustomer() final = default;

    void computeBalance() final {
        for (auto reading = readings.begin(); reading != readings.end(); reading++) {
            unsigned char dayModSeven = reading->getDay() % 7;
            unsigned char hour = reading->getHour();

            //If off-peak
            if (dayModSeven == 6  ||  dayModSeven == 0  ||  (1 <= hour && hour <= 7)  ||  (20 <= hour && hour <= 24)) {
                setBalance(getBalance() + reading->getMeterReading() * offPeakPrice);
            }
            //If mid-peak
            else if (12 <= hour && hour < 17) {
                setBalance(getBalance() + reading->getMeterReading() * midPeakPrice);
            }
            //If on-peak
            else {
                setBalance(getBalance() + reading->getMeterReading() * onPeakPrice);
            }
        }
    }
};

class TIERCustomer final : public Customer {
    //These values are in cents multiplied by 10 to be represented as integral types
    unsigned char tier1Price;
    unsigned char tier2Price;

public:
    TIERCustomer() {
        tier1Price = 126;
        tier2Price = 146;}
    explicit TIERCustomer(int newMeterNumber) : Customer(newMeterNumber) {
        tier1Price = 126;
        tier2Price = 146;}
    ~TIERCustomer() final = default;

    void computeBalance() final {
        double kwhSpent = 0;

        for (auto reading = readings.begin(); reading != readings.end(); reading++) {
            kwhSpent += reading->getMeterReading();
        }

        if (TIER2_THRESHOLD < kwhSpent) {
            double kwhSpentInTier2 = kwhSpent - TIER2_THRESHOLD;
            setBalance(getBalance() + TIER2_THRESHOLD * tier1Price + kwhSpentInTier2 * tier2Price);
        }
        else setBalance(getBalance() + kwhSpent * tier1Price);
    }
};

class Simulation {
    vector<TOUCustomer> touCustomerVector;
    vector<TIERCustomer> tierCustomerVector;
    double totalKwhUsedTOU;
    double totalBalanceTOU;
    double maxBalanceTOU;
    double minBalanceTOU;
    double totalKwhUsedTIER;
    double totalBalanceTIER;
    double maxBalanceTIER;
    double minBalanceTIER;

    void generateCustomers() {
        default_random_engine engine = generateRandomEngine();

        //Generate NUM_CUSTOMERS_SIMULATED TOUCustomers
        for (int i = 0; i < NUM_CUSTOMERS_SIMULATED; i++) {
            double totalKwhUsed;

            auto* customer = new TOUCustomer(STARTING_METER_NUMBER + i);
            generateReadings(engine, customer, totalKwhUsed);

            customer->setTotalKwhUsed(totalKwhUsed);
            customer->computeBalance();

            touCustomerVector.push_back(*customer);
            delete customer;
        }
        //Generate NUM_CUSTOMERS_SIMULATED TIERCustomers
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
    void analyzeCustomers() {
        //TOUCustomers
        for (auto customer = touCustomerVector.begin(); customer != touCustomerVector.end(); customer++) {
            //TODO: DEBUGGING:
            cout << "This customer's total kWh use is: " << customer->getTotalKwhUsed() << endl;
            totalKwhUsedTOU += customer->getTotalKwhUsed();
            totalBalanceTOU += customer->getBalance() / CENT_MUL_TEN_TO_DOLLAR_CONVERSION_RATE;

            if (maxBalanceTOU == 0.0 ||
            maxBalanceTOU < customer->getBalance() / CENT_MUL_TEN_TO_DOLLAR_CONVERSION_RATE) {
                maxBalanceTOU = customer->getBalance() / CENT_MUL_TEN_TO_DOLLAR_CONVERSION_RATE;
            }
            if (minBalanceTOU == 0.0 ||
            customer->getBalance() < minBalanceTOU / CENT_MUL_TEN_TO_DOLLAR_CONVERSION_RATE) {
                minBalanceTOU = customer->getBalance() / CENT_MUL_TEN_TO_DOLLAR_CONVERSION_RATE;
            }
        }
        //TIERCustomers
        for (auto customer = tierCustomerVector.begin(); customer != tierCustomerVector.end(); customer++) {
            //TODO: DEBUGGING:
            cout << "This customer's total kWh use is: " << customer->getTotalKwhUsed() << endl;
            totalKwhUsedTIER += customer->getTotalKwhUsed();
            totalBalanceTIER += customer->getBalance() / CENT_MUL_TEN_TO_DOLLAR_CONVERSION_RATE;

            if (maxBalanceTIER == 0.0 ||
            maxBalanceTIER < customer->getBalance() / CENT_MUL_TEN_TO_DOLLAR_CONVERSION_RATE) {
                maxBalanceTIER = customer->getBalance() / CENT_MUL_TEN_TO_DOLLAR_CONVERSION_RATE;
            }
            if (minBalanceTIER == 0.0 ||
            customer->getBalance() / CENT_MUL_TEN_TO_DOLLAR_CONVERSION_RATE < minBalanceTIER) {
                minBalanceTIER = customer->getBalance() / CENT_MUL_TEN_TO_DOLLAR_CONVERSION_RATE;
            }
        }
    }

public:
    static default_random_engine generateRandomEngine() {
        unsigned int seed = chrono::steady_clock::now().time_since_epoch().count();
        default_random_engine engine(seed);
        return engine;
    }
    static void generateReadings(default_random_engine& engine, Customer* customer, double& totalKwhUsed) {
        uniform_int_distribution<int> distribution(MIN_READING_INT, MAX_READING_INT);
        //For each day
        for (int i = 1; i <= NUM_DAYS_SIMULATED; i++) {
            //For each hour
            for (int j = 1; j <= READINGS_PER_DAY; j++) {
                double meterReading = distribution(engine);
                meterReading /= 100;
                customer->addReading(Reading(meterReading, i, j));

                totalKwhUsed += meterReading;
            }
        }
    }

    Simulation() {
        totalKwhUsedTOU = 0.0;
        totalBalanceTOU = 0.0;
        maxBalanceTOU = 0.0;
        minBalanceTOU = 0.0;
        totalKwhUsedTIER = 0.0;
        totalBalanceTIER = 0.0;
        maxBalanceTIER = 0.0;
        minBalanceTIER = 0.0;
        generateCustomers();
        analyzeCustomers();
    }

    void printResult() const {
        cout << setprecision(2) << fixed;
        cout << "Simulation run:" << endl;
        cout << "   Number of customers in each group: " << setw(10) << NUM_CUSTOMERS_SIMULATED << endl;

        cout << "TOU Customers:" << endl;
        cout << "   Average electric consumption: " << setw(20) <<
        totalKwhUsedTOU / NUM_CUSTOMERS_SIMULATED << " kWh" << endl;
        cout << "   Average balance/per customer: " << setw(12) <<
        "$" << totalBalanceTOU / NUM_CUSTOMERS_SIMULATED << endl;
        cout << "   Largest balance: " << setw(25) << "$" << maxBalanceTOU << endl;
        cout << "   Smallest balance: " << setw(24) << "$" << minBalanceTOU << endl;

        cout << "TIER Customers:" << endl;
        cout << "   Average electric consumption: " << setw(20) <<
        totalKwhUsedTIER / NUM_CUSTOMERS_SIMULATED << " kWh" << endl;
        cout << "   Average balance/per customer: " << setw(12) <<
        "$" << totalBalanceTIER / NUM_CUSTOMERS_SIMULATED << endl;
        cout << "   Largest balance: " << setw(25) << "$" << maxBalanceTIER << endl;
        cout << "   Smallest balance: " << setw(24) << "$" << minBalanceTIER << endl;
    }
};

int main() {
    auto* sim = new Simulation();
    sim->printResult();
    delete sim;
    return 0;
}
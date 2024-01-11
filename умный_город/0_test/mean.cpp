#include <iostream>

float analogRead(int pin) {
    return 5.768;
}

void delay(int ms) {
}

struct Avg {
    int pin, samples, ms;
    float sum;
    float get() {
        for (int i = 0; i < samples; i++) {
            sum += analogRead(pin);
            delay(ms);
        }
        float average = sum / samples; sum = 0;
        return average;
    }
    Avg(int p, int s, int m) {
        pin = p; samples = s; ms = m;
    }
};

int main() {
    auto avg_10 = Avg(10, 5, 100);
    std::cout << avg_10.get();
    std::cout << avg_10.get();
    return 0;
}
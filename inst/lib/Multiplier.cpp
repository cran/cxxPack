// Multiplier.cpp

#include "Multiplier.hpp"

Multiplier::Multiplier() {
    currValue = 1;
}

double Multiplier::getValue() {
    return currValue;
}

void Multiplier::multiplyBy(double x) {
    currValue *= x;
}



// Adder.cpp

#include "Adder.hpp"

Adder::Adder() {
    currValue = 0;
}

double Adder::getValue() {
    return currValue;
}

void Adder::addTo(double x) {
    currValue += x;
}



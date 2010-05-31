#include <iostream>
#include <string>

class Foo {
    static int count;
public:
    std::string name;
    Foo(std::string n) : name(n) { 
        Foo::count++;
        std::cout << "Foo constructor called " << Foo::count << " times\n";
    }
};

int Foo::count = 0;

static Foo a("a"),b("b"); // two static constructions (before main)

extern "C" {

void showFoo() {
    Foo c("c"); // non-static constructor
    std::cout << a.name << ", " << b.name << ", " << c.name << std::endl;
}

}

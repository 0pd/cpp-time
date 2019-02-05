#include <iostream>

struct shared
{
    static void static_print();
};

void shared::static_print() { std::cout << "static print\n"; }

extern "C"
{
    void extern_print() { std::cout << "extern print\n"; }
}

void print() { std::cout << "print\n"; }

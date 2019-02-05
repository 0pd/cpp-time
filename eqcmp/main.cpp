#include <iostream>
#include <vector>
#include "equality_comparable.h"

void test(equality_comparable const& lhs, equality_comparable const& rhs, bool expected)
{
    auto actual = lhs == rhs;

    if (actual != expected) 
    {
        std::cout << "Test failed. Actual: " << actual << ", expected: " << expected << std::endl;
    }
}

int main()
{
    equality_comparable x(0);
    equality_comparable y(0);
    equality_comparable z(1);

    test(x, y, true);
    test(x, z, false);
    test(z, y, false);
    
    equality_comparable a("string");
    equality_comparable b("string");
    equality_comparable c("gnirts");

    test(a, b, true);
    test(a, c, false);
    test(c, b, false);

    test(x, a, false);
}
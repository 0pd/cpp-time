#include <exception>
#include <iostream>
#include "dynamic_library.h"

int main(int argc, char * argv[])
{
    try
    {
        dynamic_library library(argv[1]);
        auto f = library.load_function<void(*)()>(argv[2]);
        f();
    }
    catch (std::domain_error const& exception)
    {
        std::cout << exception.what() << std::endl;
    }
}

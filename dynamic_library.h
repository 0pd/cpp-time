#include <dlfcn.h>
#include <string>
#include <exception>

struct dynamic_library
{
    dynamic_library(std::string const&);
    ~dynamic_library();

    template<typename Signature>
    Signature load_function(std::string const&);

private:
    void * handle_;
};

dynamic_library::dynamic_library(std::string const& name)
{
    handle_ = dlopen(name.c_str(), RTLD_LAZY);
    if (handle_ == nullptr)
    {
        auto error = dlerror();
        throw std::domain_error(error);
    }
}

dynamic_library::~dynamic_library()
{
    dlclose(handle_);
}

template<typename Signature>
Signature dynamic_library::load_function(std::string const& name)
{
    auto function = dlsym(handle_, name.c_str());
    if (function == nullptr)
    {
        auto error = dlerror();
        throw std::domain_error(error);
    }

    return (Signature) function;
}

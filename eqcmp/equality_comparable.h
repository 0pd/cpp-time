#pragma once

#include <memory>
#include <typeinfo>

struct equality_comparable
{
    template<typename T>
    equality_comparable(T value);

    template<typename T>
    equality_comparable& operator=(T value);

    friend bool operator==(equality_comparable const& lhs, equality_comparable const& rhs);
    friend bool operator!=(equality_comparable const& lhs, equality_comparable const& rhs);

private:
    struct impl_base
    {
        using Pointer = std::unique_ptr<impl_base>;

        virtual ~impl_base();

        virtual bool equals(impl_base const& other) const = 0;
        virtual std::type_info const& type() const = 0;
    };

    template<typename T>
    struct impl : impl_base
    {
        impl(T value);

        virtual bool equals(impl_base const& other) const override;

        virtual std::type_info const& type() const override;

    private:
        T value_;
    };

    impl_base::Pointer impl_;
};

template<typename T>
equality_comparable::equality_comparable(T value)
    : impl_(new impl<T>(std::forward<T>(value)))
{
}

template<typename T>
equality_comparable& equality_comparable::operator=(T value)
{
    impl_ = std::make_unique(std::forward(value));

    return *this;
}

template<typename T>
equality_comparable::impl<T>::impl(T value)
    : value_(value)
{
}

template<typename T>
bool equality_comparable::impl<T>::equals(impl_base const& other) const
{
    if (other.type() != type()) return false;

    auto cast = dynamic_cast<impl<T> const&>(other);

    return cast.value_ == value_;
}

template<typename T>
std::type_info const& equality_comparable::impl<T>::type() const
{
    return typeid(T);
}
#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include "my_vector.h"

struct big_integer
{
	big_integer();
	big_integer(big_integer const& other);
	big_integer(int a);
	explicit big_integer(std::string const& str);

	big_integer& operator=(big_integer const& other) &;

	big_integer& operator+=(big_integer const& rhs) &;
	big_integer& operator-=(big_integer const& rhs) &;
	big_integer& operator*=(big_integer const& rhs) &;
	big_integer& operator/=(big_integer const& rhs) &;
	big_integer& operator%=(big_integer const& rhs) &;

	big_integer& operator&=(big_integer const& rhs) &;
	big_integer& operator|=(big_integer const& rhs) &;
	big_integer& operator^=(big_integer const& rhs) &;

	big_integer& operator<<=(int rhs) &;
	big_integer& operator>>=(int rhs) &;

	big_integer operator+() const;
	big_integer operator-() const;
	big_integer operator~() const;

	big_integer& operator++() &;
	big_integer operator++(int) &;

	big_integer& operator--() &;
	big_integer operator--(int) &;

	friend bool operator==(big_integer const& a, big_integer const& b);
	friend bool operator!=(big_integer const& a, big_integer const& b);
	friend bool operator<(big_integer const& a, big_integer const& b);
	friend bool operator>(big_integer const& a, big_integer const& b);
	friend bool operator<=(big_integer const& a, big_integer const& b);
	friend bool operator>=(big_integer const& a, big_integer const& b);

	friend std::string to_string(big_integer const& a);

private:
	bool signum() const;
	std::uint32_t at(size_t index) const;
	size_t digits_count() const;
	void set_bit(size_t which, bool value);
	void remove_redundancy();
	void to_big();
	void zero_setting();

	template <typename F>
	big_integer& bit_operation(F operation, big_integer const& rhs);

	bool small;
	std::int32_t number;
	my_vector<std::uint32_t> digits;
};

big_integer operator+(big_integer a, big_integer const& b);
big_integer operator-(big_integer a, big_integer const& b);
big_integer operator*(big_integer a, big_integer const& b);
big_integer operator/(big_integer a, big_integer const& b);
big_integer operator%(big_integer a, big_integer const& b);

big_integer operator&(big_integer a, big_integer const& b);
big_integer operator|(big_integer a, big_integer const& b);
big_integer operator^(big_integer a, big_integer const& b);

big_integer operator<<(big_integer a, int b);
big_integer operator>>(big_integer a, int b);

bool operator==(big_integer const& a, big_integer const& b);
bool operator!=(big_integer const& a, big_integer const& b);

bool operator<(big_integer const& a, big_integer const& b);
bool operator>(big_integer const& a, big_integer const& b);
bool operator<=(big_integer const& a, big_integer const& b);
bool operator>=(big_integer const& a, big_integer const& b);

std::string to_string(big_integer const& a);
std::ostream& operator<<(std::ostream& s, big_integer const& a);


template <typename F>
big_integer& big_integer::bit_operation(F operation, big_integer const& rhs)
{
	if (small && rhs.small)
	{
		number = operation(number, rhs.number);
		return *this;
	}
	to_big();

	big_integer b = rhs;
	b.to_big();

	for (int i = 0; i < digits.size() || i < b.digits.size(); ++i)
	{
		if (i >= digits.size())
		{
			digits.push_back(operation(at(i), b.at(i)));
		}
		else
		{
			digits[i] = operation(digits[i], b.at(i));
		}
	}

	remove_redundancy();

	return *this;
}
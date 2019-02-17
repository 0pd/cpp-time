#include <sstream>
#include <iomanip>
#include <iostream>
#include <functional>
#include "big_integer.h"

big_integer::big_integer()
: small(true), number(0), digits(my_vector<std::uint32_t>())
{
}

big_integer::big_integer(big_integer const& other)
: small(other.small), number(other.number), digits(other.digits)
{
}

big_integer::big_integer(int a)
: small(true), number(a), digits(my_vector<std::uint32_t>())
{
}

big_integer::big_integer(std::string const& str)
: big_integer()
{
	bool negative = str.at(0) == '-';
	for (auto it = negative ? str.begin() + 1 : str.begin(); it != str.end(); ++it)
	{
		*this *= 10;
		*this += *it - '0';
	}
	if (negative) *this = -*this;
}


big_integer& big_integer::operator=(big_integer const& other) &
{
	if (*this == other)
		return *this;

	small = other.small;
	number = other.number;
	digits = other.digits;
	return *this;
}


big_integer& big_integer::operator+=(big_integer const& rhs) &
{
	if (small && rhs.small)
	{
		std::uint64_t result = static_cast<std::int64_t>(number)+static_cast<std::int64_t>(rhs.number);
		zero_setting();
		digits.push_back(result % (1ULL << 32));
		digits.push_back(result >> 32);
		remove_redundancy();
		return *this;
	}

	to_big();

	big_integer b = rhs;
	b.to_big();

	std::uint64_t carry = 0;
	int count = digits.size() > b.digits.size() ? digits.size() : b.digits.size();
	for (int i = 0; i < count; ++i)
	{
		if (i > digits.size())
		{
			digits.push_back(at(i));
		}

		carry += static_cast<std::uint64_t>(digits[i]) + static_cast<std::uint64_t>(b.at(i));

		digits[i] = static_cast<std::uint32_t>(carry % (1ULL << 32));
		carry >>= 32;
	}
	remove_redundancy();

	return *this;
}

big_integer& big_integer::operator-=(big_integer const& rhs) &
{
	*this += -rhs;
	return *this;
}

big_integer& big_integer::operator*=(big_integer const& rhs) &
{
	if (small && rhs.small)
	{
		std::uint64_t result = static_cast<std::int64_t>(number)* static_cast<std::int64_t>(rhs.number);
		zero_setting();
		digits.push_back(result % (1ULL << 32));
		digits.push_back(result >> 32);
		remove_redundancy();
		return *this;
	}

	bool sign = signum() ^ rhs.signum();

	big_integer a = signum() ? -*this : *this;
	big_integer b = rhs.signum() ? -rhs : rhs;
	a.to_big();
	b.to_big();

	big_integer result;
	result.to_big();
	result.digits.insert(result.digits.cend(), a.digits.size() + b.digits.size(), 0);
	for (auto i = 0; i < a.digits.size(); ++i)
	{
		std::uint64_t carry = 0;
		for (auto j = 0; j < b.digits.size(); ++j)
		{
			std::uint64_t s = static_cast<std::uint64_t>(result.at(i + j)) + static_cast<std::uint64_t>(a.at(i)) * static_cast<std::uint64_t>(b.at(j)) + carry;
			result.digits[i + j] = static_cast<std::uint32_t>(s % (1ULL << 32));
			carry = s >> 32;
		}
		if (carry != 0)
		{
			result.digits[i + b.digits.size()] = carry;
		}
	}
	result.remove_redundancy();
	if (result.signum()) result.digits.push_back(0);

	return *this = sign ? -result : result;
}

big_integer& big_integer::operator/=(big_integer const& rhs) &
{
	if (rhs == 0) throw std::runtime_error("division by zero");

	if (small && rhs.small)
	{
		return *this = number / rhs.number;
	}

	bool sign = signum() ^ rhs.signum();

	big_integer a = signum() ? -*this : *this;
	big_integer b = rhs.signum() ? -rhs : rhs;

	if (a < b) return *this = 0;

	int n = a.digits_count() - b.digits_count();

	big_integer result;
	result.to_big();
	if (b.small)
	{
		result.digits.insert(result.digits.cend(), a.digits.size() - 1, 0);
		std::uint64_t dividend = 0;
		for (int i = a.digits.size() - 1; i >= 0; --i)
		{
			dividend |= a.digits[i];
			result.digits[i] = dividend / b.number;
			dividend = (dividend % b.number) << 32;
		}
	}
	else
	{
		result.digits.insert(result.digits.cend(), (n + 1) / 32, 0);
		if (a < b << n)
		{
			n--;
		}
		while (a >= b)
		{
			a -= b << n;
			result.set_bit(n, true);
			while (a < b << n && n > 0)
			{
				n--;
			}
		}
	}
	result.remove_redundancy();

	return *this = sign ? -result : result;
}

big_integer& big_integer::operator%=(big_integer const& rhs)  &
{
	return *this = *this - rhs * (*this / rhs);
}


big_integer& big_integer::operator&=(big_integer const& rhs) &
{
	return bit_operation(std::bit_and<std::uint32_t>(), rhs);
}

big_integer& big_integer::operator|=(big_integer const& rhs) &
{
	return bit_operation(std::bit_or<std::uint32_t>(), rhs);
}

big_integer& big_integer::operator^=(big_integer const& rhs) &
{
	return bit_operation(std::bit_xor<std::uint32_t>(), rhs);
}


big_integer& big_integer::operator<<=(int rhs) &
{
	to_big();

	int real_shift = rhs % 32;
	if (real_shift != 0)
	{
		digits.push_back(static_cast<std::int32_t>(digits.back()) >> (32 - real_shift));
		for (int i = static_cast<int>(digits.size()) - 2; i > 0; --i)
		{
			digits[i] = (digits[i] << real_shift) + (digits[i - 1] >> (32 - real_shift));
		}
		digits.front() <<= real_shift;
	}
	digits.insert(digits.begin(), rhs / 32, 0);

	remove_redundancy();

	return *this;
}


big_integer& big_integer::operator>>=(int rhs) &
{
	to_big();

	digits.erase(digits.cbegin(), digits.cbegin() + rhs / 32);

	int real_shift = rhs % 32;
	if (real_shift != 0)
	{
		for (auto it = digits.begin(); it != digits.end(); ++it)
		{
			if (it != digits.begin())
			{
				std::uint32_t temp = *it % (1 << real_shift);
				*(it - 1) += temp << (32 - real_shift);
			}
			if (it + 1 == digits.end())
			{
				*it = static_cast<std::int32_t> (*it) >> real_shift;
				continue;
			}
			*it >>= real_shift;
		}
	}
	remove_redundancy();

	return *this;
}


big_integer big_integer::operator+() const
{
	return *this;
}

big_integer big_integer::operator-() const
{
	if (small) return big_integer(-number);

	return ~*this + 1;
}

big_integer big_integer::operator~() const
{
	if (small) return big_integer(~number);

	big_integer result = *this;
	for (auto it = result.digits.begin(); it != result.digits.end(); ++it)
	{
		*it = ~*it;
	}

	return result;
}


big_integer& big_integer::operator++() &
{
	return *this += 1;
}

big_integer big_integer::operator++(int) &
{
	big_integer temp = *this;
	*this += 1;
	return temp;
}


big_integer& big_integer::operator--() &
{
	return *this -= 1;
}

big_integer big_integer::operator--(int) &
{
	big_integer temp = *this;
	*this -= 1;
	return temp;
}


bool operator==(big_integer const& a, big_integer const& b)
{
	if (a.small && b.small) return a.number == b.number;
	else if (a.small || b.small) return false;
	return a.digits == b.digits;
}

bool operator!=(big_integer const& a, big_integer const& b)
{
	return !(a == b);
}


bool operator<(big_integer const& a, big_integer const& b)
{
	if (a.small && b.small) return a.number < b.number;
	if (!a.signum() && b.signum()) return false;
	if (a.signum() && !b.signum()) return true;
	if (a.signum() && b.signum())
	{
		if (a.small) return false;
		if (b.small) return true;
		if (a.digits.size() > b.digits.size()) return true;
		if (a.digits.size() < b.digits.size()) return false;
		return !std::lexicographical_compare(a.digits.crbegin(), a.digits.crend(), b.digits.crbegin(), b.digits.crend());
	}
	if (a.small) return true;
	if (b.small) return false;
	if (a.digits.size() < b.digits.size()) return true;
	if (a.digits.size() > b.digits.size()) return false;
	return std::lexicographical_compare(a.digits.crbegin(), a.digits.crend(), b.digits.crbegin(), b.digits.crend());
}

bool operator>(big_integer const& a, big_integer const& b)
{
	return b < a;
}

bool operator<=(big_integer const& a, big_integer const& b)
{
	return a < b || a == b;
}

bool operator>=(big_integer const& a, big_integer const& b)
{
	return a > b || a == b;
}


big_integer operator+(big_integer a, big_integer const& b)
{
	return a += b;
}

big_integer operator-(big_integer a, big_integer const& b)
{
	return a -= b;
}

big_integer operator*(big_integer a, big_integer const& b)
{
	return a *= b;
}

big_integer operator/(big_integer a, big_integer const& b)
{
	return a /= b;
}

big_integer operator%(big_integer a, big_integer const& b)
{
	return a %= b;
}


big_integer operator&(big_integer a, big_integer const& b)
{
	return a &= b;
}

big_integer operator|(big_integer a, big_integer const& b)
{
	return a |= b;
}

big_integer operator^(big_integer a, big_integer const& b)
{
	return a ^= b;
}


big_integer operator<<(big_integer a, int b)
{
	return a <<= b;
}

big_integer operator>>(big_integer a, int b)
{
	return a >>= b;
}


std::string to_string(big_integer const& a)
{
	if (a.small) return std::to_string(a.number);

	std::ostringstream result;
	std::vector<std::uint32_t> result_vector;
	big_integer temp = a;

	if (a.signum())
	{
		result << "-";
		temp = -temp;
	}

	const int base = static_cast<int>(1e9);

	do
	{
		big_integer t = temp % base;
		result_vector.push_back(t.small ? t.number : t.digits[0]);
		temp /= base;
	} while (temp > 0);

	for (auto it = result_vector.crbegin(); it != result_vector.crend(); ++it)
	{
		if (it == result_vector.crbegin())
		{
			result << *it;
			continue;
		}
		result << std::setfill('0') << std::setw(9) << *it;
	}

	return result.str();
}

std::ostream& operator<<(std::ostream& s, big_integer const& a)
{
	return s << to_string(a);
}

void big_integer::to_big()
{
	if (small)
	{
		auto temp = number;
		zero_setting();
		digits.push_back(temp);
	}
}

bool big_integer::signum() const
{
	if (small) return number < 0;
	return at(digits.size()) == -1;
}


std::uint32_t big_integer::at(size_t index) const
{
	if (index >= digits.size())
	{
		return (digits.back() >> 31 & 1) == 1 ? -1 : 0;
	}
	return digits[index];
}

size_t big_integer::digits_count() const
{
	size_t result = small ? 0 : (digits.size() - 1) * 32;
	std::uint32_t x = small ? number : digits.back();

	if (x == 0 || x == -1)
	{
		return result + 1;
	}

	result += 32;

	for (auto i = 31; i >= 0 && (x >> 31) == ((x >> i) & 1); --i)
	{
		--result;
	}

	return result + 1;
}

void big_integer::set_bit(size_t which, bool value)
{
	if (value)
	{
		digits[which / 32] |= 1 << (which % 32);
	}
	else
	{
		digits[which / 32] &= ~(1 << (which % 32));
	}
}

void big_integer::remove_redundancy()
{
	if (small) return;

	bool sign = signum();
	while (digits.size() > 1 && (digits.back() == 0 || digits.back() == -1))
	{
		digits.pop_back();
		if (sign != signum())
		{
			digits.push_back(sign ? -1 : 0);
			break;
		}
	}

	if (digits.size() == 1)
	{
		small = true;
		number = digits.back();
		digits = my_vector<std::uint32_t>();
	}
}

void big_integer::zero_setting()
{
	small = false;
	number = 0;
	digits = my_vector<std::uint32_t>();
}
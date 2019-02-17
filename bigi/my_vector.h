#pragma once
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <memory>

template <typename T>
struct my_vector
{
	my_vector();
	explicit my_vector(size_t n);
	my_vector(size_t n, T const& value);
	my_vector(const my_vector& other);
	my_vector(my_vector&& other);
	my_vector(std::initializer_list<T> iList);
	~my_vector();

	my_vector& operator=(my_vector const& rhs);
	my_vector& operator=(std::initializer_list<T> iList);

	T& at(size_t pos);
	T const& at(size_t pos) const;

	T& operator[](size_t pos);
	T const& operator[](size_t pos) const;

	T& front();
	T const& front() const;
	T& back();
	T const& back() const;

	typedef T * iterator;
	typedef T const * const_iterator;

	iterator begin();
	const_iterator begin() const;
	const_iterator cbegin() const;
	iterator end();
	const_iterator end() const;
	const_iterator cend() const;

	std::reverse_iterator<iterator> rbegin();
	const std::reverse_iterator<const_iterator> crbegin()const;
	std::reverse_iterator<iterator> rend();
	const std::reverse_iterator<const_iterator> crend()const;

	bool empty() const;
	size_t size() const;
	size_t capacity() const;

	void push_back(T const& value);
	void push_back(T&& value);

	void pop_back();

	void clear();
	iterator insert(const_iterator pos, T const& value);
	iterator insert(const_iterator pos, T&& value);
	iterator insert(const_iterator pos, size_t count, const T& value);
	iterator erase(const_iterator pos);
	iterator erase(const_iterator first, const_iterator last);

private:
	struct copy_on_write
	{
		copy_on_write(size_t capacity, size_t size, T * data);
		~copy_on_write();

		T * data;
		size_t capacity;
		size_t size;
	};

	std::shared_ptr<copy_on_write> data;

	void expand(size_t capacity = 0);
	void contract();
	void init(size_t capacity, size_t size);
	void init(size_t capacity);
	void fork();
};

template <typename T>
my_vector<T>::copy_on_write::copy_on_write(size_t capacity, size_t size, T* data)
: capacity(capacity), size(size), data(data)
{
}

template <typename T>
my_vector<T>::copy_on_write::~copy_on_write()
{
	delete reinterpret_cast<void *>(data);
}

template <typename T>
my_vector<T>::my_vector()
	: data(std::make_shared<copy_on_write>(copy_on_write(0, 0, nullptr)))
{
}

template <typename T>
my_vector<T>::my_vector(my_vector const& other)
{
	data = other.data;
}

template <typename T>
my_vector<T>::my_vector(my_vector&& other)
	: data(other.data)
{
	other.init(0);
}

template <typename T>
my_vector<T>::my_vector(size_t n)
	: data(std::make_shared<copy_on_write>(copy_on_write(n, 0, nullptr)))
{
	data->data = reinterpret_cast<T*> (operator new (data->capacity * sizeof(T)));

	for (int i = 0; i < n; ++i)
	{
		push_back(T());
	}
}

template <typename T>
my_vector<T>::my_vector(size_t n, T const& value)
	: data(std::make_shared<copy_on_write>(copy_on_write(n, 0, nullptr)))
{
	data->data = reinterpret_cast<T*> (operator new (data->capacity * sizeof(T)));

	for (int i = 0; i < n; ++i)
	{
		push_back(value);
	}
}

template <typename T>
my_vector<T>::my_vector(std::initializer_list<T> iList)
	: data(std::make_shared<copy_on_write>(copy_on_write(iList.size(), iList.size(), nullptr)))
{
	data->data = reinterpret_cast<T*> (operator new (data->capacity * sizeof(T)));

	std::copy(iList.begin(), iList.end(), data->data);
}

template <typename T>
my_vector<T>::~my_vector()
{
	clear();
}

template <typename T>
my_vector<T>& my_vector<T>::operator=(my_vector const& rhs)
{
	data = rhs.data;

	return *this;
}

template <typename T>
my_vector<T>& my_vector<T>::operator=(std::initializer_list<T> iList)
{
	clear();

	init(iList.size(), iList.size());

	std::copy(iList.begin(), iList.end(), data->data);

	return *this;
}

template <typename T>
T& my_vector<T>::at(size_t pos)
{
	fork();
	if (pos >= size) throw std::out_of_range("Trying to access to nonexistent element");

	return data->data[pos];
}

template <typename T>
T const& my_vector<T>::at(size_t pos) const
{
	if (pos >= size) throw std::out_of_range("Trying to access to nonexistent element");

	return data->data[pos];
}

template <typename T>
T& my_vector<T>::operator[](size_t pos)
{
	fork();
	return data->data[pos];
}

template <typename T>
T const& my_vector<T>::operator[](size_t pos) const
{
	return data->data[pos];
}

template <typename T>
T& my_vector<T>::front()
{
	fork();
	return data->data[0];
}

template <typename T>
T const& my_vector<T>::front() const
{
	return data->data[0];
}

template <typename T>
T& my_vector<T>::back()
{
	fork();
	return data->data[data->size - 1];
}

template <typename T>
T const& my_vector<T>::back() const
{
	return data->data[data->size - 1];
}

template <typename T>
typename my_vector<T>::iterator my_vector<T>::begin()
{
	fork();
	return data->data;
}

template <typename T>
typename my_vector<T>::const_iterator my_vector<T>::begin() const
{
	return data->data;
}

template <typename T>
typename my_vector<T>::const_iterator my_vector<T>::cbegin() const
{
	return data->data;
}

template <typename T>
typename my_vector<T>::iterator my_vector<T>::end()
{
	fork();
	return data->data + data->size;
}

template <typename T>
typename my_vector<T>::const_iterator my_vector<T>::end() const
{
	return data->data + data->size;
}

template <typename T>
typename my_vector<T>::const_iterator my_vector<T>::cend() const
{
	return data->data + data->size;
}

template <typename T>
std::reverse_iterator<T*> my_vector<T>::rbegin()
{
	return std::reverse_iterator<iterator>(end());
}

template <typename T>
std::reverse_iterator<T const*> const my_vector<T>::crbegin() const
{
	return std::reverse_iterator<const_iterator>(cend());
}

template <typename T>
std::reverse_iterator<T*> my_vector<T>::rend()
{
	return std::reverse_iterator<iterator>(begin());
}

template <typename T>
std::reverse_iterator<T const*> const my_vector<T>::crend() const
{
	return std::reverse_iterator<const_iterator>(cbegin());
}

template <typename T>
bool my_vector<T>::empty() const
{
	return data->size == 0;
}

template <typename T>
size_t my_vector<T>::size() const
{
	return data->size;
}

template <typename T>
size_t my_vector<T>::capacity() const
{
	return data->capacity;
}

template <typename T>
void my_vector<T>::push_back(T const& value)
{
	fork();
	if (data->size == data->capacity)
	{
		expand();
	}

	data->data[data->size++] = value;
}

template <typename T>
void my_vector<T>::push_back(T&& value)
{
	fork();
	if (data->size == data->capacity)
	{
		expand();
	}

	data->data[data->size++] = std::move(value);

	value = 0;
}

template <typename T>
void my_vector<T>::pop_back()
{
	fork();
	if (empty()) throw std::out_of_range("Empty vector");
	data->data[data->size--].~T();

	if (data->size <= data->capacity / 4)
	{
		contract();
	}
}

template <typename T>
void my_vector<T>::clear()
{
	fork();
	for (auto it = begin(); it != end(); ++it)
	{
		it->~T();
	}
}


template <typename T>
typename my_vector<T>::iterator my_vector<T>::insert(const_iterator pos, T const& value)
{
	return insert(pos, 1, value);
}

template <typename T>
typename my_vector<T>::iterator my_vector<T>::insert(const_iterator pos, T&& value)
{
	fork();
	iterator old_begin = begin();
	if (data->size == data->capacity)
	{
		expand(data->size + 1);
	}
	pos = begin() + (pos - old_begin);

	for (auto it = end(); it != begin() + (pos - begin()); --it)
	{
		*it = *(it - 1);
	}

	*(begin() + (pos - begin())) = std::move(value);
	++data->size;

	return begin() + (pos - begin());
}

template <typename T>
typename my_vector<T>::iterator my_vector<T>::insert(const_iterator pos, size_t count, T const& value)
{
	fork();
	iterator old_begin = begin();
	if (data->size + count > data->capacity)
	{
		expand(data->size + count);
	}
	pos = begin() + (pos - old_begin);

	for (auto it = end(); it != begin() + (pos - begin()); --it)
	{
		*(it - 1 + count) = *(it - 1);
	}

	for (auto it = begin() + (pos - begin()); it != pos + count; ++it)
	{
		*it = value;
	}
	data->size += count;

	return begin() + (pos - begin());
}

template <typename T>
typename my_vector<T>::iterator my_vector<T>::erase(const_iterator pos)
{
	return erase(pos, pos + 1);
}

template <typename T>
typename my_vector<T>::iterator my_vector<T>::erase(const_iterator first, const_iterator last)
{
	fork();
	int count = std::distance(first, last);
	if (count == 0) return begin() + (first - begin());
	for (auto it = begin() + (first - begin()); it != begin() + (last - begin()) - 1; ++it)
	{
		it->~T();
		new(it) T(std::move(*(begin() + (last - begin()) + 1)));
	}
	data->size -= count;
	contract();

	return begin() + (first - begin());
}


template <typename T>
void my_vector<T>::expand(size_t capacity)
{
	data->capacity = capacity == 0 || capacity < data->capacity ? data->capacity * 2 : capacity;
	if (data->capacity == 0) ++data->capacity;
	T * new_array = reinterpret_cast<T*> (operator new (data->capacity * sizeof(T)));
	std::move(data->data, data->data + data->size, new_array);
	delete reinterpret_cast<void *> (data->data);
	data->data = new_array;
}

template <typename T>
void my_vector<T>::contract()
{
	data->capacity /= 2;
	if (data->capacity == 0)
	{
		clear();
	}
	else
	{
		T * new_array = reinterpret_cast<T*> (operator new (data->capacity * sizeof(T)));
		std::move(data->data, data->data + data->size, new_array);
		clear();
		delete reinterpret_cast<void *> (data->data);
		data->data = new_array;
	}
}

template <typename T>
void my_vector<T>::init(size_t capacity, size_t size)
{
	if (data.unique())
	{
		clear();
		if (capacity != 0) delete reinterpret_cast<void *> (data->data); // if nullptr it has no effect
		data->capacity = capacity;
		data->size = size;
	}
	else
	{
		data = std::make_shared<copy_on_write>(copy_on_write(capacity, size, nullptr));
	}
	data->data = capacity != 0 ? reinterpret_cast<T*> (operator new (capacity * sizeof(T))) : nullptr;
}

template <typename T>
void my_vector<T>::init(size_t capacity)
{
	if (data.unique())
	{
		clear();
		if (capacity != 0) delete reinterpret_cast<void *> (data->data); // if nullptr it has no effect
		data->capacity = capacity;
	}
	else
	{
		data = std::make_shared<copy_on_write>(copy_on_write(capacity, 0, nullptr));
	}
	data->data = capacity != 0 ? reinterpret_cast<T*> (operator new (capacity * sizeof(T))) : nullptr;
}

template <typename T>
bool operator==(my_vector<T> const& lhs, my_vector<T> const& rhs)
{
	return lhs.size() == rhs.size() && std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
}

template <typename T>
bool operator!=(my_vector<T> const& lhs, my_vector<T> const& rhs)
{
	return !(lhs == rhs);
}

template <typename T>
void my_vector<T>::fork()
{
	if (!data.unique())
	{
		std::shared_ptr<copy_on_write> new_data = std::make_shared<copy_on_write>(copy_on_write(data->capacity, data->size, nullptr));
		new_data->data = reinterpret_cast<T*> (operator new (data->capacity * sizeof(T)));
		std::copy(data->data, data->data + data->size, new_data->data);
		data = new_data;
	}
}
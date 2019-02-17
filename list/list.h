#include <iterator>
#include <utility>
#include <stdexcept>

namespace detail
{
	template< typename T >
	struct list_node
	{
		list_node(T value, list_node * next, list_node * previous);
		T value;
		list_node * next;
		list_node * previous;
	};

	template< typename T >
	list_node<T>::list_node(T value, list_node * next, list_node * previous)
		: value(value), next(next), previous(previous)
	{
	}
}

template< typename T >
struct list
{
	list();
	list(size_t count, T const& value);
	explicit list(size_t count);
	list(list const& other);
	list(list && other);
	list(std::initializer_list<T> iList);
	~list();
	list<T>& operator=(list const& rhs);
	list<T>& operator=(list && rhs);
	list<T>& operator=(std::initializer_list<T> iList);

	void push_back(T value);
	void pop_back();

	void push_front(T value);
	void pop_front();

	T const& front() const;
	T& front();
	T const& back() const;
	T& back();

	bool empty() const;
	size_t size() const;

	void clear();

	struct iterator : public std::iterator<std::bidirectional_iterator_tag, T>
	{
		T& operator*();
		iterator operator++();
		iterator operator++(int);
		iterator operator--();
		iterator operator--(int);

		friend bool operator==(iterator const& lhs, iterator const& rhs)
		{
			return lhs.node_ == rhs.node_;
		}

		friend bool operator!=(iterator const& lhs, iterator const& rhs)
		{
			return !(lhs == rhs);
		}

		friend struct list;
	private:
		explicit iterator(detail::list_node<T> * node);
		detail::list_node<T> * node_;
	};

	typedef const iterator const_iterator;

	iterator begin();
	iterator end();
	iterator begin() const;
	iterator end() const;
	const_iterator cbegin() const;
	const_iterator cend() const;

	std::reverse_iterator<iterator> rbegin();
	std::reverse_iterator<iterator> rend();
	std::reverse_iterator<const_iterator> crbegin() const;
	std::reverse_iterator<const_iterator> crend() const;

	void swap(list& other);

	iterator insert(const_iterator pos, T const& value);
	iterator insert(const_iterator pos, T && value);
	iterator insert(const_iterator pos, size_t count, T const& value);
	template< typename InputIt >
	typename std::enable_if<!std::is_integral<InputIt>::value, iterator>::type
		insert(const_iterator pos, InputIt first, InputIt last);
	iterator insert(const_iterator pos, std::initializer_list<T> iList);

	iterator erase(const_iterator pos);
	iterator erase(const_iterator first, const_iterator last);

	void splice(const_iterator pos, list& other);
	void splice(const_iterator pos, list&& other);
	void splice(const_iterator pos, list& other, const_iterator it);
	void splice(const_iterator pos, list&& other, const_iterator it);
	void splice(const_iterator pos, list& other, const_iterator first, const_iterator last);
	void splice(const_iterator pos, list&& other, const_iterator first, const_iterator last);

private:
	size_t size_;
	detail::list_node<T> * head_;
};


template< typename T >
list<T>::list()
	: size_(0), head_(new detail::list_node<T>(0, nullptr, nullptr))
{
	head_->next = head_;
	head_->previous = head_;
}

template <typename T>
list<T>::list(size_t count, T const& value)
: list()
{
	try
	{
		for (auto i = 0; i < count; ++i)
		{
			push_back(value);
		}
	}
	catch (...)
	{
		clear();
		delete head_;
		throw;
	}
}

template <typename T>
list<T>::list(size_t count)
	: list(count, T())
{
}

template< typename T >
list<T>::list(list const& other)
	: list()
{
	try
	{
		for (auto it = other.begin(); it != other.end(); ++it)
		{
			push_back(*it);
		}
	}
	catch (...)
	{
		clear();
		delete head_;
		throw;
	}
}

template <typename T>
list<T>::list(list&& other)
	: list()
{
	swap(other);
}

template< typename T >
list<T>::list(std::initializer_list<T> iList)
	: list()
{
	try
	{
		for (auto i : iList)
		{
			push_back(i);
		}
	}
	catch (...)
	{
		clear();
		delete head_;
		throw;
	}
}

template< typename T >
list<T>::~list()
{
	clear();
	delete head_;
}

template< typename T >
list<T>& list<T>::operator=(list<T> const& rhs)
{
	list(rhs).swap(*this);
	return *this;
}

template< typename T >
list<T>& list<T>::operator=(list<T> && rhs)
{
	swap(rhs);
	return *this;
}

template< typename T >
list<T>& list<T>::operator=(std::initializer_list<T> iList)
{
	list(iList).swap(*this);
	return *this;
}

template< typename T >
void list<T>::push_back(T value)
{
	detail::list_node<T> * new_node = new detail::list_node<T>(value, head_, head_->previous);
	head_->previous = new_node;
	new_node->previous->next = new_node;
	++size_;
}

template< typename T >
void list<T>::pop_back()
{
	if (empty()) throw std::out_of_range("empty list");

	detail::list_node<T> * old_node = head_->previous;
	head_->previous = head_->previous->previous;
	head_->previous->next = head_;
	delete old_node;
	--size_;
}


template< typename T >
void list<T>::push_front(T value)
{
	detail::list_node<T> * new_node = new detail::list_node<T>(value, head_->next, head_);
	head_->next = new_node;
	new_node->next->previous = new_node;
	++size_;
}

template< typename T >
void list<T>::pop_front()
{
	if (empty()) throw std::out_of_range("empty list");

	detail::list_node<T> * old_node = head_->next;
	head_->next = head_->next->next;
	head_->next->previous = head_;
	delete old_node;
	--size_;
}


template< typename T >
T const& list<T>::front() const
{
	if (empty()) throw std::out_of_range("empty list");

	return head_->next->value;
}

template< typename T >
T& list<T>::front()
{
	if (empty()) throw std::out_of_range("empty list");

	return head_->next->value;
}

template< typename T >
T const& list<T>::back() const
{
	if (empty()) throw std::out_of_range("empty list");

	return head_->previous->value;
}

template< typename T >
T& list<T>::back()
{
	if (empty()) throw std::out_of_range("empty list");

	return head_->previous->value;
}


template< typename T >
bool list<T>::empty() const
{
	return size_ == 0;
}

template< typename T >
size_t list<T>::size() const
{
	return size_;
}

template< typename T >
void list<T>::clear()
{
	erase(begin(), end());
}


template< typename T >
typename list<T>::iterator list<T>::begin()
{
	return iterator(head_->next);
}

template< typename T >
typename list<T>::iterator list<T>::begin() const
{
	return iterator(head_->next);
}

template< typename T >
typename list<T>::iterator list<T>::end()
{
	return iterator(head_);
}

template< typename T >
typename list<T>::iterator list<T>::end() const
{
	return iterator(head_);
}

template< typename T >
typename list<T>::const_iterator list<T>::cbegin() const
{
	return const_iterator(head_->next);
}

template< typename T >
typename list<T>::const_iterator list<T>::cend() const
{
	return const_iterator(head_);
}

template< typename T >
std::reverse_iterator<typename list<T>::iterator> list<T>::rbegin()
{
	return std::reverse_iterator<list<T>::iterator>(end());
}

template< typename T >
std::reverse_iterator<typename list<T>::iterator> list<T>::rend()
{
	return std::reverse_iterator<list<T>::iterator>(begin());
}

template< typename T >
std::reverse_iterator<typename list<T>::const_iterator> list<T>::crbegin() const
{
	return std::reverse_iterator<const_iterator>(cend());
}

template< typename T >
std::reverse_iterator<typename list<T>::const_iterator> list<T>::crend() const
{
	return std::reverse_iterator<const_iterator>(cbegin());
}


template< typename T >
typename list<T>::iterator list<T>::insert(const_iterator pos, T const& value)
{
	return insert(pos, 1, value);
}

template <typename T>
typename list<T>::iterator list<T>::insert(const_iterator pos, T&& value)
{
	iterator res = pos;
	--res;

	detail::list_node<T> * new_node = new detail::list_node<T>(0, pos.node_, pos.node_->previous);
	pos.node_->previous = new_node;
	new_node->previous->next = new_node;
	std::swap(new_node->value, value);
	++size_;

	return res;
}

template< typename T >
typename list<T>::iterator list<T>::insert(const_iterator pos, size_t count, T const& value)
{
	iterator res = pos;
	--res;
	for (size_t i = 0; i < count; ++i, ++size_)
	{
		detail::list_node<T> * new_node = new detail::list_node<T>(value, pos.node_, pos.node_->previous);
		pos.node_->previous = new_node;
		new_node->previous->next = new_node;
	}
	return res;
}

template< typename T >
template< typename InputIt >
typename std::enable_if<!std::is_integral<InputIt>::value, typename list<T>::iterator>::type
	list<T>::insert(const_iterator pos, InputIt first, InputIt last)
{
	iterator res = pos;
	--res;
	for (auto it = first; it != last; ++size_, ++it)
	{
		detail::list_node<T> * new_node = new detail::list_node<T>(*it, pos.node_, pos.node_->previous);
		pos.node_->previous = new_node;
		new_node->previous->next = new_node;
	}
	return res;
}

template <typename T>
typename list<T>::iterator list<T>::insert(const_iterator pos, std::initializer_list<T> iList)
{
	insert(pos, iList.begin(), iList.end());
}


template< typename T >
typename list<T>::iterator list<T>::erase(const_iterator pos)
{
	if (empty()) throw std::out_of_range("empty list");
	if (pos == cend()) throw std::runtime_error("can't delete end=(");

	return erase(pos, const_iterator(pos.node_->next));
}

template< typename T >
typename list<T>::iterator list<T>::erase(const_iterator first, const_iterator last)
{
	if (empty()) throw std::out_of_range("empty list");
	if (first == cend()) throw std::runtime_error("can't delete end=(");

	for (iterator it = first; it != last; --size_)
	{
		it.node_->next->previous = it.node_->previous;
		it.node_->previous->next = it.node_->next;
		detail::list_node<T> * old_node = it.node_;
		++it;
		delete old_node;
	}

	return last;
}


template <typename T>
void list<T>::splice(const_iterator pos, list& other)
{
	insert(pos, other.begin(), other.end());
	other.clear();
}

template <typename T>
void list<T>::splice(const_iterator pos, list&& other)
{
	insert(pos, other.begin(), other.end());
	other.clear();
}

template <typename T>
void list<T>::splice(const_iterator pos, list& other, const_iterator it)
{
	insert(pos, *it);
	other.erase(it);
}

template <typename T>
void list<T>::splice(const_iterator pos, list&& other, const_iterator it)
{
	insert(pos, *it);
	other.erase(it);
}

template <typename T>
void list<T>::splice(const_iterator pos, list& other, const_iterator first, const_iterator last)
{
	insert(pos, first, last);
	other.erase(first, last);
}

template <typename T>
void list<T>::splice(const_iterator pos, list&& other, const_iterator first, const_iterator last)
{
	insert(pos, first, last);
	other.erase(first, last);
}

template< typename T >
void list<T>::swap(list& other)
{
	std::swap(head_, other.head_);
	std::swap(size_, other.size_);
}


template< typename T >
list<T>::iterator::iterator(detail::list_node<T> * node)
	: node_(node)
{
}

template< typename T >
T& list<T>::iterator::operator*()
{
	return node_->value;
}

template< typename T >
typename list<T>::iterator list<T>::iterator::operator++()
{
	return *this = iterator(node_->next);
}

template< typename T >
typename list<T>::iterator list<T>::iterator::operator++(int)
{
	iterator temp = *this;
	*this = iterator(node_->next);
	return temp;
}

template< typename T >
typename list<T>::iterator list<T>::iterator::operator--()
{
	return *this = iterator(node_->previous);
}

template< typename T >
typename list<T>::iterator list<T>::iterator::operator--(int)
{
	iterator temp = *this;
	*this = iterator(node_->previous);
	return temp;
}
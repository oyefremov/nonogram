#pragma once
#include <utility>


template <class T>
class step_iterator_t
{
	T ptr;
	size_t step;
public:
	typedef step_iterator_t<T> me;
	typedef typename std::iterator_traits<T>::reference reference;
	typedef typename std::iterator_traits<T>::value_type value_type;

	me(T _ptr, size_t _step) : ptr(_ptr), step(_step) {}

	value_type operator*() const {return *ptr;}
	reference operator*() {return *ptr;}

	me& operator++()
	{
		ptr += step;
		return *this;
	}

	me operator++(int)
	{	// postincrement
		me res = *this;
		ptr += step;
		return res;
	}

	me& operator--()
	{
		ptr -= step;
		return *this;
	}

	me operator--(int)
	{
		me res = *this;
		ptr -= step;
		return res;
	}

	me& operator+=(size_t _Off)
	{
		ptr += _Off * step;
		return (*this);
	}

	me operator+(size_t _Off) const
	{	// return this + integer
		me _Tmp = *this;
		return (_Tmp += _Off);
	}

	me& operator-=(size_t _Off)
	{	// decrement by integer
		return (*this += -_Off);
	}

	me operator-(size_t _Off) const
	{	// return this - integer
		me _Tmp = *this;
		return (_Tmp -= _Off);
	}

	size_t operator-(const me& _Right) const
	{	// return difference of iterators
		return (this->ptr - _Right.ptr) / step;
	}

	value_type operator[](size_t _Off) const
	{	// subscript
		return ptr[_Off * step];
	}

	reference operator[](size_t _Off)
	{	// subscript
		return ptr[_Off * step];
	}

	bool operator==(const me& _Right) const
	{	// test for iterator equality
		return (this->ptr == _Right.ptr);
	}

	bool operator!=(const me& _Right) const
	{	// test for iterator inequality
		return (!(*this == _Right));
	}

	bool operator<(const me& _Right) const
	{	// test if this < _Right
		return (this->ptr < _Right.ptr);
	}

	bool operator>(const me& _Right) const
	{	// test if this > _Right
		return (_Right < *this);
	}

	bool operator<=(const me& _Right) const
	{	// test if this <= _Right
		return (!(_Right < *this));
	}

	bool operator>=(const me& _Right) const
	{	// test if this >= _Right
		return (!(*this < _Right));
	}
};

namespace std
{
	template <class T>
	struct iterator_traits<step_iterator_t<T>> : public std::iterator_traits<T>	{};
}
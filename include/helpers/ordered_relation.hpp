#ifndef ORDERED_RELATION_HPP
#define ORDERED_RELATION_HPP

#include <cstddef>
#include <vector>

#include "helpers/math.hpp"

namespace helpers
{

template <typename T>
class OrderedRelation
{
    private:
        std::vector<T> elements;

    public:
        OrderedRelation();
        OrderedRelation(const OrderedRelation<T>& _orderedRelation);
        OrderedRelation(const std::vector<T>& _elements);

        void add(const T& element);
        void keepOnlyNFirstElements(std::size_t n);
        OrderedRelation<T> getThisWithOnlyNFirstElements(std::size_t n) const;

        std::size_t size() const;
        const std::vector<T>& getElements() const;

        OrderedRelation<T> operator=(const OrderedRelation<T>& other);
        bool operator==(const OrderedRelation<T>& other) const;
        bool operator!=(const OrderedRelation<T>& other) const;
        bool operator<(const OrderedRelation<T>& other) const;
        bool operator<=(const OrderedRelation<T>& other) const;
        bool operator<<(const OrderedRelation<T>& other) const; // Has the same precedence (Ex : 4 and 4 - 3 have the same beginning, 4 and 3 - 4 not)
};

template <typename T>
OrderedRelation<T>::OrderedRelation() :
    elements()
{
}

template <typename T>
OrderedRelation<T>::OrderedRelation(const OrderedRelation<T>& _orderedRelation) :
    elements(_orderedRelation.getElements())
{
}

template <typename T>
OrderedRelation<T>::OrderedRelation(const std::vector<T>& _elements) :
    elements(_elements)
{
}

template <typename T>
void OrderedRelation<T>::add(const T& element)
{
    this->elements.push_back(element);
}

template <typename T>
void OrderedRelation<T>::keepOnlyNFirstElements(std::size_t n)
{
    std::size_t i;
    std::size_t size = this->size();

    if (size >= n)
    {
        for (i = n; i < size; ++i)
        {
            this->elements.pop_back();
        }
    }
}

template <typename T>
OrderedRelation<T> OrderedRelation<T>::getThisWithOnlyNFirstElements(std::size_t n) const
{
    OrderedRelation<T> result = *this;

    result.keepOnlyNFirstElements(n);

    return result;
}

template <typename T>
std::size_t OrderedRelation<T>::size() const
{
    return this->elements.size();
}

template <typename T>
const std::vector<T>& OrderedRelation<T>::getElements() const
{
    return this->elements;
}

template <typename T>
OrderedRelation<T> OrderedRelation<T>::operator=(const OrderedRelation<T>& other)
{
    std::size_t otherElementsSize = other.elements.size();

    if (otherElementsSize > this->elements.size())
    {
        this->elements.resize(otherElementsSize);
    }

    this->elements = other.elements;

    return *this;
}

template <typename T>
bool OrderedRelation<T>::operator==(const OrderedRelation<T>& other) const
{
    bool result = true;
    std::size_t size = this->size();
    std::size_t i = 0;

    if (size != other.size())
    {
        result = false;
    }

    if (result == true)
    {
        while ((result == true) && (i < size))
        {
            if (this->elements.at(i) != other.elements.at(i))
            {
                result = false;
            }

            ++i;
        }
    }

    return result;
}

template <typename T>
bool OrderedRelation<T>::operator!=(const OrderedRelation<T>& other) const
{
    return !(*this == other);
}

template <typename T>
bool OrderedRelation<T>::operator<(const OrderedRelation<T>& other) const
{
    bool result = true;
    std::size_t i = 0;
    std::size_t thisSize = this->size();
    std::size_t otherSize = other.size();

    if (thisSize > otherSize)
    {
        result = false;
    }
    else if (thisSize == otherSize)
    {
        while ((result == true) && (i < thisSize))
        {
            if (this->elements.at(i) == other.elements.at(i))
            {
                ++i;
            }
            else
            {
                result = false;
            }
        }

        if (i == thisSize) // All elements of *this and other are same, and in the same order
        {
            result = false;
        }
        else
        {
            result = (this->elements.at(i) < other.elements.at(i));
        }
    }

    return result;
}

template <typename T>
bool OrderedRelation<T>::operator<=(const OrderedRelation<T>& other) const
{
    return ((*this < other) || (*this == other));
}

template <typename T>
bool OrderedRelation<T>::operator<<(const OrderedRelation<T>& other) const
{
    bool result = true;
    std::size_t i = 0;
    std::size_t size = helpers::math::min(this->size(), other.size());

    if (*this != other)
    {
        while ((result == true) && (i < size))
        {
            if (this->elements.at(i) != other.elements.at(i))
            {
                result = false;
            }
            else
            {
                ++i;
            }
        }
    }

    return result;
}

} // namespace helpers

#endif
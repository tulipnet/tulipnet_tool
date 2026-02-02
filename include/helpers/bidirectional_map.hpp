#ifndef BIDIRECTIONAL_MAP_HPP
#define BIDIRECTIONAL_MAP_HPP

#include <cstddef>
#include <vector>
#include <utility>
#include <exception>

namespace helpers
{

/**
 * @brief Thrown when trying to look up an non-existing element in a BidirectionalMap.
 * 
 */
class BidirectionalMapElementNotFoundException : public std::exception
{
    public:
        virtual const char* what() const throw();
};

/**
 * @brief A map that can be requested by both the key and the value.
 *
 * Because of the bidirectional character of a such map, this class is abstract, and an adding constraint has to be written for an implementation.
 * When looking for an object of type A or B, the first matching occurrence is returned.
 * 
 * @tparam A The left key type
 * @tparam B The right key type
 */
template <typename A, typename B>
class BidirectionalMap
{
    private:
        std::vector<std::pair<A, B>> map;

    public:
        /**
         * @brief Construct a new Bidirectional Map object
         * 
         */
        BidirectionalMap();

        /**
         * @brief Add an element to the map.
         * 
         * @param newElement Pair containing the new tuple to insert.
         * @return true All is done.
         * @return false Something went wrong. The reason is the `newElement` already exists in the pair regarding the adding constraint.
         */
        bool add(std::pair<A, B> newElement);

        /**
         * @brief Get the size of the map.
         * 
         * @return std::size_t Size of the map.
         */
        std::size_t size() const;

        /**
         * @brief Test the existence of an element in the left member of the map.
         * 
         * @param element Element to test the existence in the left member.
         * @return true The element exits.
         * @return false The element does not exit.
         */
        bool existsLeft(A element) const;

        /**
         * @brief Test the existence of an element in the right member of the map.
         * 
         * @param element Element to test the existence in the right member.
         * @return true The element exits.
         * @return false The element does not exit.
         */
        bool existsRight(B element) const;

        /**
         * @brief Test if the adding constraint is true when tested on a specific pair.
         * 
         * @param pair The pair on which to test the adding constraint.
         * @return true The adding constraint answers true, and it is not possible to insert this pair in the map.
         * @return false The adding constraint answers false, and it is possible to insert this pair in the map.
         */
        bool addConstraintHolds(std::pair<A, B> pair) const;

        /**
         * @brief Get a constant pointer on a right element with a left element reference.
         * 
         * @param leftElement The left element to use to query the map.
         * @return const B* The result.
         *
         * @throws BidirectionalMapElementNotFoundException If the queried element does not exit.
         */
        const B* getPtrFromLeft(const A& leftElement) const;

        /**
         * @brief Get a constant pointer on a left element with a right element reference.
         * 
         * @param rightElement The right element to use to query the map.
         * @return const A* The result.
         *
         * @throws BidirectionalMapElementNotFoundException If the queried element does not exit.
         */
        const A* getPtrFromRight(const B& rightElement) const;

        /**
         * @brief Get a constant reference on a right element with a left element reference.
         * 
         * @param leftElement The left element to use to query the map.
         * @return const B& The result.
         *
         * @throws BidirectionalMapElementNotFoundException If the queried element does not exit.
         */
        const B& getFromLeft(const A& leftElement) const;

        /**
         * @brief Get a constant reference on a left element with a right element reference.
         * 
         * @param rightElement The right element to use to query the map.
         * @return const A* The result.
         *
         * @throws BidirectionalMapElementNotFoundException If the queried element does not exit.
         */
        const A& getFromRight(const B& rightElement) const;

        /**
         * @brief Get constant pointers on all the left elements in a new vector.
         * 
         * @return std::vector<const A*> A new vector containing constant pointers on all the left elements.
         */
        std::vector<const A*> getPtrLeftElements() const;

        /**
         * @brief Get constant pointers on all the right elements in a new vector.
         * 
         * @return std::vector<const B*> A new vector containing constant pointers on all the right elements.
         */
        std::vector<const B*> getPtrRightElements() const;

        /**
         * @brief Get a new vector containing copies of all the left elements.
         * 
         * @return std::vector<A> The new vector containing copies of all the left elements.
         */
        std::vector<A> getLeftElements() const;

        /**
         * @brief Get a new vector containing copies of all the right elements.
         * 
         * @return std::vector<B> The new vector containing copies of all the right elements.
         */
        std::vector<B> getRightElements() const;

        /**
         * @brief Adding constraint to **implement** before using a BidirectionalMap.
         * 
         * @param pairA \f$ A \f$
         * @param pairB \f$ B \f$
         * @return true \f$ A = B \f$ regarding this adding constraint
         * @return false \f$ A \ne B \f$ regarding this adding constraint
         */
        virtual bool addConstraint(std::pair<A, B> pairA, std::pair<A, B> pairB) const = 0;
};

#pragma region Methods definitions

template <typename A, typename B>
BidirectionalMap<A, B>::BidirectionalMap()
{
}

template <typename A, typename B>
bool BidirectionalMap<A, B>::add(std::pair<A, B> newElement)
{
    bool result = false;

    if (this->addConstraintHolds(newElement) == false)
    {
        this->map.push_back(newElement);

        result = true;
    }

    return result;
}

template <typename A, typename B>
std::size_t BidirectionalMap<A, B>::size() const
{
    return this->map.size();
}

template <typename A, typename B>
bool BidirectionalMap<A, B>::existsLeft(A element) const
{
    bool result = false;
    std::size_t i = 0;

    while ((result == false) && (i < this->map.size()))
    {
        const std::pair<A, B>& currentElement = this->map.at(i);

        if (currentElement.first == element)
        {
            result = true;
        }
    }

    return result;
}

template <typename A, typename B>
bool BidirectionalMap<A, B>::existsRight(B element) const
{
    bool result = false;
    std::size_t i = 0;

    while ((result == false) && (i < this->map.size()))
    {
        const std::pair<A, B>& currentElement = this->map.at(i);

        if (currentElement.second == element)
        {
            result = true;
        }
    }

    return result;
}

template <typename A, typename B>
const B* BidirectionalMap<A, B>::getPtrFromLeft(const A& leftElement) const
{
    bool found = false;
    std::size_t i = 0;
    const B* result = NULL;

    while ((found == false) && (i < this->map.size()))
    {
        const std::pair<A, B>& currentElement = this->map.at(i);

        if (currentElement.first == leftElement)
        {
            result = &currentElement.second;
            found = true;
        }
        else
        {
            ++i;
        }
    }

    if (found == false)
    {
        throw BidirectionalMapElementNotFoundException();
    }

    return result;
}

template<typename A, typename B>
const A* BidirectionalMap<A, B>::getPtrFromRight(const B& rightElement) const
{
    bool found = false;
    std::size_t i = 0;
    const A* result = NULL;

    while ((found == false) && (i < this->map.size()))
    {
        const std::pair<A, B>& currentElement = this->map.at(i);

        if (currentElement.second == rightElement)
        {
            result = &currentElement.first;
            found = true;
        }
        else
        {
            ++i;
        }
    }

    if (found == false)
    {
        throw BidirectionalMapElementNotFoundException();
    }

    return result;
}

template <typename A, typename B>
const B& BidirectionalMap<A, B>::getFromLeft(const A& leftElement) const
{
    return *this->getPtrFromLeft(leftElement);
}

template <typename A, typename B>
const A& BidirectionalMap<A, B>::getFromRight(const B& rightElement) const
{
    return *this->getPtrFromRight(rightElement);
}

template <typename A, typename B>
bool BidirectionalMap<A, B>::addConstraintHolds(std::pair<A, B> pair) const
{
    bool found = false;
    std::size_t i = 0;

    while ((found == false) && (i < this->map.size()))
    {
        const std::pair<A, B>& currentElement = this->map.at(i);

        if (this->addConstraint(pair, currentElement) == true)
        {
            found = true;
        }
        else
        {
            ++i;
        }
    }

    return found;
}

template <typename A, typename B>
std::vector<const A*> BidirectionalMap<A, B>::getPtrLeftElements() const
{
    std::vector<const A*> result;

    for (const std::pair<A, B>& currentPair : this->map)
    {
        result.push_back(&currentPair.first);
    }

    return result;
}

template <typename A, typename B>
std::vector<const B*> BidirectionalMap<A, B>::getPtrRightElements() const
{
    std::vector<const B*> result;

    for (const std::pair<A, B>& currentPair : this->map)
    {
        result.push_back(&currentPair.second);
    }

    return result;
}

template <typename A, typename B>
std::vector<A> BidirectionalMap<A, B>::getLeftElements() const
{
    std::vector<A> result;

    for (const std::pair<A, B>& currentPair : this->map)
    {
        result.push_back(currentPair.first);
    }

    return result;
}

template <typename A, typename B>
std::vector<B> BidirectionalMap<A, B>::getRightElements() const
{
    std::vector<B> result;

    for (const std::pair<A, B>& currentPair : this->map)
    {
        result.push_back(currentPair.second);
    }

    return result;
}

#pragma endregion

} // namespace helpers

#endif
#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <algorithm>
#include <cstddef>
#include <functional>
#include <vector>

namespace helpers::vector
{

template <typename TSource, typename TDestination>
std::vector<TDestination> cast(const std::vector<TSource>& sourceVector)
{
    std::vector<TDestination> result(sourceVector.size());

    std::transform(sourceVector.begin(), sourceVector.end(), result.begin(), [](const TSource& sourceElement){ return (TDestination)sourceElement; });

    return result;
}

template <typename T>
bool contains(const std::vector<T>& vector, const T& value)
{
    bool result = false;
    std::size_t i = 0;
    std::size_t vectorSize = vector.size();

    while ((i < vectorSize) && (result == false))
    {
        result = (vector.at(i) == value);

        ++i;
    }

    return result;
}

/* Takes a vector in input, and returns a vector containing a subpart of the input.
Example : { 1, 2, 3, 4, 5, 6 }, numberOfChunks = 3, chunkID = 1 => { 3, 4 } */
template <typename T>
std::vector<T> getChunk(const std::vector<T>& input, std::size_t numberOfChunks, std::size_t chunkID)
{
    std::vector<T> result;
    std::size_t inputSize = input.size();
    std::size_t numberOfItemsInAChunk = inputSize / numberOfChunks;
    std::size_t i;
    std::size_t end;

    if (chunkID == numberOfChunks - 1)
    {
        end = inputSize;
    }
    else
    {
        end = (chunkID + 1) * numberOfItemsInAChunk;
    }

    for (i = chunkID * numberOfItemsInAChunk; i < end; ++i)
    {
        result.push_back(input.at(i));
    }

    return result;
}

template <typename T>
bool match(const std::vector<T>& vector, const std::function<bool(const T&)>& predicate)
{
    bool result = false;
    std::size_t i = 0;
    std::size_t sizeOfVector = vector.size();

    while ((result == false) && (i < sizeOfVector))
    {
        if (predicate(vector.at(i)) == true)
        {
            result = true;
        }
        else
        {
            ++i;
        }
    }

    return result;
}

} // namespace helpers::vector

#endif
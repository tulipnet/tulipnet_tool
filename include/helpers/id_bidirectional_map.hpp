#ifndef ID_BIDIRECTIONAL_MAP
#define ID_BIDIRECTIONAL_MAP

#include "helpers/bidirectional_map.hpp"

namespace helpers
{

/**
 * @brief Bidirectional map based on an ID. Both the ID and the values are uniques.
 * 
 * @tparam Type of the values.
 */
template <typename T>
class IdBidirectionalMap : public BidirectionalMap<int, T>
{
    public:
        /**
         * @brief Construct a new IdBidirectionalMap object.
         * 
         */
        IdBidirectionalMap();

        /**
         * @brief Adding constraint for this kind of BidirectionalMap, where the ID should be unique
         * 
         * @param pairA \f$ A \f$
         * @param pairB \f$ B \f$
         * @return true \f$ A = B \f$ regarding this adding constraint
         * @return false \f$ A \ne B \f$ regarding this adding constraint
         */
        virtual bool addConstraint(std::pair<int, T> pairA, std::pair<int, T> pairB) const;
};

template <typename T>
IdBidirectionalMap<T>::IdBidirectionalMap() :
    BidirectionalMap<int, T>()
{
}

template <typename T>
bool IdBidirectionalMap<T>::addConstraint(std::pair<int, T> pairA, std::pair<int, T> pairB) const
{
    return (pairA.first == pairB.first);
}

}

#endif
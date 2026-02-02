#ifndef MAP_HPP
#define MAP_HPP

#include <map>
#include <set>
#include <utility>

namespace helpers::map
{

/**
 * @brief Tests if a map contains a binding with a specific key
 * 
 * @tparam K Key type
 * @tparam V Value type
 * @param map The map
 * @param key The key to test
 * @return true The map contains a binding with the tested key
 * @return false The map does not contain a binding with the tested key
 */
template <typename K, typename V>
bool constainsKey(const std::map<K, V>& map, const K& key)
{
    bool result;

    if (map.find(key) == map.end())
    {
        result = false;
    }
    else
    {
        result = true;
    }

    return result;
}

/**
 * @brief Get a set of all keys of the provided map
 * 
 * @tparam K Key type
 * @tparam V Value type
 * @param map The map
 * @return std::set<K> A set containing all keys of the map
 */
template <typename K, typename V>
std::set<K> getAllKeys(const std::map<K, V>& map)
{
    std::set<K> result;

    for (const std::pair<const K, V>& currentPair : map)
    {
        result.insert(currentPair.first);
    }

    return result;
}

} // namespace helpers::map

#endif
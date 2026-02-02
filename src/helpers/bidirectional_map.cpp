#include "helpers/bidirectional_map.hpp"

namespace helpers
{

#pragma region BidirectionalMapElementNotFoundException

const char* BidirectionalMapElementNotFoundException::what() const throw()
{
    return "ERROR : An inexistent element access attempt was made";
}

} // namespace helpers

#pragma endregion
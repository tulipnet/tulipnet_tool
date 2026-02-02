#ifndef TUPLE_HPP
#define TUPLE_HPP

namespace helpers
{

namespace tuples
{

template <typename... Types>
class Tuple;

template <>
class Tuple<>
{
    public:
        static constexpr int position = 0;
};

template <typename TailType>
class Tuple<TailType> : private Tuple<>
{
    private:
        TailType value;

    public:
        static constexpr int position = 1;

        Tuple(TailType _value) :
            Tuple<>(),
            value(_value)
        {
        }

        const TailType& get() const
        {
            return this->value;
        }

        const Tuple<>& tail() const
        {
            return (Tuple<>&)*this;
        }
};

template <typename HeadType, typename... TailTypes>
class Tuple<HeadType, TailTypes...> : private Tuple<TailTypes...>
{
    private:
        HeadType value;

    public:
        static constexpr int position = 1 + sizeof...(TailTypes);

        Tuple(HeadType _value, TailTypes... _otherValues) :
            Tuple<TailTypes...>(_otherValues...),
            value(_value)
        {
        }

        const HeadType& get() const
        {
            return this->value;
        }

        const Tuple<TailTypes...>& tail() const
        {
            return (Tuple<TailTypes...>&)*this;
        }
};

template <int i, typename HeadType, typename... TailTypes>
struct NthType : NthType<i - 1, TailTypes...>
{
    static_assert(i <= sizeof...(TailTypes) + 1, "Index out of bounds");
};

template <typename HeadType, typename... TailTypes>
struct NthType<0, HeadType, TailTypes...>
{
    HeadType value;
};

template <int i>
struct TupleGetter;

template <>
struct TupleGetter<0>
{
    template <typename HeadType, typename... TailTypes>
    static const HeadType& get(const Tuple<HeadType, TailTypes...>& tuple)
    {
        return tuple.get();
    }
};

template <int i>
struct TupleGetter
{
    template <typename... Types>
    static const decltype(NthType<i, Types...>::value)& get(const Tuple<Types...>& tuple)
    {
        return TupleGetter<i - 1>::get(tuple.tail());
    }
};

template <int i, typename... Types>
const decltype(NthType<i, Types...>::value)& get(const Tuple<Types...>& tuple)
{
    return TupleGetter<i>::get(tuple);
};

} // namespace tuples

} // namespace helpers

#endif
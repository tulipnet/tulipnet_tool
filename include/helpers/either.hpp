#ifndef EITHER_HPP
#define EITHER_HPP

#include <cstddef>
#include <cstring>
#include <exception>
#include <optional>
#include <string>
#include <typeinfo>

namespace helpers
{

template<typename T>
class EitherBadTypeCastException : public std::exception
{
    private:
        std::string message;

    public:
        virtual const char* what() throw();
};

/**
 * @brief Union-based type containing either an element of type A, either an element of type B.
 * 
 * @tparam A The type A
 * @tparam B The type B
 */
template <typename A, typename B>
class Either
{
    private:
        union Content
        {
            A* a;
            B* b;
        };

        enum class ValueTypeDiscriminant
        {
            TYPE_A,
            TYPE_B,
            NEITHER
        };

        union Content value;
        ValueTypeDiscriminant discriminant;

    public:
        /**
         * @brief Construct a new empty Either object that have no type (Neither A nor B).
         * 
         */
        Either();

        /**
         * @brief Construct a new Either object that embeds a constant reference on an object of type A.
         * 
         * @param _value Constant reference on an object of type A to embed inside the new Either object.
         */
        Either(const A& _value);

        /**
         * @brief Construct a new Either object that embeds a constant reference on an object of type B.
         * 
         * @param _value Constant reference on an object of type B to embed inside the new Either object.
         */
        Either(const B& _value);

        /**
         * @brief Get a constant pointer of type T* on the contained object within this.
         * 
         * @tparam T The type of the object pointed by the requested pointer.
         * @return std::optional<const T*> If the type T is the same type as the type used to construct this, a constant pointer on the object referenced by the reference embedded in this. If not, `std::nullopt`.
         */
        template <typename T> std::optional<const T*> getPtr() const;

        /**
         * @brief Tests if this embeds an reference on an object of type T
         * 
         * @tparam T The type to test.
         * @return true This Either embeds a reference to an object of type T.
         * @return false This Either does not embeds a reference to an object of type T.
         */
        template <typename T> bool is() const;

        /**
         * @brief Get a copy of the object of type T contained in this Either.
         * 
         * @tparam T The type of the requested object.
         * @return std::optional<T> If the type of the embedded object is T, a copy of it. Otherwise, `std::nullopt`.
         */
        template <typename T> std::optional<T> get() const;

        /**
         * @brief Cast operator.
         * 
         * @tparam T Type on which to cast this.
         * @return T Result of the cast.
         *
         * @throws EitherBadTypeCastException<T> If the type of the embedded object is not T.
         */
        template <typename T> operator T() const; // Cast to T operator

        ~Either();
};

#pragma region EitherBadTypeCastException 

template <typename T>
const char* EitherBadTypeCastException<T>::what() throw()
{
    std::string typeNameOfT = typeid(T).name();

    this->message = "ERROR : There is no value of type '" + typeNameOfT + "' in the either value";

    return this->message.c_str();
}

#pragma endregion

template <typename A, typename B>
Either<A, B>::Either()
{
    this->discriminant = ValueTypeDiscriminant::NEITHER;
}

template <typename A, typename B>
Either<A, B>::Either(const A& _value)
{
    this->value.a = new A(_value);
    this->discriminant = ValueTypeDiscriminant::TYPE_A;
}

template <typename A, typename B>
Either<A, B>::Either(const B& _value)
{
    this->value.b = new B(_value);
    this->discriminant = ValueTypeDiscriminant::TYPE_B;
}

template <typename A, typename B>
template <typename T>
bool Either<A, B>::is() const
{
    std::size_t typeHashOfT = typeid(T).hash_code();
    std::size_t typeHashOfA = typeid(A).hash_code();
    std::size_t typeHashOfB = typeid(B).hash_code();
    bool result;

    if (((this->discriminant == ValueTypeDiscriminant::TYPE_A) && (typeHashOfT == typeHashOfA)) || ((this->discriminant == ValueTypeDiscriminant::TYPE_B) && (typeHashOfT == typeHashOfB)))
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}

template <typename A, typename B>
template <typename T>
std::optional<const T*> Either<A, B>::getPtr() const
{
    std::optional<const T*> result;

    if (this->is<A>() == true)
    {
        result = (const T*)this->value.a;
    }
    else if (this->is<B>() == true)
    {
        result = (const T*)this->value.b;
    }
    else
    {
        result = std::nullopt;
    }

    return result;
}

template <typename A, typename B>
template <typename T>
std::optional<T> Either<A, B>::get() const
{
    std::optional<const T*> ptr = this->getPtr<T>();
    std::optional<T> result;

    if (ptr != std::nullopt)
    {
        result = *ptr.value();
    }
    else
    {
        result = std::nullopt;
    }

    return result;
}

template <typename A, typename B>
template <typename T>
Either<A, B>::operator T() const
{
    std::optional<T> value = this->get<T>();

    if (value == std::nullopt)
    {
        throw EitherBadTypeCastException<T>();
    }

    return value.value();
}

template <typename A, typename B>
Either<A, B>::~Either()
{
    if (this->discriminant == ValueTypeDiscriminant::TYPE_A)
    {
        delete this->value.a;
    }
    else if (this->discriminant == ValueTypeDiscriminant::TYPE_B)
    {
        delete this->value.b;
    }
}

} // namespace helpers

#endif
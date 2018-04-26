#ifndef VARNUMBER_H
#define VARNUMBER_H

#include <exception>
#include "mapbox/variant.hpp"


using VarNum = mapbox::util::variant<uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, float, double>;


class AnyNumber
{

public:

    AnyNumber() {}

    ~AnyNumber() = default;

    template<typename T> AnyNumber(const T& value) : _var(value) { }

    template<typename T> T convert( ) const;

    template<typename T> T extract( ) const
    {
        return _var.get<T>();
    }

    template <typename T> void assign(const T& value);

private:

    VarNum _var;
};

//----------------------------------------------
namespace details{

template <typename T>
using Invoke = typename T::type;


template <typename BoolCondition>
using EnableIf = Invoke<std::enable_if<BoolCondition::value> >;


template <typename T>
struct is_integer : std::integral_constant<bool, std::is_integral<T>::value
        && !std::is_same<T, bool>::value
        && !std::is_same<T, char>::value>
{};

template <typename From, typename To>
struct is_same_real : std::integral_constant<bool,
        std::is_same<From, To>::value
        && std::is_floating_point<To>::value >
{};


template <typename From, typename To>
struct is_safe_integer_conversion
        : std::integral_constant<bool, is_integer<From>::value
        && is_integer<To>::value
        && sizeof(From) <= sizeof(To)
&& std::is_signed<From>::value == std::is_signed<To>::value>
{};

template <typename From, typename To>
struct float_conversion
        : std::integral_constant<bool, std::is_floating_point<From>::value
        && std::is_floating_point<To>::value && !std::is_same<From, To>::value >
{};

template <typename From, typename To>
struct unsigned_to_smaller_conversion
        : std::integral_constant<bool, is_integer<From>::value
        && is_integer<To>::value
        && (sizeof(From) > sizeof(To))
&& !std::is_signed<From>::value
&& !std::is_signed<To>::value >
{};

template <typename From, typename To>
struct signed_to_smaller_conversion
        : std::integral_constant<bool, is_integer<From>::value
        && is_integer<To>::value
        && (sizeof(From) > sizeof(To))
&& std::is_signed<From>::value
&& std::is_signed<To>::value >
{};

//---------------------------
template <typename From, typename To>
struct signed_to_smaller_unsigned_conversion
        : std::integral_constant<bool, is_integer<From>::value
        && is_integer<To>::value
        && sizeof(From) >= sizeof(To)
&& std::is_signed<From>::value
&& !std::is_signed<To>::value >
{};

template <typename From, typename To>
struct signed_to_larger_unsigned_conversion
        : std::integral_constant<bool, is_integer<From>::value
        && is_integer<To>::value
        && sizeof(From) < sizeof(To)
&& std::is_signed<From>::value
&& !std::is_signed<To>::value >
{};

template <typename From, typename To>
struct unsigned_to_smaller_signed_conversion
        : std::integral_constant<bool, is_integer<From>::value
        && is_integer<To>::value
        && (sizeof(From) >= sizeof(To))
&& !std::is_signed<From>::value
&& std::is_signed<To>::value >
{};

template <typename From, typename To>
struct unsigned_to_larger_signed_conversion
        : std::integral_constant<bool, is_integer<From>::value
        && is_integer<To>::value
        && sizeof(From) < sizeof(To)
&& !std::is_signed<From>::value
&& std::is_signed<To>::value >
{};

template <typename From, typename To>
struct floating_to_signed_conversion
        : std::integral_constant<bool, std::is_floating_point<From>::value
        && is_integer<To>::value
        && std::is_signed<To>::value >
{};

template <typename From, typename To>
struct floating_to_unsigned_conversion
        : std::integral_constant<bool, std::is_floating_point<From>::value
        && is_integer<To>::value
        && !std::is_signed<To>::value >
{};

template <typename From, typename To>
struct integer_to_floating_conversion
        : std::integral_constant<bool, is_integer<From>::value
        && std::is_floating_point<To>::value >
{};

template <typename From, typename To>
inline void checkUpperLimit(const From& from)
{
    if ((sizeof(To) < sizeof(From)) &&
            (from > static_cast<From>(std::numeric_limits<To>::max()))) {
        throw std::runtime_error("Value too large.");
    }
    else if (static_cast<To>(from) > std::numeric_limits<To>::max()) {
        throw std::runtime_error("Value too large.");
    }
}

template <typename From, typename To>
inline void checkUpperLimitFloat(const From& from)
{
    if (from > std::numeric_limits<To>::max()){
        throw std::runtime_error("Value too large.");
    }
}

template <typename From, typename To>
inline void checkLowerLimitFloat(const From& from)
{
    if (from < -std::numeric_limits<To>::max()){
        throw std::runtime_error("Value too small.");
    }
}

template <typename From, typename To>
inline void checkLowerLimit(const From& from)
{
    if (from < std::numeric_limits<To>::min()){
        throw std::runtime_error("Value too small.");
    }
}

template <typename From, typename To>
inline void checkTruncation(const From& from)
{
    if( from != static_cast<From>(static_cast<To>( from))){
        throw std::runtime_error("Floating point truncated");
    }
}


//----------------------- Implementation ----------------------------------------------

template<typename SRC,typename DST,
         typename details::EnableIf< details::is_same_real<SRC, DST>>* = nullptr >
inline void convert_impl( const SRC& from, DST& target )
{
    target = from;
}


template<typename SRC,typename DST,
         typename details::EnableIf< details::is_safe_integer_conversion<SRC, DST>>* = nullptr >
inline void convert_impl( const SRC& from, DST& target )
{
    //std::cout << "is_safe_integer_conversion" << std::endl;
    target = static_cast<DST>( from);
}

template<typename SRC,typename DST,
         typename details::EnableIf< details::float_conversion<SRC, DST>>* = nullptr >
inline void convert_impl( const SRC& from, DST& target )
{
    checkTruncation<SRC,DST>(from);
    target = static_cast<DST>( from );
}


template<typename SRC,typename DST,
         typename details::EnableIf< details::unsigned_to_smaller_conversion<SRC, DST>>* = nullptr  >
inline void convert_impl( const SRC& from, DST& target )
{
    checkUpperLimit<SRC,DST>(from);
    target = static_cast<DST>( from);
}

template<typename SRC,typename DST,
         typename details::EnableIf< details::signed_to_smaller_conversion<SRC, DST>>* = nullptr  >
inline void convert_impl( const SRC& from, DST& target )
{
    checkLowerLimit<SRC,DST>(from);
    checkUpperLimit<SRC,DST>(from);
    target = static_cast<DST>( from);
}


template<typename SRC,typename DST,
         typename details::EnableIf< details::signed_to_smaller_unsigned_conversion<SRC, DST>>* = nullptr  >
inline void convert_impl( const SRC& from, DST& target )
{
    if (from < 0 )
        throw std::runtime_error("Value is negative and can't be converted to signed");

    checkUpperLimit<SRC,DST>(from);
    target = static_cast<DST>( from );
}


template<typename SRC,typename DST,
         typename details::EnableIf< details::signed_to_larger_unsigned_conversion<SRC, DST>>* = nullptr   >
inline void convert_impl( const SRC& from, DST& target )
{
    //std::cout << "signed_to_larger_unsigned_conversion" << std::endl;

    if ( from < 0 )
        throw std::runtime_error("Value is negative and can't be converted to signed");

    target = static_cast<DST>( from);
}

template<typename SRC,typename DST,
         typename details::EnableIf< details::unsigned_to_larger_signed_conversion<SRC, DST>>* = nullptr   >
inline void convert_impl( const SRC& from, DST& target )
{
    //std::cout << "unsigned_to_larger_signed_conversion" << std::endl;
    target = static_cast<DST>( from);
}

template<typename SRC,typename DST,
         typename details::EnableIf< details::unsigned_to_smaller_signed_conversion<SRC, DST>>* = nullptr   >
inline void convert_impl( const SRC& from, DST& target )
{
    //std::cout << "unsigned_to_smaller_signed_conversion" << std::endl;

    checkUpperLimit<SRC,DST>(from);
    target = static_cast<DST>( from);
}

template<typename SRC,typename DST,
         typename details::EnableIf< details::floating_to_signed_conversion<SRC, DST>>* = nullptr   >
inline void convert_impl( const SRC& from, DST& target )
{
    //std::cout << "floating_to_signed_conversion" << std::endl;

    checkLowerLimitFloat<SRC,DST>(from);
    checkLowerLimitFloat<SRC,DST>(from);

    if( from != static_cast<SRC>(static_cast<DST>( from)))
        throw std::runtime_error("Floating point truncated");

    target = static_cast<DST>( from);
}

template<typename SRC,typename DST,
         typename details::EnableIf< details::floating_to_unsigned_conversion<SRC, DST>>* = nullptr   >
inline void convert_impl( const SRC& from, DST& target )
{
    //std::cout << "floating_to_unsigned_conversion" << std::endl;
    if ( from < 0 )
        throw std::runtime_error("Value is negative and can't be converted to signed");

    checkLowerLimitFloat<SRC,DST>(from);

    if( from != static_cast<SRC>(static_cast<DST>( from)))
        throw std::runtime_error("Floating point truncated");

    target = static_cast<DST>( from);
}

template<typename SRC,typename DST,
         typename details::EnableIf< details::integer_to_floating_conversion<SRC, DST>>* = nullptr >
inline void convert_impl( const SRC& from, DST& target )
{
    //std::cout << "floating_to_unsigned_conversion" << std::endl;

    checkTruncation<SRC,DST>(from);
    target = static_cast<DST>( from);
}

} //end namespace details


template <typename DST> struct Visitor
{
    template <typename SRC>
    void operator()(const SRC& from, DST& target) const
    {
        details::convert_impl<SRC,DST>( from, target);
    }
};

template<typename DST> inline
DST AnyNumber::convert() const
{
    using namespace details;
    DST out;

    _var.match([&] (uint8_t src) { convert_impl<uint8_t,DST>(src, out); },
    [&] (uint16_t src) { convert_impl<uint16_t,DST>(src, out); },
    [&] (uint32_t src) { convert_impl<uint32_t,DST>(src, out); },
    [&] (uint64_t src) { convert_impl<uint64_t,DST>(src, out); },
    [&] (int8_t src) { convert_impl<int8_t,DST>(src, out); },
    [&] (int16_t src) { convert_impl<int16_t,DST>(src, out); },
    [&] (int32_t src) { convert_impl<int32_t,DST>(src, out); },
    [&] (int64_t src) { convert_impl<int64_t,DST>(src, out); },
    [&] (float src) { convert_impl<float,DST>(src, out); },
    [&] (double src) { convert_impl<double,DST>(src, out); } );

    return out;
}


#endif // VARNUMBER_H

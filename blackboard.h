#ifndef BLACKBOARD_H
#define BLACKBOARD_H

#include <iostream>
#include <string>
#include <memory>
#include <stdint.h>
#include <unordered_map>
#include "nonstd/any.hpp"
#include "varnumber.h"

class BlackboardImpl
{
public:

    virtual ~BlackboardImpl() = default;

    virtual bool get(const std::string& key, nonstd::any& value) const = 0;
    virtual void set(const std::string& key, const nonstd::any& value) = 0;

    virtual bool get(const std::string& key, AnyNumber& value) const = 0;
    virtual void set(const std::string& key, const AnyNumber& value) = 0;

    virtual bool get(const std::string& key, std::string& value) const = 0;
    virtual void set(const std::string& key, const std::string& value) = 0;
};


// Blackboard is the front-end used by the developer.
// Even if the abstract class BlackboardImpl can be used directly,
// the templatized methods set() and get() are more
class Blackboard
{
public:

    Blackboard( std::unique_ptr<BlackboardImpl> implementation):
        impl_( std::move(implementation ) )
    { }

    virtual ~Blackboard() = default;

    template <typename T> bool get(const std::string& key, T& value) const
    {
        return getImpl(key, value);
    }

    template <typename T> void set(const std::string& key, const T& value) {
        setImpl(key, value);
    }

private:


    // This implementation is triggered by a integral number or a floating point
    template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, T>::type* = nullptr>
    void setImpl(const std::string& key,const T& value)
    {
        impl_->set(key, AnyNumber(value));
    }

    // Not a number, nor a std::string, nor a const char*
    template <typename T, typename std::enable_if<!std::is_arithmetic<T>::value, T>::type* = nullptr>
    void setImpl(const std::string& key,const T& value)
    {
        impl_->set(key, nonstd::any(value));
    }

    void setImpl(const std::string& key,const char* value)
    {
        impl_->set(key, std::string(value));
    }

    void setImpl(const std::string& key,const std::string& value)
    {
        impl_->set(key, value);
    }

    // This implementation is triggered by a integral number or a floating point
    template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, T>::type* = nullptr>
    bool getImpl(const std::string& key, T& value) const
    {
        AnyNumber num;
        bool found = impl_->get(key, num);
        if( !found ){ return false; }
        value = num.convert<T>();
        return true;
    }

    // Not a number, nor a std::string, nor a const char*
    template <typename T, typename std::enable_if<!std::is_arithmetic<T>::value, T>::type* = nullptr>
    bool getImpl(const std::string& key, T& value) const
    {
        nonstd::any res;
        bool found = impl_->get(key, res);
        if( !found ){ return false; }
        value = nonstd::any_cast<T>(res);
        return true;
    }

    bool getImpl(const std::string& key, std::string& value) const
    {
        return impl_->get(key, value);
    }

    std::unique_ptr<BlackboardImpl> impl_;
};


#endif // BLACKBOARD_H

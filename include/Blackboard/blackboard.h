#ifndef BLACKBOARD_H
#define BLACKBOARD_H

#include <iostream>
#include <string>
#include <memory>
#include <stdint.h>
#include <unordered_map>

#include <SafeAny/safe_any.hpp>


class BlackboardImpl
{
public:

    virtual ~BlackboardImpl() = default;

    virtual const SafeAny::Any* get(const std::string& key) const = 0;
    virtual void set(const std::string& key, const SafeAny::Any& value) = 0;
};


// Blackboard is the front-end to be used by the developer.
// Even if the abstract class BlackboardImpl can be used directly,
// the templatized methods set() and get() are more user-friendly
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

    // Not a number, nor a std::string, nor a const char*
    template <typename T>
    void setImpl(const std::string& key,const T& value)
    {
        impl_->set(key, SafeAny::Any(value));
    }

    void setImpl(const std::string& key,const char* value)
    {
        impl_->set(key, std::string(value));
    }

    template <typename T>
    bool getImpl(const std::string& key, T& value) const
    {
        const SafeAny::Any* val = impl_->get(key);
        if( !val ){ return false; }

        value = val->convert<T>();
        return true;
    }

    std::unique_ptr<BlackboardImpl> impl_;
};


#endif // BLACKBOARD_H

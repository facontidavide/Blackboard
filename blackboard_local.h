#ifndef BLACKBOARD_LOCAL_H
#define BLACKBOARD_LOCAL_H

#include "blackboard.h"

class BlackboardLocal: public BlackboardImpl
{
public:

    BlackboardLocal() {}

    virtual bool get(const std::string& key, nonstd::any& value) const override
    {
        auto it = storage_.find(key);
        if( it == storage_.end() ){ return false; }
        value = it->second;
        return true;
    }

    virtual void set(const std::string& key, const nonstd::any& value) override
    {
        storage_[key] = value;
    }

    virtual bool get(const std::string& key, AnyNumber& value) const override
    {
        auto it = storage_num_.find(key);
        if( it == storage_num_.end() ){ return false; }
        value = it->second;
        return true;
    }

    virtual void set(const std::string& key, const AnyNumber& value) override
    {
        storage_num_[key] = value;
    }

    virtual bool get(const std::string& key, std::string& value) const override
    {
        auto it = storage_str_.find(key);
        if( it == storage_str_.end() ){ return false; }
        value = it->second;
        return true;
    }

    virtual void set(const std::string& key, const std::string& value) override
    {
        storage_str_[key] = value;
    }

private:
    std::unordered_map<std::string, nonstd::any> storage_;
    std::unordered_map<std::string, AnyNumber> storage_num_;
    std::unordered_map<std::string, std::string> storage_str_;
};


#endif // BLACKBOARD_LOCAL_H

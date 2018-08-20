#ifndef BLACKBOARD_LOCAL_H
#define BLACKBOARD_LOCAL_H

#include "blackboard.h"

class BlackboardLocal: public BlackboardImpl
{
public:

    BlackboardLocal() {}

    virtual bool get(const std::string& key, SafeAny::Any& value) const override
    {
        auto it = storage_.find(key);
        if( it == storage_.end() ){ return false; }
        value = it->second;
        return true;
    }

    virtual void set(const std::string& key, const SafeAny::Any& value) override
    {
        storage_[key] = value;
    }


private:
    std::unordered_map<std::string, SafeAny::Any> storage_;

};


#endif // BLACKBOARD_LOCAL_H

#ifndef RULEGROUP_HPP
#define RULEGROUP_HPP
#include "Rule.hpp"
#include <vector>
#include <list>

class RuleGroup
{
    public:
        std::list<Rule *> rules;
        Rule *top() const;
        std::string get_str() const;
        std::vector<Rule> incremented() const;
};

#endif
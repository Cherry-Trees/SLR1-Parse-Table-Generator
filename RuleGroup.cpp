#include "RuleGroup.hpp"

Rule *RuleGroup::top() const {return rules.front();}
std::string RuleGroup::get_str() const {return rules.front()->get_str();}
std::vector<Rule> RuleGroup::incremented() const {
    std::vector<Rule> rg;
    for (Rule *rule : rules)
        rg.push_back(rule->incremented());
    return rg;
}
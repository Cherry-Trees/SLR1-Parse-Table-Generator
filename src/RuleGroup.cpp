#include "RuleGroup.hpp"

Rule *RuleGroup::top() const {return rules.front();}
char RuleGroup::get() const {return top()->get();}
std::vector<Rule> RuleGroup::incremented() const {
    std::vector<Rule> rg;
    for (Rule *rule : rules)
        rg.push_back(rule->incremented());
    return rg;
}

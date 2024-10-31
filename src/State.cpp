#include "State.hpp"
#include "globals.hpp"
#include <iostream>

extern std::unordered_map<std::string, std::vector<Rule>> non_terminals;
extern std::unordered_set<State *> states;
extern std::list<State *> states_in_order;

Rule *State::top() const {return rules_in_order.front();}

void State::add_rule(const Rule &rule)
{
    auto inserted = rules.insert(rule);
    if (inserted.second)
        rules_in_order.push_back(&*inserted.first);   
}

void State::add_rules(const std::vector<Rule> &rls)
{
    for (const Rule &rl : rls) {
        auto inserted = rules.insert(rl);
        if (inserted.second)
            rules_in_order.push_back(&*inserted.first); 
    }  
}

void State::take_closure(const Rule rule)
{
    const char next_token = rule.get();
    const std::string next_token_str{next_token};
    if (std::isupper(next_token))
    {
        for (const Rule &global_rule : non_terminals[next_token_str])
            add_rule(global_rule);
        
        if (next_token_str != rule.lhs)
            for (const Rule &global_rule : non_terminals[next_token_str])
                take_closure(global_rule);
    }
} 

std::ostream &operator<<(std::ostream &os, const State &state)
{
    for (const Rule *rule : state.rules_in_order)
        os << *rule << "\\n";
    return os;
}
#include "State.hpp"
#include "globals.hpp"
#include <iostream>

extern std::unordered_map<char, NonTerminal> u_nonterminal_set;

Rule *State::top() const {return rules_in_order.front();}
bool State::add_rule(const Rule &rule) {
    auto inserted = rule_set.insert(rule);
    if (inserted.second) {
        rules_in_order.push_back(&*inserted.first);
        return 1;
    } return 0;
}

void State::add_rules(const std::vector<Rule> &rls) {
    for (const Rule &rl : rls) {
        auto inserted = rule_set.insert(rl);
        if (inserted.second)
            rules_in_order.push_back(&*inserted.first); 
    }  
}

void State::take_closure(const Rule rule) {
    const char next = rule.get();
    if (std::isupper(next))
    {
        bool rules_exist = 1;
        for (const Rule &u_rule : u_nonterminal_set[next].rules)
            if (add_rule(u_rule))
                rules_exist = 0;

        if (next != rule.lhs && (!rules_exist))
            for (const Rule &u_rule : u_nonterminal_set[next].rules)
                take_closure(u_rule);
    }
} 

std::ostream &operator<<(std::ostream &os, const State &state) {
    os << state.index << "\\n";
    for (const Rule *rule : state.rules_in_order)
        os << *rule << "\\n";
    return os;
}

void debug_print(const State &state) {
    for (const Rule *rule : state.rules_in_order)
        std::cout << *rule << std::endl;
}

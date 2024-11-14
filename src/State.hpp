#ifndef STATE_HPP
#define STATE_HPP

#include "Rule.hpp"
#include "NonTerminal.hpp"
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>

extern std::unordered_map<char, NonTerminal> u_nonterminal_set;

class State {       
    public:
        unsigned int index;
        std::list<Rule *> rules_in_order;
        std::unordered_set<Rule, Rule::Hash> rule_set;
        std::unordered_map<char, State *> adj;

        Rule *top() const;
        bool add_rule(const Rule &rule);
        void add_rules(const std::vector<Rule> &rls);
        void take_closure(const Rule rule);

    friend std::ostream &operator<<(std::ostream &os, const State &state);
    friend void debug_print(const State &state);
};

#endif

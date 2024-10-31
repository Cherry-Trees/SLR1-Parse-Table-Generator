#ifndef STATE_HPP
#define STATE_HPP

#include "Rule.hpp"
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>

class State;

extern std::unordered_map<std::string, std::vector<Rule>> non_terminals;
extern std::unordered_set<State *> states;
extern std::list<State *> states_in_order;

class State
{       
    public:
        std::list<Rule *> rules_in_order;
        std::unordered_set<Rule, Rule::Hash> rules;
        std::unordered_map<std::string, State *> adj;

        Rule *top() const;
        void add_rule(const Rule &rule);
        void add_rules(const std::vector<Rule> &rls);
        void take_closure(const Rule rule);

    friend std::ostream &operator<<(std::ostream &os, const State &state);
};

#endif
#include "functions.hpp"
#include <iostream>
#include <fstream>

extern std::unordered_map<std::string, std::vector<Rule>> non_terminals;
extern std::unordered_set<State *> states;
extern std::list<State *> states_in_order;

void add_state(State *state)
{
    auto inserted = states.insert(state);
    if (inserted.second)
        states_in_order.push_back(*inserted.first);
    else
        delete state;  
}


State *get_state(const std::vector<Rule> &top)
{
    for (State *state : states) {
        unsigned int i = 0;
        bool same = 1;
        if (state->rules_in_order.size() >= top.size()) {
            for (Rule *rule : state->rules_in_order) {
                if (top[i] != *rule)
                    same = 0;
                
                i++;
                if (i >= top.size()) break;   
            }
            if (same) return state;
        }
    }
    return 0;
}

void make_states(State *state)
{
    // Take the closure.
    for (Rule *rule : state->rules_in_order)
        state->take_closure(*rule);
    
    // Store the state.
    add_state(state);

    // Group rules based on next input token.
    std::map<std::string, RuleGroup> rgs;
    for (Rule *rule : state->rules_in_order)
        if (rule->index < rule->rhs.size())   
        {
            RuleGroup rg; rg.rules.push_back(rule);
            auto inserted = rgs.insert({rule->get_str(), rg});
            if (!inserted.second)
                rgs[rule->get_str()].rules.push_back(rule);
        }

    
    for (const std::pair<std::string, RuleGroup> &rg_pair : rgs)
    {
        const RuleGroup *rg = &rg_pair.second; // For readability.
        std::vector<Rule> copies = rg->incremented();
        State *next = get_state(copies);

        if (next) {
            state->adj.insert({rg->get_str(), next});
            continue;
        }

        next = new State;
        next->add_rules(copies);
        state->adj.insert({rg->get_str(), next});
        make_states(next);
    }
}

void build(std::ifstream &in)
{
    std::ofstream out;
    out.open("automata.dot");

    State *start = new State;
    while (in)
    {
        std::string line;
        std::getline(in, line);
        if (line != "")
        {
            if (!non_terminals.emplace(line.substr(0, 1), std::vector<Rule>{line}).second)
                non_terminals[line.substr(0, 1)].push_back(line);
        }
    }
    start->add_rule("P->S");
    make_states(start);

    out << "digraph {" << std::endl;

    for (State *s : states_in_order)
        out << "\ts" << s << "[label=\"" << *s << "\", shape=\"Rect\"];" << std::endl;
    
    for (State *s : states_in_order)
        for (const std::pair<std::string, State *> transition : s->adj)
            out << "\ts" << s << "->s" << transition.second << "[label=\"" << transition.first << "\"]" << std::endl;

    out << '}';
    for (State *s : states)
        delete s;
    
    out.close();
}
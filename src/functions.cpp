#include "functions.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>

#define EPSILON *(char *)"\uEFFF"

extern std::unordered_map<std::string, std::vector<Rule>> non_terminals;
extern std::vector<Rule> rules;
extern std::unordered_set<State *> states;
extern std::list<State *> states_in_order;
extern std::size_t state_index_counter;
extern std::unordered_map<std::string, std::unordered_set<char>> first;
extern std::unordered_map<std::string, std::unordered_set<char>> follow;

bool add_state(State *state)
{
    auto inserted = states.insert(state);
    if (inserted.second) {
        states_in_order.push_back(*inserted.first);
        return 1;
    }
    else {
        delete state; 
        return 0;
    } 
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
    if (add_state(state))
        state->index = state_index_counter++;

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

void read_input_file(std::ifstream &in)
{
    while (in)
    {
        std::string line;
        std::getline(in, line);
        if (line != "") {
            if (!non_terminals.emplace(line.substr(0, 1), std::vector<Rule>{line}).second) 
                non_terminals[line.substr(0, 1)].push_back(line);
        }
    }
}

void write_dot_file()
{
    std::ofstream out;
    out.open("automata.dot");

    std::string font = "Courier";
    out << "digraph {" << std::endl
        << "bgcolor=\"white\"" << std::endl
        << "rankdir=\"LR\"" << std::endl
        << "node[shape=box,fontname=\"" << font << "\",color=\"black\",fontcolor=\"black\"]" << std::endl
        << "edge[fontname=\"" << font << "\",color=\"black\"]" << std::endl;

    for (State *s : states_in_order)
        out << 's' << s << "[label=\"" << *s << "\"]" << std::endl;
    
    for (State *s : states_in_order)
        for (const std::pair<std::string, State *> transition : s->adj)
            out << 's' << s << "->s" << transition.second << "[label=\"" << transition.first << "\",fontcolor=\"black\"]" << std::endl;

    out << '}';
    out.close();
}

void clean_states()
{
    for (State *s : states)
        delete s;
}

void union_sets(const std::unordered_set<char> s1, 
                const std::unordered_set<char> s2, 
                std::unordered_set<char> *dest) {
    std::set_union(
        s1.cbegin(), s1.cend(), 
        s2.cbegin(), s2.cend(), 
        std::inserter(*dest, dest->end())
    );
}

bool contains_epsilon(const std::unordered_set<char> *s) {
    return s->find(EPSILON) != s->end() && s->size() > 0;
}

// TODO: make this better :3
bool goes_to_epsilon(const std::string lhs) {
    for (const Rule &rule : non_terminals[lhs])
        if (rule.rhs.empty())
            return 1;
    return 0;
}


void get_first(std::string rhs, std::unordered_set<std::string> &visited) {

    if (visited.contains({rhs[0]}) && goes_to_epsilon(rhs.substr(0, 1)))
        return get_first(rhs.erase(0, 1), visited);
    
    if (visited.contains({rhs[0]}))
        return;

    if (rhs.empty()) {
        visited.insert({EPSILON});
        return;
    }

    if (std::islower(rhs[0]) || !std::isalpha(rhs[0])) {
        visited.insert({rhs[0]});
        return;
    }

    std::string front = rhs.substr(0, 1);
    visited.insert(front); rhs.erase(0, 1);
    for (const Rule &rule : non_terminals[front])
        get_first(rule.rhs + rhs, visited);
}

void init_first() {
    for (const std::pair<std::string, std::vector<Rule>> &nt : non_terminals) {
        std::unordered_set<std::string> visited;
        for (const Rule &rule : nt.second) 
            get_first(rule.rhs, visited);

        for (const std::string &token : visited) 
            if (std::islower(token[0]) || token[0] == EPSILON)
                first[nt.first].insert(token[0]);
    }
}

void build(std::ifstream &in)
{
    read_input_file(in);

    State *start = new State;
    start->add_rule("P->S");

    make_states(start);
    write_dot_file();

    init_first();
    
    for (const std::pair<std::string, std::unordered_set<char>> &first_set : first) {
        std::cout << "first(" << first_set.first << ") = { ";
        for (const char token : first_set.second)
            std::cout << token << ' ';
        std::cout << '}' << std::endl;
    }

    clean_states();
}

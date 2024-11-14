#include "NonTerminal.hpp"
#include <iostream>
#include <algorithm>

extern std::unordered_map<char, NonTerminal> u_nonterminal_set;

NonTerminal::SetNode::SetNode(std::unordered_set<char> *set) : set(set), unioned(0) {}

NonTerminal::SetNode *NonTerminal::SetNode::direct_to(SetNode *other) {
    adj.push_back(other);
    return this;
}

// BFS
void NonTerminal::SetNode::set_union() {
    std::unordered_map<SetNode *, bool> visited;
    std::queue<SetNode *> queue;
    queue.push(this);

    while (!queue.empty()) {
        SetNode *next = queue.front(); queue.pop();
        std::set_union(
            set->cbegin(), set->cend(),
            next->set->cbegin(), next->set->cend(),
            std::inserter(*set, set->end())
        );
        if (!next->unioned)
            for (SetNode *adj: next->adj)
                if (!visited[adj]) {
                    queue.push(adj);
                    visited[adj] = 1;
                }
    } 
    unioned = 1;
}

std::size_t NonTerminal::Hash::operator()(const NonTerminal &nt) const {
    return std::hash<char>()(nt.symbol);
}

NonTerminal::NonTerminal(const std::vector<Rule> &rule_vec) : symbol(rule_vec.front().lhs), rules(rule_vec), follow_node() {
    follow_node.set = &follow;
}

NonTerminal::NonTerminal(const NonTerminal &other) {
    symbol = other.symbol;
    rules = other.rules;
    first = other.first;
    follow = other.follow;
    follow_node = other.follow_node;
}

NonTerminal &NonTerminal::operator=(const NonTerminal &other) {
    symbol = other.symbol;
    rules = other.rules;
    first = other.first;
    follow = other.follow;
    follow_node = other.follow_node;
    return *this;
}
bool NonTerminal::operator==(const NonTerminal &other) const {return symbol == other.symbol;}
bool NonTerminal::operator!=(const NonTerminal &other) const {return symbol != other.symbol;}

bool NonTerminal::goes_to_epsilon() const {
    for (const Rule &rule : rules)
        if (rule.rhs.empty()) 
            return 1;
    
    return 0;
}

std::ostream &NonTerminal::out_first(std::ostream &os) const {
    os << "first(" << symbol << ") = { ";
    for (char token : first)
        os << token << ' ';
    os << '}' << std::endl;
    return os;
}
std::ostream &NonTerminal::out_follow(std::ostream &os) const {
    os << "follow(" << symbol << ") = { ";
    for (char token : follow)
        os << token << ' ';
    os << '}' << std::endl;
    return os;
}
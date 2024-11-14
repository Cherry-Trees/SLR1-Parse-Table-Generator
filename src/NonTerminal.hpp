#ifndef NONTERMINAL_HPP
#define NONTERMINAL_HPP

#include "Rule.hpp"
#include <unordered_set>
#include <vector>
#include <unordered_map>
#include <queue>
#include <iosfwd>

#define EPSILON *(char *)"\uEFFF"

class NonTerminal;

extern std::unordered_map<char, NonTerminal> u_nonterminal_set;

class NonTerminal {
    public:
        struct Hash {
            std::size_t operator()(const NonTerminal &nt) const;
        };
        class SetNode {
            private:
                bool unioned;

            public:
                std::unordered_set<char> *set;
                std::vector<SetNode *> adj;

            public:
                SetNode() : unioned(0) {}
                SetNode(std::unordered_set<char> *set);
                SetNode *direct_to(SetNode *other);
                void set_union();
        };
        
    public:
        char symbol;
        std::vector<Rule> rules;
        std::unordered_set<char> first, follow;
        SetNode follow_node;
    
    public:
        NonTerminal() = default;
        NonTerminal(const NonTerminal &other);
        NonTerminal(const std::vector<Rule> &rule_vec);

        NonTerminal &operator=(const NonTerminal &other);
        bool operator==(const NonTerminal &other) const;
        bool operator!=(const NonTerminal &other) const;

        bool goes_to_epsilon() const;
        std::ostream &out_first(std::ostream &os) const;
        std::ostream &out_follow(std::ostream &os) const;
};

#endif
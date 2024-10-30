#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

class Rule;
class State;

class Rule
{
    public:
        struct Hash {
            std::size_t operator()(const Rule &rule) const {
                return std::hash<std::string>()(rule.rhs) ^ std::hash<unsigned int>()(rule.index);
            }
        };  

    private:
        explicit Rule(const std::string lhs, const std::string rhs, const unsigned int index) : lhs(lhs), rhs(rhs), index(index) {}

    public:
        std::string lhs, rhs;
        unsigned int index;

        Rule() = default;
        Rule(const std::string lhs, const std::string rhs) : lhs(lhs), rhs(rhs), index(0) {}
        Rule(const Rule &other) : lhs(other.lhs), rhs(other.rhs), index(other.index) {}

        Rule(const std::string line) : lhs(line.substr(0, 1)), rhs(line.substr(3)), index(0) {}
        Rule (const char *line) : lhs{line[0]}, rhs(line + 3), index(0) {}

        char &get() {return rhs[index];}
        char get() const {return rhs[index];}
        std::string get_str() const {return {rhs[index]};}

        Rule *operator&() const {return (Rule *)this;}

        Rule &operator=(const Rule &other)
        {
            lhs = other.lhs;
            rhs = other.rhs;
            index = other.index;
            return *this;
        }


        Rule incremented() const {return Rule(lhs, rhs, index + 1);}
        bool operator==(const Rule &other) const {return rhs == other.rhs && index == other.index;}
        bool operator!=(const Rule &other) const {return rhs != other.rhs || index != other.index;}

    
    friend std::ostream &operator<<(std::ostream &os, const Rule rule)
    {
        return os << rule.lhs << " -> " << rule.rhs.substr(0, rule.index) << '_' << rule.rhs.substr(rule.index);
    }
};

std::unordered_map<std::string, std::vector<Rule>> global_nts;
std::unordered_set<State *> states;
std::list<State *> states_in_order;

class State
{       
    public:
        std::list<Rule *> rules_in_order;
        std::unordered_set<Rule, Rule::Hash> rules;
        std::unordered_map<std::string, State *> adj;

        Rule *top() const {return rules_in_order.front();}

        void add_rule(const Rule &rule)
        {
            auto inserted = rules.insert(rule);
            if (inserted.second)
                rules_in_order.push_back(&*inserted.first);   
        }

        void add_rules(const std::vector<Rule> &rls)
        {
            for (const Rule &rl : rls) {
                auto inserted = rules.insert(rl);
                if (inserted.second)
                    rules_in_order.push_back(&*inserted.first); 
            }  
        }

        void take_closure(const Rule rule)
        {
            const char next_token = rule.get();
            const std::string next_token_str{next_token};
            if (std::isupper(next_token))
            {
                for (const Rule &global_rule : global_nts[next_token_str])
                    add_rule(global_rule);
                
                if (next_token_str != rule.lhs)
                    for (const Rule &global_rule : global_nts[next_token_str])
                        take_closure(global_rule);
            }
        } 

    friend std::ostream &operator<<(std::ostream &os, const State &state)
    {
        for (const Rule *rule : state.rules_in_order)
            os << *rule << std::endl;
        return os;
    }
};


class RuleGroup
{
    public:
        std::list<Rule *> rules;
        Rule *top() const {return rules.front();}
        std::string get_str() const {return rules.front()->get_str();}
        std::vector<Rule> incremented() const {
            std::vector<Rule> rg;
            for (Rule *rule : rules)
                rg.push_back(rule->incremented());
            return rg;
        }
};

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
    std::unordered_map<std::string, RuleGroup> rgs;
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

void build(std::ifstream &input)
{
    State *start = new State;
    std::cout << "INPUT:" << std::endl;
    while (input)
    {
        std::string line;
        std::getline(input, line);
        if (line != "")
        {
            std::cout << line << std::endl;
            if (!global_nts.emplace(line.substr(0, 1), std::vector<Rule>{line}).second)
                global_nts[line.substr(0, 1)].push_back(line);
        }
    }
    start->add_rule("P->S");
    make_states(start);
    std::cout << std::endl << "STATES:" << std::endl;

    unsigned int i = 0;
    for (State *s : states_in_order)
    {
        std::cout << i++ << std::endl;
        std::cout << *s << std::endl;
    }

    for (State *s : states)
        delete s;
}


int main(int argc, char *argv[])
{
    if (argc != 2)
        std::cerr << "Usage: a.exe <filename>" << std::endl;
    
    std::ifstream input(argv[1]);
    build(input);

    return 0;
};

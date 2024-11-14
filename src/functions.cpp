#include "functions.hpp"
#include <iostream>
#include <algorithm>

#define EPSILON *(char *)"\uEFFF"

extern std::unordered_set<char> u_terminal_set;
extern std::unordered_map<char, NonTerminal> u_nonterminal_set;
extern std::vector<Rule> rules_contain_nonterminal;
extern std::set<State *> u_state_set; // states have to be ordered.
extern std::size_t state_index_counter;
extern std::vector<std::unordered_map<char, std::vector<std::string>>> action;
extern std::vector<std::unordered_map<char, std::size_t>> go_to;

void read_input_file(std::ifstream &in) {
    std::cout << "INPUT:\n";
    while (in) {
        bool contains_nonterminal = 0;
        std::string line;
        std::getline(in, line);
        if (line != "") {
            std::cout << line << std::endl;
            auto inserted = u_nonterminal_set.emplace(line[0], std::vector<Rule>{line});
            if (!inserted.second) {
                u_nonterminal_set[line[0]].rules.push_back(line);
            }
            
            for (char token : line.substr(3)) {
                if (!std::isupper(token))
                    u_terminal_set.insert(token);
                else 
                    contains_nonterminal = 1;
            }
            if (contains_nonterminal)
                rules_contain_nonterminal.emplace_back(line);
        }
    }
}

bool add_state(State *state) {
    auto inserted = u_state_set.insert(state);
    if (inserted.second) 
        return 1;

    delete state; 
    return 0;
}

State *get_state(const std::vector<Rule> &top) {
    for (State *state : u_state_set) {
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

void make_states(State *state) {

    // Take the closure.
    for (Rule *rule : state->rules_in_order)
        state->take_closure(*rule);
    
    // Store the state.
    if (add_state(state))
        state->index = state_index_counter++;

    // Group rules based on next input token.
    std::map<char, RuleGroup> rgs;
    for (Rule *rule : state->rules_in_order)
        if (rule->index < rule->rhs.size())  {
            RuleGroup rg; rg.rules.push_back(rule);
            auto inserted = rgs.insert({rule->get(), rg});
            if (!inserted.second)
                rgs[rule->get()].rules.push_back(rule);
        }

    for (const std::pair<char, RuleGroup> &rg_pair : rgs) {
        const RuleGroup *rg = &rg_pair.second; // For readability.
        std::vector<Rule> copies = rg->incremented();
        State *next = get_state(copies);

        if (next) {
            state->adj.insert({rg->get(), next});
            continue;
        }
        next = new State;
        next->add_rules(copies);
        state->adj.insert({rg->get(), next});
        make_states(next);
    }
}

void clean_states() {
    for (State *state : u_state_set)
        delete state;
}

void write_automata_dot_file() {
    std::ofstream out;
    out.open("automata.dot");

    std::string font = "Courier";
    out << "digraph {" << std::endl
        << "bgcolor=\"white\"" << std::endl
        << "rankdir=\"UD\"" << std::endl
        << "node[shape=box,fontname=\"" << font << "\",color=\"black\",fontcolor=\"black\"]" << std::endl
        << "edge[fontname=\"" << font << "\",color=\"black\"]" << std::endl;

    for (State *s : u_state_set)
        out << 's' << s << "[label=\"" << *s << "\"]" << std::endl;
    
    for (State *s : u_state_set)
        for (const std::pair<char, State *> &t : s->adj)
            out << 's' << s << "->s" << t.second << "[label=\"" << t.first << "\",fontcolor=\"black\"]" << std::endl;

    out << '}';
    out.close();
}

void get_first(std::string rhs, std::unordered_set<char> &visited) {
    char front = rhs.front();
    if (visited.contains(front)) {
        if (std::isupper(front) && u_nonterminal_set[front].goes_to_epsilon())
            return get_first(rhs.erase(0, 1), visited); 
        return;
    }
    if (rhs.empty()) {
        visited.insert(EPSILON);
        return;
    }
    if (std::islower(front) || !std::isalpha(front)) {
        visited.insert(front);
        return;
    }
    visited.insert(front); rhs.erase(0, 1);
    for (const Rule &rule : u_nonterminal_set[front].rules)
        get_first(rule.rhs + rhs, visited);
}

void init_first_sets() {
    for (auto & [symbol, nt] : u_nonterminal_set) {
        std::unordered_set<char> visited;
        for (const Rule &rule : nt.rules) 
            get_first(rule.rhs, visited);

        for (const char &token : visited) 
            if (std::islower(token) || token == EPSILON)
                nt.first.insert(token);
    }
}

void init_follow_sets() {
    for (const Rule &rule : rules_contain_nonterminal) {
        NonTerminal *lhs_nt = &u_nonterminal_set[rule.lhs];
        if (rule.lhs == 'P')
            lhs_nt->follow.insert('$');

        std::string rhs = rule.rhs;
        while (!rhs.empty()) {
            
            // Get to the next nonterminal in the rhs.
            while (!std::isupper(rhs.front()) && !rhs.empty()) {
                rhs.erase(0, 1);
            } 
            // Skip if no nonterminal was found
            if (rhs.empty()) break;

            // Store a pointer to the nonterminal.
            NonTerminal *rhs_nt = &u_nonterminal_set[rhs.front()];

            /* If the nonterminal is at the end of the rule, whatever is in the follow
               of the lhs is in the follow of the nonterminal. */
            if (rhs.erase(0, 1).empty())
                rhs_nt->follow_node.direct_to(&lhs_nt->follow_node);
            
            // If the next token is a terminal, add it to the follow set.
            else if (std::islower(rhs.front()) || !std::isalpha(rhs.front()))
                rhs_nt->follow.insert(rhs.front());

            /* If the next token is a terminal, whatever is in the first of the lhs
               is in the follow of the nonterminal. If the first set contains epsilon,
               repeat until a terminal is reached or the rhs is empty. */
            else {
                // Store consecutive nonterminals so we can update all of their follow sets.
                std::vector<NonTerminal *> nt_consecs{rhs_nt};
                bool front_contains_epsilon = 0;

                // Iterate through consecutive epsilon-containing nonterminals.
                do {
                    front_contains_epsilon = 0;
                    NonTerminal *front_nt = &u_nonterminal_set[rhs.front()];
                    for (NonTerminal *nt : nt_consecs) {
                        std::set_union(
                            nt->follow.cbegin(), nt->follow.cend(),
                            front_nt->first.cbegin(), front_nt->first.cend(),
                            std::inserter(nt->follow, nt->follow.end())
                        );
                        if (nt->follow.erase(EPSILON))
                            front_contains_epsilon = 1;
                    }
                    nt_consecs.push_back(front_nt);
                } while (std::isupper(rhs.erase(0, 1).front()) && front_contains_epsilon);

                if (front_contains_epsilon) {
                    if (rhs.empty())
                        for (NonTerminal *nt : nt_consecs)
                            nt->follow_node.direct_to(&lhs_nt->follow_node);
                    else if (!std::isupper(rhs.front()))
                        for (NonTerminal *nt : nt_consecs)
                            nt->follow.insert(rhs.front());
                } 
                else {
                    rhs = nt_consecs.back()->symbol + rhs;
                }
            }   
        }
    }
    for (auto & [sym, nt] : u_nonterminal_set)
        nt.follow_node.set_union();
}

void write_follow_dot_file() {
    std::ofstream out;
    out.open("follow.dot");

    std::string font = "Courier";
    out << "digraph {" << std::endl
        << "bgcolor=\"white\"" << std::endl
        << "rankdir=\"UD\"" << std::endl
        << "node[shape=rect,fontname=\"" << font << "\",color=\"black\",fontcolor=\"black\"]" << std::endl
        << "edge[fontname=\"" << font << "\",color=\"black\"]" << std::endl;

    for (auto & [sym, nt] : u_nonterminal_set) {
        out << 's' << &nt.follow_node <<  "[label=\"" << "follow(" << sym << ")\"]" << std::endl;
    }
    
    for (auto & [sym, nt] : u_nonterminal_set)
        for (NonTerminal::SetNode *adj : nt.follow_node.adj)
            out << 's' << &nt.follow_node << "->s" << adj << "[fontcolor=\"black\"]" << std::endl;

    out << '}';
    out.close();
}


void write_csv_file() {
    u_terminal_set.insert('$');

    // Init action and go_to rows.
    for (std::size_t i = 0; i < u_state_set.size(); i++) {
        std::unordered_map<char, std::vector<std::string>> action_col;
        std::unordered_map<char, std::size_t> go_to_col;
        for (char token : u_terminal_set)
            action_col.insert({token, {}});
        
        for (auto & [sym, nt] : u_nonterminal_set)
            go_to_col.insert({sym, SIZE_MAX});

        action.push_back(action_col);
        go_to.push_back(go_to_col);
    }

    std::ofstream out;
    out.open("table.csv");
    
    for (const State *state : u_state_set) {
        for (auto & [sym, adj] : state->adj) {
            if (!std::isupper(sym)) {
                action[state->index][sym].push_back("s" + std::to_string(adj->index));
            }
            else if (sym != 'P') {
                go_to[state->index][sym] = adj->index;
            }

        }
        for (Rule *rule : state->rules_in_order) {
            if (rule->eoi()) {
                for (const char token : u_nonterminal_set[rule->lhs].follow) {
                    std::string reduce, rhs;
                    if (rule->rhs.empty()) rhs = "\u03B5";
                    else rhs = rule->rhs;
                    reduce = std::string(1, 'r') + rule->lhs + "->" + rhs;
                    if (rule->lhs == 'P') reduce = "acc";
                    action[state->index][token].push_back(reduce);
                }
            }
        }
    }

    out << ',';
    std::size_t k = 0;
    for (auto & [sym, sr] : action[0]) {
        out << sym << ',';
    }
    for (auto & [sym, num] : go_to[0]) {
        k++;
        if (sym == 'P') continue;
        out << sym;
        if (k < go_to[0].size()) out << ',';
    }
    out << std::endl;
    for (std::size_t i = 0; i < u_state_set.size(); i++) {
        out << i << ',';
        for (auto & [sym, entry] : action[i]) {
            int m = 0;
            for (std::string sr : entry) {
                m++;
                out << sr;
                if (m < entry.size()) out << " / ";
            }
            out << ',';
        }
        std::size_t k = 0;
        for (auto & [sym, num] : go_to[i]) {

            k++;
            if (sym == 'P') continue;
            if (num != SIZE_MAX)
                out << num;
            if (k < go_to[i].size()) out << ',';
        }
        out << std::endl;
    }
    out.close();
}

void build(std::ifstream &in)
{

    std::string augment = "P->S";
    u_nonterminal_set.emplace('P', std::vector<Rule>{augment});
    rules_contain_nonterminal.push_back(u_nonterminal_set['P'].rules.front());
    read_input_file(in);
    
    State *start = new State;

    start->add_rule(augment);

    make_states(start);
    write_automata_dot_file();
    init_first_sets();

    for (auto & [sym, nt] : u_nonterminal_set) {
        nt.out_first(std::cout);
    }

    init_follow_sets();

    for (auto & [sym, nt] : u_nonterminal_set) {
        nt.out_follow(std::cout);
    }

    // write_follow_dot_file();
    write_csv_file();
    clean_states();
}

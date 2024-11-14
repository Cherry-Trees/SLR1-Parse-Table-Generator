#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include <unordered_map>
#include <unordered_set>
#include <list>
#include <set>
#include "Rule.hpp"
#include "State.hpp"
#include "NonTerminal.hpp"

std::unordered_set<char> u_terminal_set;
std::unordered_map<char, NonTerminal> u_nonterminal_set;
std::vector<Rule> rules_contain_nonterminal;
std::set<State *> u_state_set;
std::size_t state_index_counter = 0;

std::vector<std::unordered_map<char, std::vector<std::string>>> action;
std::vector<std::unordered_map<char, std::size_t>> go_to;

#endif

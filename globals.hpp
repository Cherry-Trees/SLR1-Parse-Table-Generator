#ifndef GLOBALS_HPP
#define GLOBALS_HPP
#include "State.hpp"

std::unordered_map<std::string, std::vector<Rule>> non_terminals;
std::unordered_set<State *> states;
std::list<State *> states_in_order;

#endif
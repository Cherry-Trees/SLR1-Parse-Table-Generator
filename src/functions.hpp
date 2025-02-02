#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include "RuleGroup.hpp"
#include "State.hpp"
#include "NonTerminal.hpp"
#include <fstream>
#include <set>

bool add_state(State *state);
State *get_state(const std::vector<Rule> &top);
void make_states(State *state);
void build(std::ifstream &input);

#endif

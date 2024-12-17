#include "Rule.hpp"
#include <iostream>

std::size_t Rule::Hash::operator()(const Rule &rule) const {
    return std::hash<std::string>()(rule.lhs) ^ std::hash<std::string>()(rule.rhs) ^ std::hash<unsigned int>()(rule.index);
}
Rule::Rule(const std::string lhs, const std::string rhs, const unsigned int index) : lhs(lhs), rhs(rhs), index(index) {}
Rule::Rule() = default;
Rule::Rule(const std::string lhs, const std::string rhs) : lhs(lhs), rhs(rhs), index(0) {}
Rule::Rule(const Rule &other) : lhs(other.lhs), rhs(other.rhs), index(other.index) {}
Rule::Rule(const std::string line) : lhs(line.substr(0, 1)), rhs(line.substr(3)), index(0) {}
Rule::Rule (const char *line) : lhs{line[0]}, rhs(line + 3), index(0) {}
char &Rule::get() {return rhs[index];}
char Rule::get() const {return rhs[index];}
std::string Rule::get_str() const {return {rhs[index]};}
Rule *Rule::operator&() const {return (Rule *)this;}
Rule &Rule::operator=(const Rule &other) {
    lhs = other.lhs;
    rhs = other.rhs;
    index = other.index;
    return *this;
}
Rule Rule::incremented() const {return Rule(lhs, rhs, index + 1);}
bool Rule::eoi() const {return index == rhs.size();}
bool Rule::operator==(const Rule &other) const {return lhs == other.lhs && rhs == other.rhs && index == other.index;}
bool Rule::operator!=(const Rule &other) const {return lhs != other.lhs || rhs != other.rhs || index != other.index;}
std::ostream &operator<<(std::ostream &os, const Rule rule) {
    if (rule.rhs.empty())
        return os << rule.lhs << "->" << "\u03B5_";
    return os << rule.lhs << "->" << rule.rhs.substr(0, rule.index) << '_' << rule.rhs.substr(rule.index);
}

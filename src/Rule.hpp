#ifndef RULES_HPP
#define RULES_HPP
#include <iosfwd>
#include <string>

class Rule
{
    public:
        struct Hash {
            std::size_t operator()(const Rule &rule) const;
        };
        struct Compare {
            bool operator()(const Rule &r1, const Rule &r2) const;
        };

    private:
        Rule(const char lhs, const std::string rhs, const unsigned int index);

    public:
        char lhs;
        std::string rhs;
        unsigned int index;

        Rule();
        Rule(const Rule &other);
        Rule(const std::string line);

        char &get();
        char get() const;
        Rule incremented() const;
        bool eoi() const;

        Rule *operator&() const;
        Rule &operator=(const Rule &other);
        bool operator==(const Rule &other) const;
        bool operator!=(const Rule &other) const;

    friend std::ostream &operator<<(std::ostream &os, const Rule rule);
};

#endif

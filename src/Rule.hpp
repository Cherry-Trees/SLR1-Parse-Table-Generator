#pragma once
#include <iosfwd>
#include <string>

class Rule
{
    public:
        struct Hash {
            std::size_t operator()(const Rule &rule) const;
        };

    private:
        Rule(const std::string lhs, const std::string rhs, const unsigned int index);

    public:
        std::string lhs, rhs;
        unsigned int index;

        Rule();
        Rule(const std::string lhs, const std::string rhs);
        Rule(const Rule &other);
        Rule(const std::string line);
        Rule(const char *line);

        char &get();
        char get() const;
        std::string get_str() const;
        Rule incremented() const;

        Rule *operator&() const;
        Rule &operator=(const Rule &other);
        bool operator==(const Rule &other) const;
        bool operator!=(const Rule &other) const;

    friend std::ostream &operator<<(std::ostream &os, const Rule rule);
};
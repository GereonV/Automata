#include <iostream>
#include <string_view>
#include "automata.hpp"

int main() {
    using namespace std::literals;
    auto alphabet = fa::alphabet<char>("AB"sv);
    fa::nfa nfa{{
        {{
            {0, 0},
            {1, 1}
        }, false},
        {{
            {0, 0},
            {1, 0},
            {1, 1}
        }, true}
    }};
    auto alphs = alphabet.size(); // nfa.states.size()
    auto dfa = fa::nfa_to_dfa(nfa, alphs);
    auto not_equal = [&](auto && in, bool should) { auto ans = evaluate(in, dfa, alphs); return ans ^ should || ans ^ evaluate(in, nfa); };
    if(not_equal(std::initializer_list{0, 1, 1, 0}, false) ||
       not_equal(std::initializer_list{1, 0, 1, 0}, false) ||
       not_equal(std::initializer_list{1, 0, 1, 1}, true)  ||
       not_equal("ABBA"sv | fa::map(alphabet), false)      ||
       not_equal("BABA"sv | fa::map(alphabet), false)      ||
       not_equal("ABAB"sv | fa::map(alphabet), true))
        return -1;
    for(std::size_t i{}; i < dfa.delta.size() / alphs; ++i) {
        std::cout << "State " << i << (dfa.accepting_states.contains(i) ? " (accepting)" : "") << ":\n";
        for(std::size_t j{}; j < alphs; ++j)
            std::cout << j << ':' << dfa.delta[i * alphs + j] << '\n';
    }
    std::cout << std::endl;
    dfa = minimize(dfa, alphs);
    for(std::size_t i{}; i < dfa.delta.size() / alphs; ++i) {
        std::cout << "State " << i << (dfa.accepting_states.contains(i) ? " (accepting)" : "") << ":\n";
        for(std::size_t j{}; j < alphs; ++j)
            std::cout << j << ':' << dfa.delta[i * alphs + j] << '\n';
    }
}

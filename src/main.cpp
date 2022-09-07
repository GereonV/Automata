#include <iostream>
#include "automata.hpp"

int main() {
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
    auto alphs = nfa.states.size();
    std::cout << evaluate(std::initializer_list{0, 1, 1, 0}, nfa) << '\n';
    std::cout << evaluate(std::initializer_list{1, 0, 1, 1}, nfa) << '\n';
    auto dfa = fa::nfa_to_dfa(nfa, alphs);
    std::cout << evaluate(std::initializer_list{0, 1, 1, 0}, dfa, alphs) << '\n';
    std::cout << evaluate(std::initializer_list{1, 0, 1, 1}, dfa, alphs) << '\n';
    for(int i{}; i < dfa.delta.size() / alphs; ++i) {
        std::cout << "State " << i << (dfa.accepting_states.contains(i) ? " (accepting)" : "") << ":\n";
        for(int j{}; j < alphs; ++j)
            std::cout << j << ':' << dfa.delta[i * alphs + j] << '\n';
    }
}

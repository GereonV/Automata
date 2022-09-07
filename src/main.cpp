#include <iostream>
#include "automata.hpp"

int main() {
    std::size_t alphs{2};
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
    auto dfa = fa::nfa_to_dfa(nfa, alphs);
    for(int i{}; i < dfa.delta.size() / alphs; ++i) {
        std::cout << "State " << i << (dfa.accepting_states.contains(i) ? " (accepting)" : "") << ":\n";
        for(int j{}; j < alphs; ++j)
            std::cout << j << ':' << dfa.delta[i * alphs + j] << '\n';
    }
}

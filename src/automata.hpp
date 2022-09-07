#ifndef _AUTOMATA_HPP_
#define _AUTOMATA_HPP_

#include <map>
#include <ranges>
#include <set>
#include <span>
#include <vector>

namespace fa {

    struct nfa {
        std::vector<std::pair<std::multimap<std::size_t, std::size_t>, bool>> states;
        std::size_t start{};
    };

    struct dfa {
        std::vector<std::size_t> delta;
        std::set<std::size_t> accepting_states;
        std::size_t start;
    };

    dfa nfa_to_dfa(nfa const & nfa, std::size_t alphabet_size) noexcept {
        dfa dfa;
        dfa.start = 0;
        std::vector<std::set<std::size_t>> states{{nfa.start}};
        for(std::size_t completed{}; completed < states.size(); ++completed) {
            auto state = states[completed]; // copy
            dfa.delta.resize(dfa.delta.size() + alphabet_size);
            auto it = dfa.delta.end();
            for(auto i = alphabet_size; i--;) {
                bool accepting{};
                std::set<std::size_t> og_successors;
                for(auto og_state : state) {
                    auto && [begin, end] = nfa.states[og_state].first.equal_range(i);
                    for(auto [_, succ] : std::ranges::subrange{begin, end}) {
                        og_successors.emplace(succ);
                        if(nfa.states[succ].second)
                            accepting = true;
                    }
                }
                auto corr_state = std::ranges::find(states, og_successors);
                auto pos = corr_state - states.begin();
                if(corr_state == states.end())
                    states.push_back(og_successors);
                *--it = pos;
                if(accepting)
                    dfa.accepting_states.emplace(pos);
            }
        }
        return dfa;
    }

};

#endif // _AUTOMATA_HPP_

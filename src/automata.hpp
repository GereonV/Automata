#ifndef _AUTOMATA_HPP_
#define _AUTOMATA_HPP_

#include <algorithm>
#include <map>
#include <ranges>
#include <set>
#include <span>
#include <vector>

namespace fa {

    inline constexpr auto map = [](std::map<auto, std::size_t> const & alphabet) { return std::views::transform([&](char c) { return alphabet.at(c); }); };

    template<typename T>
    std::map<T, std::size_t> alphabet(auto && symbols) noexcept {
        std::map<T, std::size_t> alpha;
        for(std::size_t i{}; auto && s : symbols)
            alpha.emplace(s, i++);
        return alpha;
    }

    template<typename T>
    struct it_pair {
        T pair;
        constexpr auto begin() const noexcept { auto && [b, _] = pair; return b; }
        constexpr auto end()   const noexcept { auto && [_, e] = pair; return e; }
    };

    struct nfa {
        std::vector<std::pair<std::multimap<std::size_t, std::size_t>, bool>> states;
        std::size_t start{};
    };

    bool evaluate(auto && input, nfa const & nfa) noexcept {
        std::set curr{nfa.start};
        for(auto c : input) {
            std::set<std::size_t> next;
            for(auto && state : curr)
                for(auto && s : it_pair{nfa.states[state].first.equal_range(c)})
                    next.emplace(s.second);
            curr.swap(next);
        }
        return std::ranges::any_of(curr, [&](auto state) { return nfa.states[state].second; });
    }

    struct dfa {
        std::vector<std::size_t> delta;
        std::set<std::size_t> accepting_states;
        std::size_t start;
    };

    bool evaluate(auto && input, dfa const & dfa, std::size_t alphabet_size) noexcept {
        auto curr = dfa.start;
        for(auto c : input)
            curr = dfa.delta[curr * alphabet_size + c];
        return dfa.accepting_states.contains(curr);
    }

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
                    for(auto [_, succ] : it_pair{nfa.states[og_state].first.equal_range(i)}) {
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

#ifndef _AUTOMATA_HPP_
#define _AUTOMATA_HPP_

#include <algorithm>
#include <map>
#include <ranges>
#include <set>
#include <span>
#include <vector>

namespace fa {

    inline constexpr auto map = [](std::map<auto, std::size_t> const & alphabet) { return std::views::transform([&](auto && c) { return alphabet.at(c); }); };

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
        std::size_t start{};
    };

    bool evaluate(auto && input, dfa const & dfa, std::size_t alphabet_size) noexcept {
        auto curr = dfa.start;
        for(auto c : input)
            curr = dfa.delta[curr * alphabet_size + c];
        return dfa.accepting_states.contains(curr);
    }

    dfa nfa_to_dfa(nfa const & nfa, std::size_t alphabet_size) noexcept {
        dfa dfa;
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
                if(corr_state == states.end()) {
                    states.push_back(og_successors);
                    if(accepting)
                        dfa.accepting_states.emplace(pos);
                }
                *--it = pos;
            }
        }
        return dfa;
    }

    dfa minimize(dfa const & old, std::size_t alphabet_size) noexcept {
        auto num_states = old.delta.size() / alphabet_size;
        std::vector<bool> distinct(num_states * num_states, false);
        auto pos = [&](auto i, auto j) { return i * num_states + j; }; // i < j

        for(;;) {
            bool change{};
            for(std::size_t i{}; i < num_states; ++i) {
                auto i_accepts = old.accepting_states.contains(i);
                for(auto j = i + 1; j < num_states; ++j) {
                    auto && ref = distinct[pos(i, j)];
                    if(ref)
                        continue;
                    if(i_accepts ^ old.accepting_states.contains(j)) {
                        ref = change = true;
                        continue;
                    }
                    for(auto k = alphabet_size; k--;) {
                        auto succ_i = old.delta[i * alphabet_size + k];
                        auto succ_j = old.delta[j * alphabet_size + k];
                        if(!distinct[succ_i < succ_j ? pos(succ_i, succ_j) : pos(succ_j, succ_i)])
                            continue;
                        ref = change = true;
                        break;
                    }
                }
            }
            if(!change)
                break;
        }
    
        std::vector<std::set<std::size_t>> states;
        std::ranges::copy(std::views::iota(std::size_t{}, num_states) | std::views::transform([](auto n) { return std::set{n}; }), std::back_inserter(states));
        for(std::size_t i{}; i < num_states; ++i) {
            for(auto j = i + 1; j < num_states; ++j) {
                if(distinct[pos(i, j)])
                    continue;
                auto is_set = std::ranges::find_if(states, [&](auto && s) { return s.contains(i); });
                auto js_set = std::ranges::find_if(states, [&](auto && s) { return s.contains(j); });
                if(is_set == js_set)
                    continue;
                is_set->merge(*js_set);
                states.erase(js_set);
            }
        }

        dfa dfa;
        dfa.delta.resize(states.size() * alphabet_size);
        for(std::size_t i{}; auto && state : states) {
            auto pos = i++ * alphabet_size;
            auto old_pos = *state.begin() * alphabet_size;
            for(auto c = alphabet_size; c--;)
                dfa.delta[pos + c] = std::ranges::find_if(states, [succ = old.delta[old_pos + c]](auto && s) { return s.contains(succ); }) - states.begin();
        }
        for(auto acc : old.accepting_states)
            dfa.accepting_states.emplace(std::ranges::find_if(states, [&](auto && s) { return s.contains(acc); }) - states.begin());
        return dfa;
    }

};

#endif // _AUTOMATA_HPP_

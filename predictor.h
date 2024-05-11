
#include <unordered_set>
// #include <multiset>
#include <vector>

#include "analyzer.h"
#include "card.h"
#include "hand.h"

class CombinationCalculator {
public:
    static std::vector<std::vector<CardValue_52_t>> calculate(const std::unordered_set<CardValue_52_t>& hands) {
        std::vector<CardValue_52_t> cards;

        for (auto i = 0; i < 52; ++i)
            if (hands.count(i) == 0)
                cards.push_back(i);

        // const auto n = cards.size();
        // const auto k = 5;
        std::vector<std::vector<CardValue_52_t>> combinations;
        std::vector<CardValue_52_t> combination;//(0);
        // combinations.reserve(numberOfCombinations(n, k));

        calculate(5, cards, combinations, combination);

        return combinations;
    }

private:
    static uint64_t numberOfCombinations(const unsigned n, const unsigned k) {
        auto factorial = [](unsigned begin, unsigned end) {
            uint64_t f = begin;
            for (auto i = begin + 1; i <= end; ++i)
                f *= i;
            return f;
        };
        return factorial(n-k, n) / factorial(1, k);
    }
    static void calculate(const unsigned k, const std::vector<CardValue_52_t>& cards, std::vector<std::vector<CardValue_52_t>>& combinations, 
                          std::vector<CardValue_52_t>& combination, unsigned index = 0)
    {
        if (combination.size() == k) {
            combinations.push_back(combination);
            return;
        }

        for (auto i = index; i < cards.size(); ++i) {
            combination.push_back(cards[i]);
            calculate(k, cards, combinations, combination, i + 1);
            combination.pop_back();
        }
    }
};

// class Predictor {
// public:
//     Predictor() = default;

//     void predict(std::vector<std::vector<CardValue_52_t>>& hands) {

//         std::unordered_set<CardValue_52_t> allHands;
//         for (auto& hand : hands)
//             for (auto card : hand)
//                 allHands.insert(card);
        
//         std::vector<CardValue_52_t> cards;
//         for (auto c = 0; c < 52; ++c)
//             if (allHands.count(c) == 0)
//                 cards.push_back(c);

//         // CombinationCalculator::calculate()

//     }
// };



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
        // const auto k = cards.size();
        std::vector<std::vector<CardValue_52_t>> combinations;
        std::vector<CardValue_52_t> combination;//(0);

        calculate(5, cards, combinations, combination);

        return combinations;
    }

private:
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

//     void predict(std::unordered_set<CardValue_52_t>& cards, std::vector<std::unique_ptr<Hand>>& hands, 
//                  std::unordered_set<CardValue_52_t>& tested) {
//         if (cards.size() == 7) {
//             Analyzer analyzer{};
//             hands.push_back(analyzer.analyze(cards));
//             return;
//         }

//         for (auto newCard = 0; newCard < 51; ++newCard) {
//             // if (tested.count(newCard) > 0)
//             //     continue;
//             if (cards.count(newCard) > 0)
//                 continue;
            
//             tested.insert(newCard);
//             cards.insert(newCard);
//             predict(cards, hands, tested);
//             cards.erase(newCard);
//         }
//     }
// };


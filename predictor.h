#include "analyzer.h"
#include "card.h"

#ifdef DEBUG
#include <chrono>
#include <iostream>
#endif
#include <vector>

class CombinationCalculator {
public:
    static std::vector<std::vector<CardValue_52_t>> calculate(const std::vector<CardValue_52_t>& cards, const unsigned selectionSize) {
        auto combinationNumber = numberOfCombinations(cards.size(), selectionSize);
        std::vector<std::vector<CardValue_52_t>> combinations;
        combinations.reserve(combinationNumber);
        std::vector<CardValue_52_t> combination;
        calculate(selectionSize, cards, combinations, combination);
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
        return factorial(n-k+1, n) / factorial(1, k);
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

class Predictor {
public:
    Predictor(IAnalyzer& analyzer) : m_analyzer{analyzer} {}

    void predict(const std::vector<std::vector<CardValue_52_t>>& playerHands) {
#ifdef DEBUG
        auto tstart = std::chrono::high_resolution_clock::now();
#endif

        auto cards = getAvailableCards(playerHands);
        auto combinations = CombinationCalculator::calculate(cards, 7 - playerHands[0].size());
        std::vector<unsigned> wins(playerHands.size(), 0);
        // std::vector<unsigned> ties(playerHands.size(), 0);

        for (auto& combination : combinations) {
            auto winner = comparePlayerHandsForCombination(playerHands, combination);
            if (winner >= 0)
                wins[winner] += 1;
        }

        for (auto p = 0; p < playerHands.size(); ++p) {
            std::cout << "player "<< p << ": " << 100. * wins[p] / combinations.size() << "%" << std::endl;
        }

#ifdef DEBUG
        auto tend = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(tend - tstart);
        std::cout << "combination analysis takes " << duration.count() << " ms\n";
#endif
    }

private:
    std::vector<CardValue_52_t> getAvailableCards(const std::vector<std::vector<CardValue_52_t>>& players) {
        std::vector<bool> deck(52, true);
        for (auto& player : players)
            for (auto card : player)
                deck[card] = false;

        std::vector<CardValue_52_t> cards;
        for (auto c = 0; c < deck.size(); ++c)
            if (deck[c])
                cards.push_back(c);
        
        return cards;
    }

    int comparePlayerHandsForCombination(const std::vector<std::vector<CardValue_52_t>>& players, 
                                          std::vector<CardValue_52_t>& combination) {
        std::unique_ptr<Hand> winningHand = std::make_unique<HighCard>(std::vector<CardValue_13_t>{5, 3, 2, 1, 0});
        std::vector<unsigned> winners;

        for (auto p = 0; p < players.size(); ++p) {
            auto& player = players[p];

            for (auto card : player)
                combination.push_back(card);

            auto hand = m_analyzer.analyze(combination);

            for (auto c = 0; c < player.size(); ++c)
                combination.pop_back();
            
            if (*winningHand < *hand) {
                winningHand = std::move(hand);
                winners.clear();
                winners.push_back(p);
            } else if (*winningHand == *hand) {
                winners.push_back(p);
            }
        }

        if (winners.size() == 1)
            return winners[0];
        return -1;
    }

    IAnalyzer&  m_analyzer;
};


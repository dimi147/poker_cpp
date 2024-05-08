#pragma once

#include "card.h"
#include "hand.h"

#include <array>
#include <memory>
#include <set>
#include <unordered_set>

class Analyzer {
public:
    std::unique_ptr<Hand> analyzeChar(const std::unordered_set<char>& cards) {
        std::vector<CardValue_52_t> _cards;
        for (auto card : cards)
            _cards.push_back(Card::valueIndex(card));
        return analyze(_cards);
    }
    
    std::unique_ptr<Hand> analyze(const std::vector<CardValue_52_t>& cards) {
        const auto breakdown = Breakdown(cards);
        return checkHand(breakdown);
    }

private:
    class Breakdown {
    public:
        Breakdown(const std::vector<CardValue_52_t>& cards) : cards{cards} {
            // valueCount.fill(0);
            for (auto& card : cards) {
                auto value = Card::value(card);
                auto suit = Card::suit(card);
                
                valueCount[value] += 1;
                suitCount[suit].insert(value);
                if (suitCount[suit].size() >= 5)
                    hasFlush = true;
            }

            for (auto value = 0; value < valueCount.size(); ++value) {
                auto count = valueCount[value];
                if (count > 0)
                    combinations[count-1].insert(value);
            }
        }

        bool hasFlush = false;
        std::array<CardValue_13_t, 13> valueCount{};
        std::array<std::set<CardValue_13_t>, 4> suitCount;
        std::array<std::set<CardValue_13_t>, 4> combinations;
        const std::vector<CardValue_52_t>& cards;
    };
    
    std::unique_ptr<Hand> checkHand(const Breakdown& breakdown) { return checkStraightFlush(breakdown); }
    
    std::unique_ptr<Hand> checkStraightFlush(const Breakdown& breakdown) {
        if (breakdown.hasFlush) {
            CardValue_13_t topCard = 0;
            CardSuit_t flushSuit = 0;

            for (auto i = 0; i < breakdown.suitCount.size(); ++i) {
                auto& suit = breakdown.suitCount[i];
                
                if (suit.size() < 5)
                    continue;
                
                flushSuit = i;
                
                for (auto card : suit) {
                    if (card == 12) {
                        if (suit.count(card - 12) &&
                            suit.count(card - 11) && 
                            suit.count(card - 10) &&
                            suit.count(card - 9))
                            topCard = std::max(topCard, (CardValue_13_t)(card - 9));
                    } else {
                        if (suit.count(card + 1) &&
                            suit.count(card + 2) &&
                            suit.count(card + 3) &&
                            suit.count(card + 4))
                            topCard = std::max(topCard, card);
                    }
                }
            }

            if (topCard > 0)
                return std::make_unique<StraightFlush>(topCard, flushSuit);
        }
        // for (auto& card : m_cards)
        // {
        //     auto value = Card::value(card);
        //     auto suit = Card::suit(card);
            
        //     if (value == 12) {
        //         if (cards.count(card-12))
        //             if (cards.count(card-11))
        //                 if (cards.count(card-10))
        //                     if (cards.count(card-9))
        //                         m_straightFlushes.insert(card-9);
        //     } else if (value < 9) {
        //         if (cards.count(card + 1) > 0)
        //             if (cards.count(card + 2) > 0)
        //                 if (cards.count(card + 3) > 0)
        //                     if (cards.count(card + 4) > 0) 
        //                         m_straightFlushes.insert(card + 4);
        //     }
        // }
        
        // if (straightflushes.empty())
        //     checkQuads();
        // else if (Card::value(*m_straightFlushes.rbegin()) == 12)
        //     result = Result::RoyalFlush;
        // else
        //     result = Result::Flush;

        // return checkPairs(breakdown);
        return checkQuads(breakdown);
    }
    
    std::unique_ptr<Hand> checkQuads(const Breakdown& breakdown) {
        if (breakdown.combinations[3].size() > 0)
            return std::make_unique<Quads>(*breakdown.combinations[3].begin());
        return checkFullHouse(breakdown);
    }
    
    std::unique_ptr<Hand> checkFullHouse(const Breakdown& breakdown) {
        if (breakdown.combinations[2].size() > 1 ||
            (breakdown.combinations[2].size() == 1 && breakdown.combinations[1].size() > 0)) {
            auto set = (*--breakdown.combinations[2].end());
            CardValue_13_t pair = 0;

            if (breakdown.combinations[1].size() > 0)
                pair = *breakdown.combinations[1].end();
            if (breakdown.combinations[2].size() > 1)
                pair = std::max(pair, *breakdown.combinations[2].begin());

            return std::make_unique<FullHouse>(set, pair);

        }
        return checkFlush(breakdown);
    }
    
    std::unique_ptr<Hand> checkFlush(const Breakdown& breakdown) {
        if (breakdown.hasFlush)
            for (auto suit = 0; suit < breakdown.suitCount.size(); ++suit) {
                auto cards = breakdown.suitCount[suit];
                if (cards.size() >= 5)
                    return std::make_unique<Flush>(cards, suit);
            }
        return checkStraight(breakdown);
    }
    
    std::unique_ptr<Hand> checkStraight(const Breakdown& breakdown) {
        CardValue_13_t straight = 0;
        
        for (auto value : breakdown.valueCount)
        {
            if (value == 12) {
                if (breakdown.valueCount[value-12] > 0)
                    if (breakdown.valueCount[value-11] > 0)
                        if (breakdown.valueCount[value-10] > 0)
                            if (breakdown.valueCount[value - 9] > 0)
                                straight = std::max(straight, (CardValue_13_t)(value - 9));
            } else if (value < 9) {
                if (breakdown.valueCount[value + 1] > 0)
                    if (breakdown.valueCount[value + 2] > 0)
                        if (breakdown.valueCount[value + 3] > 0)
                            if (breakdown.valueCount[value + 4] > 0)
                                straight = std::max(straight, value);
            }
        }

        if (straight > 0)
            return std::make_unique<Straight>(straight);

        return checkSet(breakdown);
    }
    
    std::unique_ptr<Hand> checkSet(const Breakdown& breakdown)
    {
        if (breakdown.combinations[2].size() > 0) {
            auto set = *breakdown.combinations[2].begin();
            auto it = breakdown.combinations[0].rbegin();
            auto k1 = *(it++);
            auto k2 = *it;
            return std::make_unique<Set>(set, k1, k2);
        }
        return checkPairs(breakdown);
    }
    
    std::unique_ptr<Hand> checkPairs(const Breakdown& breakdown) {
        if (breakdown.combinations[1].size() > 0)
            return std::make_unique<Pair>(breakdown.combinations[1], breakdown.combinations[0]);
        else 
            return std::make_unique<HighCard>(breakdown.valueCount);
    }
    
    // bool isHigher(uint8_t a, uint8_t b)
    // {
    //     auto _a = Card::value(a);
    //     auto _b = Card::value(b);
    //     return _a > _b;
    // }
};



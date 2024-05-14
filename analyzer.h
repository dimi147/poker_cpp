#pragma once

#include "card.h"
#include "hand.h"

#include <algorithm>
#include <array>
#include <memory>
// #include <set>
#include <unordered_set>

class IAnalyzer {
public:
    virtual ~IAnalyzer() = default;
    virtual std::unique_ptr<Hand> analyze(const std::vector<CardValue_52_t>& cards) = 0;

    std::unique_ptr<Hand> analyzeChar(const std::vector<std::string>& cards) {
        std::vector<CardValue_52_t> v;
        for (auto card : cards)
            v.push_back(Card::fromString(card));
        return analyze(v);
    }
};

class Analyzer : public IAnalyzer {
public:

    std::unique_ptr<Hand> analyze(const std::vector<CardValue_52_t>& cards) override {
        const auto breakdown = Breakdown(cards);
        return checkHand(breakdown);
    }

private:
    class Breakdown {
    public:
        Breakdown(const std::vector<CardValue_52_t>& cards) : cards{cards} {
            // valueCount.fill(0);
            std::array<unsigned, 4> suitCount{};

            for (auto& card : cards) {
                auto value = Card::value(card);
                auto suit = Card::suit(card);
                
                valueCount[value] += 1;
                suitCount[suit] += 1;
                if (suitCount[suit] >= 5) {
                    hasFlush = true;
                    flushSuit = suit;
                }
            }

            unsigned valueRecord = 0;
            for (auto value = 0; value < valueCount.size(); ++value) {
                auto count = valueCount[value];
                if (count == 4)
                    hasQuads = true;
                else if (count == 3)
                    sets += 1;
                else if (count == 2)
                    pairs += 1;
                
                valueRecord = (count > 0 ? valueRecord + 1 : 0);

                if (valueRecord >= 5) {
                    hasStraight = true;
                    straights.push_back(value);
                } else if (value == 3 && valueRecord == 4 && valueCount[12] > 0) {
                    hasStraight = true;
                    straights.push_back(value);
                }
            }
        }

        bool hasFlush = false;
        bool hasStraight = false;
        std::vector<CardValue_13_t> straights;
        std::array<CardValue_13_t, 13> valueCount{};
        // std::array<unsigned, 4> suitCount;
        CardSuit_t flushSuit = 0;
        bool hasQuads = false;
        unsigned sets = 0;
        unsigned pairs = 0;
        const std::vector<CardValue_52_t>& cards;
    };
    
    std::unique_ptr<Hand> checkHand(const Breakdown& breakdown) { 
        return checkStraightFlush(breakdown); 
    }
    
    std::unique_ptr<Hand> checkStraightFlush(const Breakdown& breakdown) {
        if (breakdown.hasFlush && breakdown.hasStraight) {

            // std::unordered_set<CardValue_52_t> cardSet;
            std::vector<bool> cardField(52, false);
            for (auto card : breakdown.cards)
                cardField[card] = true;

            CardValue_13_t topCard = 0;

            for (auto card : breakdown.cards) {
                auto value13 = Card::value(card);

                if (value13 <= 8) {
                    if (cardField[card + 1])
                        if (cardField[card + 2])
                            if (cardField[card + 3])
                                if (cardField[card + 4])
                                    topCard = std::max(topCard, (CardValue_13_t)(value13 + 4));
                } else if (value13 == 12) {
                    if (cardField[card - 12])
                        if (cardField[card - 11])
                            if (cardField[card - 10])
                                if (cardField[card - 9])
                                    topCard = std::max(topCard, CardValue_13_t{3});
                }
            }

            if (topCard > 0)
                return std::make_unique<StraightFlush>(topCard, breakdown.flushSuit);
        }

        return checkQuads(breakdown);
    }
    
    std::unique_ptr<Hand> checkQuads(const Breakdown& breakdown) {
        if (breakdown.hasQuads) {
            CardValue_13_t quads = 0;
            CardValue_13_t kicker = 0;

            for (int i = breakdown.valueCount.size(); i > 0; --i) {
                if (breakdown.valueCount[i] == 4)
                    quads = i;
                else if (breakdown.valueCount[i] > 0 && kicker == 0)
                    kicker = i;
            }
            return std::make_unique<Quads>(quads, kicker);
        }
        return checkFullHouse(breakdown);
    }
    
    std::unique_ptr<Hand> checkFullHouse(const Breakdown& breakdown) {
        if (breakdown.sets > 0 && breakdown.sets + breakdown.pairs > 1) {

            CardValue_13_t set = 0;
            CardValue_13_t pair = 0;
            for (int i = breakdown.valueCount.size(); i > 0; --i) {
                const auto count = breakdown.valueCount[i];
                if (count == 3 && set == 0)
                    set = i;
                else if (count > 1 && pair == 0)
                    pair = i;
            }

            return std::make_unique<FullHouse>(set, pair);

        }
        return checkFlush(breakdown);
    }
    
    std::unique_ptr<Hand> checkFlush(const Breakdown& breakdown) {
        if (breakdown.hasFlush) {
            std::vector<CardValue_13_t> cards;
            for (auto card : breakdown.cards) {
                if (Card::suit(card) == breakdown.flushSuit);
                    cards.push_back(Card::value(card));
            }

            std::sort(cards.begin(), cards.end(), [](auto& a, auto& b){ return a > b; });
            if (cards.size() > 5)
                cards.erase(cards.end() - 2, cards.end());

            return std::make_unique<Flush>(cards, breakdown.flushSuit);
        }

        return checkStraight(breakdown);
    }
    
    std::unique_ptr<Hand> checkStraight(const Breakdown& breakdown) {
        if (breakdown.hasStraight) {
            auto topCard = *breakdown.straights.rbegin();
            return std::make_unique<Straight>(topCard);
        }
        return checkSet(breakdown);
    }
    
    std::unique_ptr<Hand> checkSet(const Breakdown& breakdown)
    {
        if (breakdown.sets > 0) {
            CardValue_13_t set = 0;
            CardValue_13_t kicker1 = 0;
            CardValue_13_t kicker2 = 0;

            for (int i = breakdown.valueCount.size(); i > 0; --i) {
                const auto& count = breakdown.valueCount[i];
                if (count == 3)
                    set = i;
                else if (count == 1)
                    if (kicker1 == 0)
                        kicker1 = i;
                    else if (kicker2 == 0)
                        kicker2 = i;
            }

            return std::make_unique<Set>(set, kicker1, kicker2);
        }

        return checkPairs(breakdown);
    }
    
    std::unique_ptr<Hand> checkPairs(const Breakdown& breakdown) {
        if (breakdown.pairs > 0) {
            std::vector<CardValue_13_t> pairs;
            std::vector<CardValue_13_t> kickers;

            for (int i = breakdown.valueCount.size(); i > 0; --i) {
                const auto& count = breakdown.valueCount[i];
                if (count > 1 && pairs.size() < breakdown.pairs)
                    pairs.push_back(i);
                else if (count >= 1 && kickers.size() < 5 - (2 * breakdown.pairs))
                    kickers.push_back(i);
            }

            return std::make_unique<Pair>(pairs, kickers);
        }

        std::vector<CardValue_13_t> values;
        for (auto i = breakdown.valueCount.size(); values.size() < 5; --i)
            if (breakdown.valueCount[i] > 0)
                values.push_back(i);

        return std::make_unique<HighCard>(values);
    }

};



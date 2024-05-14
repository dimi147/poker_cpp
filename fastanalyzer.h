#include <iostream>
#include <chrono>
#include <bitset>
#include <vector>
#include <memory>

#include "analyzer.h"
#include "card.h"
#include "hand.h"

class FastAnalyzer : public IAnalyzer {
public:
    using Deck_t = uint64_t;
    using Suit_t = uint16_t;

    FastAnalyzer() = default;

    std::unique_ptr<Hand> analyze(const std::vector<CardValue_52_t>& cards) override {
        Deck_t deck = 0;
        for (auto card : cards)
            deck |= (Deck_t{1} << card);
        return analyze(deck);
    }

    std::unique_ptr<Hand> analyze(Deck_t deck) {
        auto suits = splitSuits(deck);
        auto merged = mergeSuits(suits);
        return checkAll(suits, merged);       
    }

    uint64_t toDeck(const std::vector<std::string>& cards) {
        uint64_t d = 0;
        for (auto card : cards)
            d |= (uint64_t{1} << Card::fromString(card));
        return d;
    }

    std::vector<std::string> fromDeck(uint64_t deck) {
        std::vector<std::string> cards;
        for (auto c = 0; c < 52; ++c)
            if (deck & (uint64_t{1} << c))
                cards.push_back(Card::toString(c));
        return cards;
    }

private:
    std::array<Suit_t, 4> splitSuits(Deck_t deck) {
        std::array<Suit_t, 4> suits{};
        for (auto s = 0; s < 4; ++s) {
            suits[s] = ((deck >> 13*s) & 0x1fff);
        }
        return suits;
    }

    Suit_t mergeSuits(const std::array<Suit_t, 4>& suits) {
        Suit_t merged = 0;
        for (auto suit : suits) {
            merged |= suit;
        }
        return merged;
    }

    std::unique_ptr<Hand> checkAll(const std::array<Suit_t, 4>& suits, Suit_t merged) {
        auto b = std::chrono::high_resolution_clock().now();
        
        std::array<unsigned, 13> values{};
        auto numquads = 0;
        auto numsets = 0;
        auto numpairs = 0;
        auto straightflush = -1;
        auto straight = -1;
        // auto flushvalue = -1;
        auto flushsuit = -1;

        for (auto s = 0; s < 4; s++) {
            auto suit = suits[s];
            auto count = 0;

            for (auto c = 0; c < 13; ++c) {

                if (c < 9) {
                    auto mask = (Suit_t{0x1f} << c);
                    if ((mask & suit) == mask)
                        straightflush = std::max(straightflush, c + 4);
                }

                if ((Suit_t{1} << c) & suit) // merge this with later block
                    count += 1;

                if (count >= 5)     // move this outside
                    flushsuit = s;

                auto mask = (Suit_t{1} << c);
                if (mask & suit) {
                    values[c] += 1;
                    if (values[c] == 4) {
                        numquads += 1;
                        numsets -= 1;
                    } else if (values[c] == 3) {
                        numsets += 1;
                        numpairs -= 1;
                    } else if (values[c] == 2) {
                        numpairs += 1;
                    }
                }
            }
            
            auto mask = (Suit_t{0x100f} << (s*13));
            if ((mask & suit) == mask)
                straightflush = std::max(straightflush, 3);
        }

        if (straightflush >= 0) {
            return std::make_unique<StraightFlush>(straightflush, flushsuit);

        } else if (numquads == 1) {
            auto value = -1;
            auto kicker = -1;
            for (int i = values.size() - 1; i >= 0; --i) {
                if (values[i] == 4)
                    value = i;
                else if (values[i] > 0 && kicker < 0)
                    kicker = i;
            }
            return std::make_unique<Quads>(value, kicker);
        } else if (numsets > 0 && numsets + numpairs > 1) {
            auto set = -1;
            auto pair = -1;
            for (int i = values.size() - 1; i >= 0; --i) {
                if (values[i] == 3 && set < 0)
                    set = i;
                else if (values[i] > 1 && pair < 0)
                    pair = i;
            }
            return std::make_unique<FullHouse>(set, pair);
        } else if (flushsuit >= 0) {
            std::vector<CardValue_13_t> cards;
            auto suit = suits[flushsuit];
            for (int i = 12; i >= 0; --i) {
                auto mask = (Suit_t{1} << i);
                if (suit & mask) {
                    cards.push_back(i);
                }
            }
            return std::make_unique<Flush>(cards, flushsuit);
        }

        for (auto c = 0; c < 13; ++c) {
            if (c < 9) {
                auto mask = (Suit_t{0x1f} << c);
                if ((mask & merged) == mask)
                    straight = std::max(straight, c + 4);
            }
        }

        auto mask = Suit_t{0x100f};
        if ((mask & merged) == mask)
            straight = std::max(straight, 3);

        if (straight >= 0) {
            return std::make_unique<Straight>(straight);
        } else if (numsets > 0) {
            CardValue_13_t set = 0;
            CardValue_13_t kicker1 = 0;
            CardValue_13_t kicker2 = 0;

            for (int i = values.size() - 1; i >= 0; --i) {
                if (values[i] == 3)
                    set = i;
                else if (values[i] == 1)
                    if (kicker1 == 0)
                        kicker1 = i;
                    else if (kicker2 == 0)
                        kicker2 = i;
            }

            return std::make_unique<Set>(set, kicker1, kicker2);
        } else if (numpairs > 0) {

            std::vector<CardValue_13_t> pairs;
            std::vector<CardValue_13_t> kickers;

            for (int i = values.size() - 1; i >= 0; --i) {
                const auto& count = values[i];
                if (count > 1 && pairs.size() < numpairs)
                    pairs.push_back(i);
                else if (count >= 1 && kickers.size() < 5 - (2 * numpairs))
                    kickers.push_back(i);
            }

            return std::make_unique<Pair>(pairs, kickers);

        } else {
            std::vector<CardValue_13_t> cards;
            for (int i = values.size() - 1; cards.size() < 5; --i)
                if (values[i] > 0)
                    cards.push_back(i);

            return std::make_unique<HighCard>(cards);
        }
        
        auto e = std::chrono::high_resolution_clock().now();
        std::cout << std::dec << std::chrono::duration_cast<std::chrono::nanoseconds>(e-b).count() << std::endl;
    }

    void checkFlush(Deck_t deck) {
        auto b = std::chrono::high_resolution_clock().now();

        for (auto s = 0; s < 4; ++s) {
            auto count = 0u;
            for (auto c = s*13; c < s*13+13; ++c) {
                if ((uint64_t{1} << c) & deck)
                    count += 1;
            }
            std::cout << "count " << count << std::endl;
        }

        auto e = std::chrono::high_resolution_clock().now();
        std::cout << std::dec << std::chrono::duration_cast<std::chrono::nanoseconds>(e-b).count() << std::endl;
    }

    int checkStraight(Suit_t suit) {
        auto b = std::chrono::high_resolution_clock().now();

        int straight = -1;
        
        for (auto c = 0; c < 9; ++c) {
            auto mask = (Suit_t{0x1f} << c);
            if ((mask & suit) == mask)
                straight = std::max(straight, c + 4);
                // std::cout << "straight " << c+4 << std::endl;
        }
        
        Suit_t mask = 0x100f;
        if ((mask & suit) == mask)
            straight = std::max(straight, 5);
            // std::cout << "straight " << 5 << '\n';
        
        auto e = std::chrono::high_resolution_clock().now();
        // std::cout << std::dec << std::chrono::duration_cast<std::chrono::nanoseconds>(e-b).count() << std::endl;

        return straight;
    }

    int checkCount(const std::array<Suit_t, 4>& suits) {
        std::array<unsigned, 13> count{};
        unsigned maxIndex = 0;
        for (auto c = 0; c < 13; ++c) {
            auto mask = Suit_t{1} << c;
            for (auto suit : suits)
                count[c] += (mask & suit);
            if (count[maxIndex] < count[c])
                maxIndex = c;
        }
    }
};

// void test() {
    // Deck_t d  = 0b0000000000001111111001111000000000000000000000000000000000000000;
    // Deck_t d2 = 0b0000000000000000000011000000000000010000000000000100000000000001;

// }

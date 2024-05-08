#pragma once

#include "card.h"

#include <set>
#include <string>
// #include <vector>

class Hand {
public:
    enum Rank {
        HighCard = 0,
        Pair,
        TwoPair,
        Set,
        Straight,
        Flush,
        FullHouse,
        Quads,
        StraightFlush
    };

    Rank getRank() { return m_rank; }

    virtual ~Hand() = default;
    virtual std::string asString() = 0;

    bool operator==(const Hand& other) {
        if (m_rank == other.m_rank)
            return isEquivalent(other);
        return false;
    }
    bool operator<(const Hand& other) {
        if (m_rank == other.m_rank)
            return isWeaker(other);
        return m_rank < other.m_rank;
    }
    bool operator!=(const Hand& other) {
        return !operator==(other);
    }
    bool operator>(const Hand& other) {
        return !operator==(other) && !operator<(other);
    }

protected:
    Hand(const Rank& rank) : m_rank{rank} {}

    virtual bool isWeaker(const Hand& other) = 0;
    virtual bool isEquivalent(const Hand& other) = 0;

    Rank m_rank;
};

class StraightFlush : public Hand {
public:
    StraightFlush(CardValue_13_t top, CardSuit_t suit) :
        Hand{Rank::StraightFlush}, m_top{top}, m_suit{suit} 
    {}

    std::string asString() override {
        std::string str;
        return str;
    }

private:
    bool isWeaker(const Hand& other) override {
        return false;
    }

    bool isEquivalent(const Hand& other) override {
        return false;
    }

    const CardValue_13_t m_top;
    const CardSuit_t m_suit;
};

class Quads : public Hand {
public:
    Quads(CardValue_13_t value) : Hand{Rank::Quads}, m_value{value}
    {}

    std::string asString() override {
        std::string str;
        str += "Quads: ";
        str += std::to_string(m_value);
        return str;
    }

private:
    bool isWeaker(const Hand& other) override {
        const auto& other_ = static_cast<const Quads&>(other);
        return m_value < other_.m_value;
    }

    bool isEquivalent(const Hand& other) override {
        const auto& other_ = static_cast<const Quads&>(other);
        return m_value == other_.m_value;
    }

    const CardValue_13_t m_value;
};

class FullHouse : public Hand {
public:
    FullHouse(CardValue_13_t set, CardValue_13_t pair) 
        : Hand{Rank::FullHouse}, m_set{set}, m_pair{pair}
    {}

    std::string asString() override {
        std::string str{"Full house: "};
        return str;
    }

private:
    bool isWeaker(const Hand& other) override {
        return false;
    }

    bool isEquivalent(const Hand& other) override {
        return false;
    }

    const CardValue_13_t m_set;
    const CardValue_13_t m_pair; 
};

class Flush : public Hand {
public:
    Flush(std::set<CardValue_13_t> cards, CardSuit_t suit) 
        : Hand{Rank::Flush}, m_cards{cards}, m_suit{suit} 
    {}

    std::string asString() override {
        std::string str{"flush"};
        return str;
    }

private:
    bool isWeaker(const Hand& other) override {
        return false;
    }

    bool isEquivalent(const Hand& other) override {
        return false;
    }

    const std::set<CardValue_13_t> m_cards;
    const CardSuit_t m_suit;
};

class Straight : public Hand {
public:
    Straight(CardValue_13_t top) :
        Hand{Rank::Straight}, m_top{top} 
    {}

    std::string asString() override {
        std::string str{"straight"};
        return str;
    }

private:
    bool isWeaker(const Hand& other) override {
        return false;
    }

    bool isEquivalent(const Hand& other) override {
        return false;
    }

    const CardValue_13_t m_top;
};

class Set : public Hand {
public:
    Set(CardValue_13_t value, CardValue_13_t k1, CardValue_13_t k2) 
        : Hand{Rank::Set}, m_value{value}, m_k1{k1}, m_k2{k2}
    {}

    std::string asString() override {
        std::string str{"set"};
        return str;
    }

private:
    bool isWeaker(const Hand& other) override {
        return false;
    }

    bool isEquivalent(const Hand& other) override {
        return false;
    }

    CardValue_13_t m_value;
    CardValue_13_t m_k1;
    CardValue_13_t m_k2;
};

class Pair : public Hand {
public:
    Pair(std::set<uint8_t> pairs, std::set<uint8_t> kickers) 
        : Hand(pairs.size() > 1 ? Rank::TwoPair : Rank::Pair), m_pairs{pairs}, m_kickers{kickers}
    {}

private:
    std::string asString() override {
        std::string str;
        if (m_pairs.size() == 2)
            str.append("Two pair: ");
        else 
            str.append("Pair: ");

        for (auto it = m_pairs.rbegin(); it != m_pairs.rend(); ++it) {
            str.push_back(Card::cvalue(*it));
            str.push_back(' ');
        }

        str.append("+ ");
        // for (auto i = 0; i < (m_pairs.size() > 1 ? 1 : 3); ++i) {
        //     str.push_back(Card::cvalue())
        // }
        for (auto it = m_kickers.rbegin(); it != m_kickers.rend(); ++it) {
            str.push_back(Card::cvalue(*it));
            str.push_back(' ');
        }

        return str;
    }

    bool isWeaker(const Hand& other) override {
        auto& otherpair = static_cast<const Pair&>(other);
        if (m_pairs.size() < otherpair.m_pairs.size())
            return true;
        
        for (auto it = m_pairs.rbegin(), it2 = otherpair.m_pairs.rbegin(); it != m_pairs.rend();) {
            if (*it != *it2)
                return *it < *it2;
        }

        for (auto it = m_kickers.rbegin(), it2 = otherpair.m_kickers.rbegin(); it != m_kickers.rend();) {
            if (*it != *it2)
                return *it < *it2;
        }

        return false;
    }

    bool isEquivalent(const Hand& other) override {
        const auto& otherpair = static_cast<const Pair&>(other);
        if (m_pairs.size() != otherpair.m_pairs.size())
            return false;
        
        for (auto it = m_pairs.rbegin(), it2 = otherpair.m_pairs.rbegin(); it != m_pairs.rend();) {
            if (*it != *it2)
                return false;
        }

        for (auto it = m_kickers.rbegin(), it2 = otherpair.m_kickers.rbegin(); it != m_kickers.rend();) {
            if (*it != *it2)
                return false;
        }

        return true;
    }

    std::set<CardValue_13_t> m_pairs;
    std::set<CardValue_13_t> m_kickers;
};

class HighCard : public Hand {
public:
    HighCard(std::array<uint8_t, 13> valueCount) 
        : Hand{Rank::HighCard}, m_valueCount{valueCount}
    {}

private:
    std::string asString() override { 
        std::string str{"high card: "};
        auto it = m_valueCount.rbegin();
        for (auto i = 0; i < 5; ++i) {
            str.push_back(Card::cvalue(Card::value(*it++)));
            str.push_back(' ');
        }
        return str;
     }

     bool isEquivalent(const Hand& other) override {
        const auto& _other = static_cast<const HighCard&>(other);
        return false;
     }

     bool isWeaker(const Hand& other) override {
        return false;
     }

    std::array<uint8_t, 13> m_valueCount;
};

#pragma once

#include "card.h"

// #include <set>
#include <string>
#include <vector>

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

    virtual ~Hand() = default;

    Rank getRank() { return m_rank; }
    
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
        std::string str{"Straight Flush: "};
        str = str + Card::cvalue(m_top) + " of " + Card::ssuit(m_suit);
        return str;
    }

private:
    bool isWeaker(const Hand& other) override {
        const auto& other_ = static_cast<const StraightFlush&>(other);
        return m_top < other_.m_top;
    }

    bool isEquivalent(const Hand& other) override {
        const auto& other_ = static_cast<const StraightFlush&>(other);
        return m_top == other_.m_top;
    }

    const CardValue_13_t m_top;
    const CardSuit_t m_suit;
};

class Quads : public Hand {
public:
    Quads(CardValue_13_t value, CardValue_13_t kicker) 
        : Hand{Rank::Quads}, m_value{value}, m_kicker{kicker}
    {}

    std::string asString() override {
        std::string str{"Quad "};
        str = str + Card::cvalue(m_value) + "s + " + Card::cvalue(m_kicker) + " kicker";
        return str;
    }

private:
    bool isWeaker(const Hand& other) override {
        const auto& other_ = static_cast<const Quads&>(other);
        if (m_value == other_.m_value)
            return m_kicker < other_.m_kicker;
        return m_value < other_.m_value;
    }

    bool isEquivalent(const Hand& other) override {
        const auto& other_ = static_cast<const Quads&>(other);
        if (m_value == other_.m_value)
            return m_kicker == other_.m_kicker;
        return false;
    }

    const CardValue_13_t m_value;
    const CardValue_13_t m_kicker;
};

class FullHouse : public Hand {
public:
    FullHouse(CardValue_13_t set, CardValue_13_t pair) 
        : Hand{Rank::FullHouse}, m_set{set}, m_pair{pair}
    {}

    std::string asString() override {
        std::string str{"Full house: "};
        str = str + Card::cvalue(m_set) + "s full of " + Card::cvalue(m_pair) + "s";
        return str;
    }

private:
    bool isWeaker(const Hand& other) override {
        const auto& other_ = static_cast<const FullHouse&>(other);
        if (m_set == other_.m_set)
            return m_pair < other_.m_pair;
        return m_set < other_.m_set;
    }

    bool isEquivalent(const Hand& other) override {
        const auto& other_ = static_cast<const FullHouse&>(other);
        if (m_set == other_.m_set)
            return m_pair == other_.m_pair;
        return false;
    }

    const CardValue_13_t m_set;
    const CardValue_13_t m_pair; 
};

class Flush : public Hand {
public:
    Flush(const std::vector<CardValue_13_t>& cards, CardSuit_t suit) 
        : Hand{Rank::Flush}, m_cards{cards}, m_suit{suit} 
    {}

    std::string asString() override {
        std::string str{"Flush: "};
        str = str + Card::cvalue(m_cards[4]) + ", "  + Card::cvalue(m_cards[3]) + ", " 
              + Card::cvalue(m_cards[2]) + ", "  + Card::cvalue(m_cards[1]) + ", " 
              + Card::cvalue(m_cards[0]);
        return str;
    }

private:
    bool isWeaker(const Hand& other) override {
        const auto& other_ = static_cast<const Flush&>(other);
        for (int c = 0; c < 5; ++c)
            if (m_cards[c] != other_.m_cards[c])
                return m_cards[c] < other_.m_cards[c];
        return false;
    }

    bool isEquivalent(const Hand& other) override {
        const auto& other_ = static_cast<const Flush&>(other);
        for (int c = 0; c < 5; ++c)
            if (m_cards[c] != other_.m_cards[c])
                return false;
        return true;
    }

    const std::vector<CardValue_13_t> m_cards;
    const CardSuit_t m_suit;
};

class Straight : public Hand {
public:
    Straight(CardValue_13_t top) :
        Hand{Rank::Straight}, m_top{top} 
    {}

    std::string asString() override {
        std::string str{"Straight to "};
        str = str + Card::cvalue(m_top);
        return str;
    }

private:
    bool isWeaker(const Hand& other) override {
        const auto& other_ = static_cast<const Straight&>(other);
        return m_top < other_.m_top;
    }

    bool isEquivalent(const Hand& other) override {
        const auto& other_ = static_cast<const Straight&>(other);
        return m_top == other_.m_top;
    }

    const CardValue_13_t m_top;
};

class Set : public Hand {
public:
    Set(CardValue_13_t value, CardValue_13_t kicker1, CardValue_13_t kicker2) 
        : Hand{Rank::Set}, m_value{value}, m_kicker1{kicker1}, m_kicker2{kicker2}
    {}

    std::string asString() override {
        std::string str{"Set of "};
        str = str + Card::cvalue(m_value) + "s + " + Card::cvalue(m_kicker1) + ", "
              + Card::cvalue(m_kicker2);
        return str;
    }

private:
    bool isWeaker(const Hand& other) override {
        const auto& other_ = static_cast<const Set&>(other);
        if (m_value != other_.m_value)
            return m_value < other_.m_value;
        else if (m_kicker1 != other_.m_kicker1)
            return m_kicker1 < other_.m_kicker1;
        else 
            return m_kicker2 < other_.m_kicker2;
    }

    bool isEquivalent(const Hand& other) override {
        const auto& other_ = static_cast<const Set&>(other);
        if (m_value != other_.m_value)
            return false;
        else if (m_kicker1 != other_.m_kicker1)
            return false;
        else 
            return m_kicker2 == other_.m_kicker2;
    }

    CardValue_13_t m_value;
    CardValue_13_t m_kicker1;
    CardValue_13_t m_kicker2;
};

class Pair : public Hand {
public:
    Pair(const std::vector<CardValue_13_t>& pairs, const std::vector<CardValue_13_t>& kickers) 
        : Hand(pairs.size() > 1 ? Rank::TwoPair : Rank::Pair), m_pairs{pairs}, m_kickers{kickers}
    {}

private:
    std::string asString() override {
        std::string str{m_pairs.size() > 1 ? "Two pair: " : "Pair of "};
        str = str + Card::cvalue(m_pairs[0]) + "s";
        if (m_pairs.size() > 1)
            str = str + " and " + Card::cvalue(m_pairs[1]) + "s";
        str = str + " + " + Card::cvalue(m_kickers[0]);
        if (m_kickers.size() > 1)
            str = str + ", " + Card::cvalue(m_kickers[1]) + ", " + Card::cvalue(m_kickers[2]);
        return str;
    }

    bool isWeaker(const Hand& other) override {
        auto& otherpair = static_cast<const Pair&>(other);
        if (m_pairs.size() < otherpair.m_pairs.size())
            return true;
        
        for (auto i = 0; i < m_pairs.size(); ++i) {
            if (m_pairs[i] != otherpair.m_pairs[i])
                return m_pairs[i] < otherpair.m_pairs[i];
        }

        for (auto i = 0; i < m_kickers.size(); ++i) {
            if (m_kickers[i] != otherpair.m_kickers[i])
                return m_kickers[i] < otherpair.m_kickers[i];
        }

        return false;
    }

    bool isEquivalent(const Hand& other) override {
        const auto& otherpair = static_cast<const Pair&>(other);
        if (m_pairs.size() != otherpair.m_pairs.size())
            return false;
        
        for (auto it = m_pairs.rbegin(), it2 = otherpair.m_pairs.rbegin(); it != m_pairs.rend();) {
            if (*it++ != *it2++)
                return false;
        }

        // if (m_kickers.size() != otherpair.m_kickers.size())
        //     return false;

        for (auto it = m_kickers.rbegin(), it2 = otherpair.m_kickers.rbegin(); it != m_kickers.rend();) {
            if (*it++ != *it2++)
                return false;
        }

        return true;
    }

    const std::vector<CardValue_13_t> m_pairs;
    const std::vector<CardValue_13_t> m_kickers;
};

class HighCard : public Hand {
public:
    HighCard(const std::vector<CardValue_13_t>& values) 
        : Hand{Rank::HighCard}, m_values{values}
    {}

private:
    std::string asString() override { 
        std::string str{"High card: "};
        str = str + Card::cvalue(m_values[0]) + Card::cvalue(m_values[1])
              + Card::cvalue(m_values[2]) + Card::cvalue(m_values[3])
              + Card::cvalue(m_values[4]);
        return str;
     }

     bool isWeaker(const Hand& other) override {
        const auto& other_ = static_cast<const HighCard&>(other);
        for (auto c = 0; c < 5; ++c) {
            if (m_values[c] != other_.m_values[c])
                return m_values[c] < other_.m_values[c];
        }
        return false;
     }

     bool isEquivalent(const Hand& other) override {
        const auto& other_ = static_cast<const HighCard&>(other);
        for (auto c = 0; c < 5; ++c) {
            if (m_values[c] != other_.m_values[c])
                return false;
        }
        return true;
     }

    std::vector<CardValue_13_t> m_values;
};

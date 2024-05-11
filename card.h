#pragma once

#include <array>
#include <string>

using CardValue_52_t = uint8_t;
using CardValue_13_t = uint8_t;
using CardSuit_t = uint8_t;

class Card
{
public:
    static CardValue_52_t fromString(const std::string& str) {
        CardValue_52_t value = 0;
        value += std::find(values.begin(), values.end(), str[0]) - values.begin();
        auto suitChecker = [&str](auto& suit) { return suit[0] == str[1]; };
        value += (std::find_if(suits.begin(), suits.end(), suitChecker) - suits.begin()) * 13;
        return value;
    }
    static std::string toString(int c)
    {
        std::string code;
        code.push_back(values[value(c)]);
        code += suits[suit(c)][0];
        return code;
    }
    static CardSuit_t suit(CardValue_52_t c) { return c / 13; }
    static CardValue_13_t value(CardValue_52_t c) { return c % 13; }
    static char cvalue(CardValue_13_t v) { return values[v]; }
    static std::string ssuit(CardSuit_t s) { return suits[s]; }
    
private:
    static constexpr std::array<const char*, 4> suits = {"diamonds", "hearts", "spades", "clubs"};
    static constexpr std::array<char, 13> values = {'2','3', '4','5','6','7','8','9','T','J','Q', 'K','A'};
};
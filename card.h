#pragma once

#include <array>
#include <string>

using CardValue_52_t = uint8_t;
using CardValue_13_t = uint8_t;
using CardSuit_t = uint8_t;

class Card
{
public:
    static std::string code(int c)
    {
        std::string code;
        code.push_back(values[value(c)]);
        code += suits[suit(c)][0];
        return code;
    }
    static uint8_t suit(int c) { return c / 13; }
    static uint8_t value(int c) { return c % 13; }
    static char cvalue(int v) { return values[v]; }
    static std::string ssuit(int s) { return suits[s]; }
    static unsigned valueIndex(char c) { 
        return std::find(values.begin(), values.end(), c) - values.begin();
    }
    // static unsigned suitIndex(char c) {
    //     return std::find(suits.begin(), suits.end(), [c](auto suit){ return true; }) - suits.begin();
    // }
    
private:
    static constexpr std::array<const char*, 4> suits = {"diamonds", "hearts", "spades", "clubs"};
    static constexpr std::array<char, 13> values = {'2','3', '4','5','6','7','8','9','T','J','Q', 'K','A'};
};
#include <array>

#include "card.h"

class Deck {
public:
    Deck() { m_cards.fill(true); }
    ~Deck() = default;

    CardValue_52_t deal() {
        CardValue_52_t rand = std::rand() % (52 - m_dealt);
        CardValue_52_t index = 0;
        
        while (m_cards[index] == false)
            index++;

        while (rand) {
            if (m_cards[index] == true)
                rand--;
            ++index;
        }

        m_cards[index] = false;
        m_dealt += 1;
        return index;
    }

    void muck(CardValue_52_t card) {
        m_cards[card] = true;
        m_dealt -= 1;
    }

    void reset() {
        m_cards.fill(true);
        m_dealt = 0;
    }

private:
    std::array<bool, 52> m_cards;
    unsigned m_dealt = 0;
};


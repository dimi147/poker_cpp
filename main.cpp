
#include "analyzer.h"
#include "card.h"
#include "fastanalyzer.h"
#include "predictor.h"
// #include "deck.h"

#include <chrono>
#include <iostream>
#include <assert.h>

void testHandComparison() {
    assert(StraightFlush(11, 0) < StraightFlush(12, 0));
    assert(StraightFlush(7, 0) == StraightFlush(7, 1));
    assert(Quads(12, 11) < StraightFlush(0, 0));
    assert(Quads(12, 10) < Quads(12, 11));
    assert(Quads(11, 12) == Quads(11, 12));
    assert(FullHouse(12, 11) < Quads(0, 1));
    assert(FullHouse(12, 10) < FullHouse(12, 11));
    assert(FullHouse(7, 3) == FullHouse(7, 3));
    assert(Flush({12, 11, 10, 9, 7}, 0) < FullHouse(0, 1));
    assert(Flush({12, 11, 9, 8, 1}, 0) == Flush({12, 11, 9, 8, 1}, 0));
    assert(Flush({11, 10, 9, 8, 1}, 0) < Flush({12, 11, 9, 8, 1}, 0));
    assert(Flush({12, 11, 9, 8, 0}, 0) < Flush({12, 11, 9, 8, 1}, 0));
    assert(Straight(12) < Flush({0, 1, 2, 3, 5}, 0));
    assert(Straight(7) < Straight(8));
    assert(Straight(7) == Straight(7));
    assert(Set(12, 11, 10) < Straight(3));
    assert(Set(12, 11, 9) < Set(12, 11, 10));
    assert(Set(7, 9, 5) == Set(7, 9, 5));
    assert(Pair({12, 11}, {10}) < Set(0, 1, 2));
    assert(Pair({7, 5}, {11}) < Pair({7, 5}, {12}));
    assert(Pair({7, 5}, {12}) == Pair({7, 5}, {12}));
    assert(Pair({12}, {11, 10, 9}) < Pair({0, 1}, {2}));
    assert(Pair({12}, {11, 10, 8}) < Pair({12}, {11, 10, 9}));
    assert(Pair({7}, {10, 9, 8}) == Pair({7}, {10, 9, 8}));
    assert(HighCard({12, 11, 10, 9, 7}) < Pair({0}, {3, 2, 1}));
    assert(HighCard({12, 11, 10, 7, 4}) < HighCard({12, 11, 10, 7, 5}));
    assert(HighCard({7, 5, 4, 2, 1}) == HighCard({7, 5, 4, 2, 1}));
}

void testAnalzerWithExampleCombinations(IAnalyzer& analyzer) {
    assert(StraightFlush(5, 0) == *analyzer.analyzeChar({"Ad", "2d", "3d", "4d", "5d", "6d", "7d"}));
    assert(StraightFlush(12, 0) == *analyzer.analyzeChar({"Ad", "Kd", "Qd", "Jd", "Td", "6d", "7d"}));
    assert(Quads(5, 12) == *analyzer.analyzeChar({"Ad", "2d", "3d", "7c", "7h", "7s", "7d"}));
    assert(FullHouse(5, 1) == *analyzer.analyzeChar({"Ad", "2d", "3d", "3c", "7h", "7s", "7d"}));
    assert(FullHouse(5, 12) == *analyzer.analyzeChar({"Ad", "Ah", "3d", "3c", "7h", "7s", "7d"}));
    assert(FullHouse(12, 5) == *analyzer.analyzeChar({"Ad", "Ah", "Ac", "3c", "7h", "7s", "7d"}));
    assert(Flush({12, 6, 5, 4, 2}, 2) == *analyzer.analyzeChar({"Ac", "6c", "3d", "3c", "7c", "8c", "4c"}));
    assert(Straight(3) == *analyzer.analyzeChar({"Ac", "2d", "3d", "4h", "5s", "8c", "4c"}));
    assert(Straight{4} == *analyzer.analyzeChar({"6c", "2d", "3d", "4h", "5s", "8c", "4c"}));
    assert(Straight{12} == *analyzer.analyzeChar({"Ac", "8d", "Td", "Jh", "Qs", "Kc", "9c"}));
    assert(Set(5, 12, 7) == *analyzer.analyzeChar({"Ad", "2d", "3d", "7c", "7h", "7s", "9c"}));
    assert(Pair({7, 5}, {12}) == *analyzer.analyzeChar({"Ad", "2d", "3d", "7c", "7h", "9h", "9c"}));
    assert(Pair({5}, {12, 8, 7}) == *analyzer.analyzeChar({"Ad", "2d", "3d", "7c", "7h", "9h", "Tc"}));
    assert(HighCard({12, 11, 8, 7, 5}) == *analyzer.analyzeChar({"Ad", "2d", "3d", "7c", "Kh", "9h", "Tc"}));
}

void testAnalyzers() {
    Analyzer analyzer{};
    FastAnalyzer fast{};
    testAnalzerWithExampleCombinations(analyzer);
    testAnalzerWithExampleCombinations(fast);
}

int main() {
    testHandComparison();
    testAnalyzers();

    FastAnalyzer fast{};
    Predictor predictor{fast};
    predictor.predict({{12+13, 12}, {0, 5 + 13}, {11, 11+13}, {10, 10+13}, {4, 5}});
    predictor.predict({{4, 12}, {2,3}});

    return 0;
}
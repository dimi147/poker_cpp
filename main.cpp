
#include "analyzer.h"
#include "card.h"
#include "predictor.h"
#include "deck.h"

#include "fastanalyzer.h"

#include <chrono>
#include <iostream>
#include <assert.h>
// #include <unordered_set>

void test() {
    FastAnalyzer analyzer{};

    // auto check = [](auto& a, auto& b) {
    //     std::cout << (a==b) << std::endl;
    // };

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

int main() {
    test();
    FastAnalyzer analyzer{};

    auto b = std::chrono::high_resolution_clock::now();
    auto combinations = CombinationCalculator::calculate({0, 1});
    auto e = std::chrono::high_resolution_clock::now();

    std::cout << "combinations take " << std::chrono::duration_cast<std::chrono::milliseconds>(e - b).count() << " ms\n";

    std::cout << combinations.size() << std::endl;

    auto m = std::array<unsigned, 9>{};

    b = std::chrono::high_resolution_clock::now();
    for (auto combination : combinations) {
        combination.push_back(0);
        combination.push_back(1);
        auto hand = analyzer.analyze(combination);

        m[hand->getRank()] += 1;
        

        // combination.analyze(combination);
    }
    e = std::chrono::high_resolution_clock::now();
    std::cout << "combination analysis takes " << std::chrono::duration_cast<std::chrono::milliseconds>(e - b).count() << " ms\n";

    auto i=0;
    for (auto r : m)
        printf("rank %d: %lf %%\n", i++, 100. * r/combinations.size());
    // std::cout << (*hand1 > *hand2);

    return 0;
}

#include "analyzer.h"
#include "card.h"
#include "predictor.h"

#include <chrono>
#include <iostream>
// #include <unordered_set>

int main() {
    
    // std::srand(std::time(nullptr)); // use current time as seed for random generator
    Analyzer analyzer{};

    // while (cards.size() < 7)
    // {
    //     auto it = cards.insert(std::rand() % 52);
    //     if (it.second)
    //         std::cout << Card::code(*it.first) << ",";
    // }
    
    auto hand1 = analyzer.analyze({0, 1, 2, 2 + 13, 4 + 13, 11, 12});
    auto hand2 = analyzer.analyze({5, 5+13, 6, 7, 12 + 26, 11+26, 3 + 39});
    // auto hand3 = analyzer.analyzeChar({'A'});

    std::cout << hand1->asString() << std::endl;
    std::cout << hand2->asString() << std::endl;

    // Predictor predictor{};
    // std::unordered_set<CardValue_52_t> potential{0, 1, 2, 3}, tested;
    // std::vector<std::unique_ptr<Hand>> hands;
    // predictor.predict(potential, hands, tested);

    auto b = std::chrono::high_resolution_clock::now();
    auto combinations = CombinationCalculator::calculate({0, 1});
    auto e = std::chrono::high_resolution_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(e - b).count() << " us\n";

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
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(e - b).count() << " us\n";

    auto i=0;
    for (auto r : m)
        printf("rank %d: %lf %%\n", i++, 100. * r/combinations.size());
    // std::cout << (*hand1 > *hand2);

    return 0;
}
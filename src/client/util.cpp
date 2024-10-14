#include <random>
#include "util.hpp"

int DPRNG::generate(int min, int max) {
    return std::uniform_int_distribution<int>{min, max}(gen);
}

class DPRNG {
private:
    std::mt19937 gen;

public:
    DPRNG(unsigned int seed) : gen(seed) {}
    int generate(int min, int max);
};

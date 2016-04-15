/*
 * Test of Step class
 */

#include "Step.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

using namespace std;

template<typename T>
void print(const T &ref)
{
    for (auto val : ref) {
        cout << val << ' ';
    }
}
void print(const Step &m)
{
    cout << "Size: " << m.Hi() << "x" << m.Hi() << endl;
    for (size_t i = 0; i < m.Hi(); ++i) {
        for (size_t j = 0; j < m.Hi(); ++j) {
            cout << m(m.global(i,j)) << " ";
        }
        cout << endl;
    }
}

class StepTest : public Step {
public:
    template <typename ...Args>
    StepTest(Args ...args) : Step(args...)
    {}
    double calc(const position &pos)
    {
        return at(pos) + step();
    }
};

int main()
{
    #define N 10
    #define M 20
    #define Hi 5
    #define Hj 10
    #define Steps 10
    #define Count N/Hi * M/Hj

    array<Step *, Count> net;

    for (size_t c = 0; c < Count; ++c) {
        net[c] = new StepTest(2, c, N, M, Hi, Hj);
    }
    for (size_t s = 0; s < Steps; ++s) {
        cout << endl << endl << "========================================";
        cout << endl << "Step " << s << endl;
        for (size_t c = 0; c < Count; ++c) {
            net[c]->next();
            print (*net[c]);
        }
    }
    for (size_t c = 0; c < Count; ++c) {
        delete net[c];
    }

    return 0;

};


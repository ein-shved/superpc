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
        for (size_t j = 0; j < m.Hj(); ++j) {
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

template <typename T> class SimpleNet;
class SimpleStep : public Step {
public:
    template <typename ...Args>
    SimpleStep(SimpleNet<SimpleStep> &net, Args ...args)
        : Step(args...)
        , m_net(net)
    {}
    double calc(const position &pos);
private:
    SimpleNet<SimpleStep> &m_net;
};
template <typename T>
class SimpleNet {
public:
    template <typename ...Args>
    SimpleNet(size_t N, size_t M, size_t Hi, size_t Hj, Args... args)
    {
        Net<> tmp(0, N, M, Hi, Hj);
        m_length = tmp.Nc() * tmp.Mc();
        for (size_t i = 0; i<m_length; ++i) {
            m_net.push_back(new T(*this, 2, i, N, M, Hi, Hj, args...));
        }
        m_net[0]->at(0, 0) = 1;
    }
    ~SimpleNet() {
        for (auto s : m_net) {
            delete s;
        }
    }
    void next() {
        for (int i = m_length - 1; i >= 0; --i) {
            m_net[i]->next();
        }
    }
    template<typename ...Args>
    double at(Args ... pos) const {
        size_t i = (*m_net[0])->global_index(pos...);
        return (*m_net[i]).at(pos...);
    }
    void print () const {
        for (size_t i = 0; i < m_net[0]->N(); ++i) {
            for (size_t j = 0; j < m_net[0]->M(); ++j) {
                cout.width(5);
                cout << long(at(i,j)) << " ";
            }
            cout << endl;
        }
    }
private:
    vector<T *> m_net;
    size_t m_length;
};
double SimpleStep::calc(const position &pos)
{
    double result;
    result = m_net.at(pos);
    if (pos.first > 0) {
        result += m_net.at(pos.first - 1, pos.second);
    }
    if (pos.second > 0) {
        result += m_net.at(pos.first, pos.second - 1);
    }
    if (pos.first > 0 && pos.second > 0) {
        result += m_net.at(pos.first - 1, pos.second - 1);
    }
    return (long)result % 100000;

}

    #define N 10
    #define M 20
    #define Hi 5
    #define Hj 10
    #define Steps 20
    #define Count (N/Hi * M/Hj)
static void arr()
{
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


}
static void net()
{
    SimpleNet<SimpleStep> net(N, M, Hi, Hj);
    net.print();
    for (size_t s = 0; s < Steps; ++s) {
        cout << endl << endl << "========================================";
        cout << endl << "Step " << s << endl;
        net.next();
        net.print();
    }
}
int main()
{
    arr();
    cout << endl << endl << endl;
    cout << "========================================================" << endl;
    cout << "========================================================" << endl;
    cout << "========================================================" << endl;
    net();

    return 0;

};


/*
 * Test of Step class
 */

#include "Exchanger.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

using namespace std;

template <typename T> class SimpleNet;
class SimpleStep : public Exchanger {
public:
    template <typename ...Args>
    SimpleStep(SimpleNet<SimpleStep> &net, Args ...args)
        : Exchanger(args...)
        , m_net(net)
    {}
    double calc(const position &pos);
    virtual void on_start(unsigned step);
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
            m_net.push_back(new T(*this, 1, 2, i, N, M, Hi, Hj, args...));
        }
        m_net[0]->Step::at(0, 0) = 1;
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
    T *operator [] (ssize_t i) {
        if (i < 0 || i > (ssize_t)m_length) {
            return NULL;
        }
        return m_net[i];
    }

    const T *operator [] (size_t i) const{
        return m_net[i];
    }
private:
    vector<T *> m_net;
    size_t m_length;
};
double SimpleStep::calc(const position &pos)
{
    double result;
    result = at(pos);
    if (pos.first > 0) {
        result += at(pos.first - 1, pos.second);
    }
    if (pos.second > 0) {
        result += at(pos.first, pos.second - 1);
    }
    if (pos.first > 0 && pos.second > 0) {
        result += at(pos.first - 1, pos.second - 1);
    }
    return (long)result % 100000;

}
void SimpleStep::on_start(unsigned step)
{
    SimpleStep *nb[4] = {m_net[at().index_top()], m_net[at().index_bottom()],
        m_net[at().index_left()], m_net[at().index_right()]};
    if (nb[0]) top(Exchanger::W)[0] = (*nb[0])->bottom();
    if (nb[1]) bottom(Exchanger::W)[0] = (*nb[1])->top();
    if (nb[2]) left(Exchanger::W)[0] = (*nb[2])->right();
    if (nb[3]) right(Exchanger::W)[0] = (*nb[3])->left();
}

    #define N 10
    #define M 20
    #define Hi 5
    #define Hj 10
    #define Steps 20
    #define Count (N/Hi * M/Hj)
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
    net();
    return 0;
};


#include "Exchanger.hpp"
#include <cstdlib>
#include <utility>

#include <iostream>
Exchanger::~Exchanger()
{
    for (size_t i=0; i<size(); ++i) {
        delete m_bottom[i];
        delete m_top[i];
        delete m_corner[i];
        delete m_left[i];
        delete m_right[i];
    }
}
Exchanger::net &Exchanger::at(size_t i)
{
    return Step::at(i);
}
const Exchanger::net &Exchanger::at(size_t i) const
{
    return Step::at(i);
}
double Exchanger::at(size_t i, size_t j, size_t l) const
{
    position pos = local(i, j);

    if (pos.first < 0) {
        if(pos.second < 0) {
            return corner(TL, abs(pos), l);
        } else if (pos.second < (ssize_t)Hj()) {
            return top(abs(pos), l);
        } else {
            return corner(TR, abs(pos), l);
        }
    } else if (pos.first < (ssize_t)Hi()) {
        if(pos.second < 0) {
            return left(abs(pos), l);
        } else if (pos.second < (ssize_t)Hj()) {
            return Step::at(l).at(pos);
        } else {
            return right(abs(pos), l);
        }
    } else {
        if(pos.second < 0) {
            return corner(BL, abs(pos), l);
        } else if (pos.second < (ssize_t)Hj()) {
            return bottom(abs(pos), l);
        } else {
            return corner(BR, abs(pos), l);
        }
    }
}
double Exchanger::at(size_t i, size_t j, size_t l)
{
    return ((const Exchanger *)this)->at(i,j, l);
}

double Exchanger::at(const position &pos, size_t l) const
{
    return Exchanger::at(pos.first, pos.second, l);
}
double Exchanger::at(const position &pos, size_t l)
{
    return Exchanger::at(pos.first, pos.second, l);
}

Exchanger::edge_t &Exchanger::top(size_t t)
{
    return *m_top [t];
}
Exchanger::edge_t &Exchanger::bottom(size_t t)
{
    return *m_bottom [t];
}
Exchanger::edge_t &Exchanger::left(size_t t)
{
    return *m_left [t];
}
Exchanger::edge_t &Exchanger::right(size_t t)
{
    return *m_right [t];
}
Exchanger::corner_t &Exchanger::corner(size_t t)
{
    return *m_corner [t];
}
const Exchanger::edge_t &Exchanger::top(size_t t) const
{
    return *m_top [t];
}
const Exchanger::edge_t &Exchanger::bottom(size_t t) const
{
    return *m_bottom [t];
}
const Exchanger::edge_t &Exchanger::left(size_t t) const
{
    return *m_left [t];
}
const Exchanger::edge_t &Exchanger::right(size_t t) const
{
    return *m_right [t];
}
const Exchanger::corner_t &Exchanger::corner(size_t t) const
{
    return *m_corner [t];
}
double Exchanger::top(const position &pos, size_t l) const
{
    return top(l)[pos.first][pos.second];
}
double Exchanger::bottom(const position &pos, size_t l) const
{
    return bottom(l)[pos.first][pos.second];
}
double Exchanger::left(const position &pos, size_t l) const
{
    return left(l)[pos.second][pos.first];
}
double Exchanger::right(const position &pos, size_t l) const
{
    return right(l)[pos.second][pos.first];
}
double Exchanger::corner(CType c, const position &pos, size_t l) const
{
    return corner(l)[c][pos];
}
Exchanger::position Exchanger::abs(const position &pos) const
{
    position res = pos;
    if (res.first < 0) {
        res.first = -1 - res.first;
    } else while (res.first > (ssize_t)Hi()) {
        res.first -= Hi();
    }
    if (res.second < 0) {
        res.second = -1 - res.second;
    } else while (res.second > (ssize_t)Hj()) {
        res.second -= Hj();
    }
    return res;
}
void Exchanger::v_next()
{
    size_t s = size() - 1;
    auto t = m_top[s];
    auto b = m_bottom[s];
    auto l = m_left[s];
    auto r = m_right[s];
    auto c = m_corner[s];

    for (size_t i = size() - 1; i >0; --i) {
        m_top[i] = m_top[i-1];
        m_bottom[i] = m_bottom[i-1];
        m_right[i] = m_right[i-1];
        m_left[i] = m_left[i-1];
        m_corner[i] = m_corner[i-1];
    }
    Step::v_next();

    m_top[0] = t;
    m_bottom[0] = b;
    m_left[0] = l;
    m_right[0] = r;
    m_corner[0] = c;
}

#include "Exchanger.hpp"
#include <cstdlib>
#include <utility>

double Exchanger::at(size_t i, size_t j) const
{
    position pos = local(i, j);

    if (pos.first < 0) {
        if(pos.second < 0) {
            return corner(TL, abs(pos));
        } else if (pos.second < (ssize_t)Hj()) {
            return top(abs(pos));
        } else {
            return corner(TR, abs(pos));
        }
    } else if (pos.first < (ssize_t)Hi()) {
        if(pos.second < 0) {
            return left(abs(pos));
        } else if (pos.second < (ssize_t)Hj()) {
            return Step::at(i, j);
        } else {
            return right(abs(pos));
        }
    } else {
        if(pos.second < 0) {
            return corner(BL, abs(pos));
        } else if (pos.second < (ssize_t)Hj()) {
            return bottom(abs(pos));
        } else {
            return corner(BR, abs(pos));
        }
    }
}
double Exchanger::at(size_t i, size_t j)
{
    return ((const Exchanger *)this)->at(i,j);
}

double Exchanger::at(const position &pos) const
{
    return Exchanger::at(pos.first, pos.second);
}
double Exchanger::at(const position &pos)
{
    return Exchanger::at(pos.first, pos.second);
}

Exchanger::edge_t &Exchanger::top(OType t)
{
    return *m_top [t];
}
Exchanger::edge_t &Exchanger::bottom(OType t)
{
    return *m_bottom [t];
}
Exchanger::edge_t &Exchanger::left(OType t)
{
    return *m_left [t];
}
Exchanger::edge_t &Exchanger::right(OType t)
{
    return *m_right [t];
}
Exchanger::corner_t &Exchanger::corner(OType t)
{
    return *m_corner [t];
}
const Exchanger::edge_t &Exchanger::top(OType t) const
{
    return *m_top [t];
}
const Exchanger::edge_t &Exchanger::bottom(OType t) const
{
    return *m_bottom [t];
}
const Exchanger::edge_t &Exchanger::left(OType t) const
{
    return *m_left [t];
}
const Exchanger::edge_t &Exchanger::right(OType t) const
{
    return *m_right [t];
}
const Exchanger::corner_t &Exchanger::corner(OType t) const
{
    return *m_corner [t];
}
double Exchanger::top(const position &pos) const
{
    return top(R)[pos.first][pos.second];
}
double Exchanger::bottom(const position &pos) const
{
    return bottom(R)[pos.first][pos.second];
}
double Exchanger::left(const position &pos) const
{
    return left(R)[pos.second][pos.first];
}
double Exchanger::right(const position &pos) const
{
    return right(R)[pos.second][pos.first];
}
double Exchanger::corner(CType c, const position &pos) const
{
    return corner(R)[c][pos];
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
template <typename T> void swap (T (&vals)[2])
{
    std::swap(vals[0], vals[1]);
}
void Exchanger::swap()
{
    ::swap(m_top);
    ::swap(m_bottom);
    ::swap(m_left);
    ::swap(m_right);
    ::swap(m_corner);
}

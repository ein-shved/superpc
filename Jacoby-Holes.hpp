#ifndef _JACOBY_HOLE_HPP_
#define _JACOBY_HOLE_HPP_

#include "Jacoby.hpp"
#include "Hole.hpp"
#include <cmath>

#include <iostream>
class Jacoby_Hole : public Jacoby {
public:
    template <typename ... Args>
    Jacoby_Hole (const Hole &h, const EdgeCondition &holeCondition,
                 Args ... args)
        : Jacoby(args...)
        , m_hole(h.copy())
        , m_holeCondition(holeCondition)
    {}
    ~Jacoby_Hole() {
        delete m_hole;
    }
    virtual double method(const position &pos)
    {
        bool edge;
        if (m_hole->contains(pos, edge)) {
            if (edge) return m_holeCondition(step(), pos.first, pos.second);
            return NAN;
        }
        return Jacoby::method(pos);
    }
private:
    Hole *m_hole;
    const EdgeCondition &m_holeCondition;
};

class HoleCondition : public EdgeCondition {
public:
    template <typename F>
    HoleCondition (const F &f, size_t N, size_t M = 0, double Lx = 1,
            double Ly = 0)
        : m_f(f)
        , Hx(Lx/(M == 0 ? N : M))
        , Hy((Ly == 0 ? Lx : Ly)/N)
    {}
    virtual double calc (size_t step, size_t i, size_t j) const
    {
        return m_f(j*Hx, i*Hy);
    }
private:
    std::function<double(double, double)> m_f = [](double x, double y)
        -> double { return 0; };
    double Hx, Hy;
};

#endif /* Jacoby-Holes.hpp */

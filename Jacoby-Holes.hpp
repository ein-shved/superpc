#ifndef _JACOBY_HOLE_HPP_
#define _JACOBY_HOLE_HPP_

#include "Jacoby.hpp"
#include "Hole.hpp"
#include <cmath>

#include <iostream>
class Jacoby_Hole : public Jacoby {
public:
    Jacoby_Hole (const Hole &h, const EdgeCondition &holeCondition,
                 const EdgeCondition &conditions,
                 const MPI_Comm &comm, size_t overlap, size_t len, 
                 size_t index, size_t N, size_t M,
                 size_t Hi, size_t Hj, 
                 const_reference val = value_type())
        : Jacoby(conditions, comm, overlap, len, index, N, M, Hi, Hj, val)
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
    HoleCondition (SplitEdgeCondition::zero_f f, size_t N, size_t M = 0, double Lx = 1,
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
    SplitEdgeCondition::zero_f m_f;
    double Hx, Hy;
};

#endif /* Jacoby-Holes.hpp */

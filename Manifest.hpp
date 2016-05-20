#include "MPI.hpp"
#include <stdexcept>
#include <cmath>

class EdgeCondition {
public:
    virtual double calc (size_t step, size_t i, size_t j) const = 0;
    double operator () (size_t step, size_t i, size_t j) const {
        return calc(step, i, j);
    }
};

class Manifest : public MPI_Exchanger {
public :
    Manifest(const EdgeCondition &conditions,
             const MPI_Comm &comm, size_t overlap, size_t len,
             size_t index, size_t N, size_t M,
             size_t Hi, size_t Hj,
             const_reference val = value_type())
        : MPI_Exchanger(comm, overlap, len, index, N, M, Hi, Hj, val)
        , m_conditions(conditions)
        , m_eps(0)
    {}
    virtual double method(const position &pos) = 0;
    double eps();
    virtual void on_start(unsigned step);
    virtual void on_stop(unsigned step);
private:
    virtual double calc(const position &pos);
    virtual void v_next_iterate(Step::net &dst);
    double resid (Step::net &dst);
private:
    const EdgeCondition &m_conditions;
    double m_eps;
};

class SplitEdgeCondition : public EdgeCondition {
public:
    typedef double (*zero_f)(double, double);
    typedef double (*edge_f)(double);
public:
    SplitEdgeCondition(size_t N, size_t M = 0, double Lx = 1, double Ly = 0)
        : m_z(default_zero)
        , m_t(default_edge)
        , m_b(default_edge)
        , m_l(default_edge)
        , m_r(default_edge)
        , Hx(Lx/(M == 0 ? N : M))
        , Hy((Ly == 0 ? Lx : Ly)/N)
        , m_N(N), m_M(M)
    {}
    void zero (zero_f f) {
        m_z = f;
    }
    void top (edge_f f) {
        m_t = f;
    }
    void bottom (edge_f f) {
        m_b = f;
    }
    void left (edge_f f) {
        m_l = f;
    }
    void right (edge_f f) {
        m_r = f;
    }
    virtual double calc (size_t step, size_t i, size_t j) const
    {
        if (step == 1) {
            return m_z(j*Hx, i*Hy);
        }
        if (j == 0) return m_l(i*Hy);
        if (j == m_M-1) return m_r(i*Hy);
        if (i == 0) return m_t(j*Hx);
        if (i == m_N-1) return m_b(j*Hx);
        return NAN;
    }
public:
    static double default_zero(double, double) {
        return NAN;
    }
    static double default_edge(double) {
        return NAN;
    }
private:
    zero_f m_z;
    edge_f m_t, m_b, m_l, m_r;
    double Hx, Hy;
    size_t m_N, m_M;
};

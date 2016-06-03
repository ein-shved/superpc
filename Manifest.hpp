#include "MPI.hpp"
#include <stdexcept>
#include <cmath>

#ifndef _MANIFEST_HPP_
#define _MANIFEST_HPP_

class EdgeCondition {
public:
    virtual double calc (size_t step, size_t i, size_t j) const = 0;
    double operator () (size_t step, size_t i, size_t j) const {
        return calc(step, i, j);
    }
};

class Manifest : public MPI_Exchanger {
public :
    template <typename ... Args>
    Manifest(const EdgeCondition &conditions, Args ... args)
        : MPI_Exchanger(args ...)
        , m_conditions(conditions)
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
    double m_eps = 0;
};

class SplitEdgeCondition : public EdgeCondition {
public:
    SplitEdgeCondition(size_t N, size_t M = 0, double Lx = 1, double Ly = 0)
        : Hx(Lx/(M == 0 ? N : M))
        , Hy((Ly == 0 ? Lx : Ly)/N)
        , m_N(N), m_M(M)
    {}
    template <typename F>
    void zero (const F &f) {
        m_z = f;
    }
    template <typename F>
    void top (const F &f) {
        m_t = f;
    }
    template <typename F>
    void bottom (const F &f) {
        m_b = f;
    }
    template <typename F>
    void left (const F &f) {
        m_l = f;
    }
    template <typename F>
    void right (const F &f) {
        m_r = f;
    }
    virtual double calc (size_t step, size_t i, size_t j) const
    {
        if (step == 1) return m_z(j*Hx, i*Hy);
        if (j == 0) return m_l(i*Hy);
        if (j == m_M-1) return m_r(i*Hy);
        if (i == 0) return m_t(j*Hx);
        if (i == m_N-1) return m_b(j*Hx);
        return NAN;
    }
private:
    std::function<double(double, double)> m_z = [](double x, double y)
        -> double { return 0; };
    std::function<double(double)> m_t = [](double x) -> double { return NAN;};
    std::function<double(double)> m_b = [](double x) -> double { return NAN;};
    std::function<double(double)> m_l = [](double y) -> double { return NAN;};
    std::function<double(double)> m_r = [](double y) -> double { return NAN;};
    double Hx, Hy;
    size_t m_N, m_M;
};

#endif /* Manifest.hpp */

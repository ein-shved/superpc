#include "Manifest.hpp"
#include "Params.hpp"

#include <algorithm>

class Jacoby : public Manifest {
public:
    template <typename F>
    Jacoby (const F &f, const EdgeCondition &conditions,
             const MPI_Comm &comm, size_t overlap, size_t len,
             size_t index, size_t N, size_t M,
             size_t Hi, size_t Hj,
             const_reference val = value_type())
        : Manifest(conditions, comm, overlap, len, index, N, M, Hi, Hj)
        , m_Hx(1./this->M())
        , m_Hy(1./this->N())
        , m_T(std::min(m_Hx*m_Hx/2, m_Hy*m_Hy/2)/2)
        , m_f(f)
    {}
    virtual double method(const position &pos)
    {
        double add_i = at(pos.first + 1, pos.second) - 2 * at(pos) +
            at(pos.first - 1, pos.second);
        double add_j = at(pos.first, pos.second + 1) - 2 * at(pos) +
            at(pos.first, pos.second - 1);
        double result =  at(pos) + m_T*(add_i/(m_Hy*m_Hy) +
                    add_j/(m_Hx*m_Hx) +
                    m_f(pos.second*m_Hx, pos.first*m_Hy, step()*m_T));
        return result;
    }
    double time() const
    {
        return m_T * step();
    }
private:
    double m_Hx;
    double m_Hy;
    double m_T;
    double (*m_f)(double, double, double);
};
class Jacoby2 {
public:
    Jacoby2(size_t N, size_t M, const Functor &f)
        : m_Hx((1.)/N)
        , m_Hy((1.)/M)
        , m_T(std::min(m_Hx*m_Hx/2, m_Hy*m_Hy/2)/5)
        , m_f(f)
    {}

    double operator () (double v, double t, double r, double b, double l,
            size_t step, double x, double y) const
    {
        double add_i = l - 2 * v + r;
        double add_j = b - 2 * v + t;
        double result =  v + m_T*(add_i/(m_Hy*m_Hy) +
                    add_j/(m_Hx*m_Hx) + m_f(x, y, step*m_T));
        return result;
    }
    double T() const {
        return m_T;
    }

private:
    double m_Hx;
    double m_Hy;
    double m_T;
    const Functor &m_f;
};

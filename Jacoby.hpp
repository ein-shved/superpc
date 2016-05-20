#include "Manifest.hpp"

#include <algorithm>

class Jacoby : public Manifest {
public:
    Jacoby (const EdgeCondition &conditions,
             const MPI_Comm &comm, size_t overlap, size_t len,
             size_t index, size_t N, size_t M,
             size_t Hi, size_t Hj,
             const_reference val = value_type())
        : Manifest(conditions, comm, overlap, len, index, N, M, Hi, Hj)
        , m_Hx(1./this->M())
        , m_Hy(1./this->N())
        , m_T(std::min(m_Hx*m_Hx/2, m_Hy*m_Hy/2)/2)

    {}
    virtual double method(const position &pos) {
        double add_i = at(pos.first + 1, pos.second) - 2 * at(pos) +
            at(pos.first - 1, pos.second);
        double add_j = at(pos.first, pos.second + 1) - 2 * at(pos) +
            at(pos.first, pos.second - 1);
        double result =  at(pos) + m_T*(add_i/(m_Hy*m_Hy) +
                add_j/(m_Hx*m_Hx));
        return result;
    }
private:
    double m_Hx;
    double m_Hy;
    double m_T;
};

#include "Manifest.hpp"

#include <algorithm>

class Jacoby : public Manifest {
public:
    template <typename ... Args>
    Jacoby (Args ... args)
        : Manifest(args ...)
        , m_Hx(((double) 1)/M())
        , m_Hy(((double) 1)/N())
        , m_T(std::min(m_Hx*m_Hx/2, m_Hy*m_Hy/2))

    {}
    virtual double method(const position &pos) {
        double add_i = at(pos.first + 1, pos.second) - 2 * at(pos) +
            at(pos.first - 1, pos.second);
        double add_j = at(pos.first, pos.second + 1) - 2 * at(pos) +
            at(pos.first, pos.second - 1);
        double result =  at(pos) + m_T*(add_i/m_Hy + add_j/m_Hx);
        return result;
    }
private:
    double m_Hx;
    double m_Hy;
    double m_T;
};

#ifndef _PARAMS_HPP_
#define _PARAMS_HPP_

#include "Hole.hpp"
#include "Manifest.hpp"
#include <string>

class HoleParams {
public:
    virtual const Hole *hole() const = 0;

    static const HoleParams *get(const std::string &name);

};

class Functor {
public:
typedef double (F)(double, double, double);
public:
    Functor(const F &f)
        : m_f(f)
    {}
    Functor(const Functor &f)
        : m_f(f.m_f)
    {}
    virtual double calc(double x, double y, double t) const
    {
        return m_f(x,y,t);
    }
    double operator () (double x, double y, double t) const
    {
        return calc(x,y,t);
    }
    static const Functor &get(const std::string &name);
private:
    const F &m_f;
};

#endif /* Params.hpp */

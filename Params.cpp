#include "Params.hpp"
#include <map>

using namespace std;
class __base_params : public HoleParams {
protected:
    __base_params ()
        : m_hole (NULL)
    {}
    __base_params(const Hole &h)
        : m_hole (h.copy())
    {}
    __base_params(const Hole *h)
        : m_hole (h->copy())
    {}
    ~__base_params() {
        if (m_hole != NULL)
            delete m_hole;
    }
    virtual const Hole *hole() const
    {
        return m_hole;
    }
    virtual const EdgeCondition *edge() const
    {
        return NULL;
    }
    void add(const Hole *hole) {
        Hole *tmp = m_hole;
        if (tmp == NULL) {
            m_hole = hole == NULL ? NULL : hole->copy();
            return;
        }
        m_hole = new Holes(tmp, hole);
    }
    void add(const Hole &hole) {
        return add(&hole);
    }
private:
    Hole *m_hole;
};
class Test_HoleParams : public __base_params {
public:
    Test_HoleParams() :
        __base_params(Holes{
            RectangleHole(1./4, 1./8, 3*1./8, 1./4),
            RectangleHole(1./8, 5*1./8, 1./4, 6*1./8),
            RectangleHole(5*1./8, 6*1./8, 6*1./8, 7*1./8),
            RectangleHole(6*1./8, 3*1./8, 7*1./8, 1./2),
            RectangleHole(3*1./8, 1./2, 1./2, 5*1./8),
            RectangleHole(3*1./8 - 1./16, 1./2 - 1./16, 1./2 - 1./16, 5*1./8 - 1./16)})
    {    }
};
const static Test_HoleParams Test;

class Double_HoleParams:public __base_params {
public:
    Double_HoleParams() :
        __base_params(Holes(fHole(up), fHole(down)))
    {}
private:
    static bool down(double x, double y) {
        return y < x*x;
    }
    static bool up(double x, double y) {
        return y*y > x;
    }
};
const static Double_HoleParams Double;

class Line_HoleParams : public __base_params {
public:
    Line_HoleParams() :
        __base_params(Holes(fHole(up), fHole(down)))
        {}
private:
    static bool down(double x, double y) {
        return y < 0.5 - x;
    }
    static bool up(double x, double y) {
        return y > 1.5 - x;
    }
};
const static Line_HoleParams Line;

const HoleParams *HoleParams::get (const string &in_name)
{
#define PARAM(name) params[#name] = &name
    static map<string, const HoleParams * > params;
    if (params.empty ()){
        PARAM(Test);
        PARAM(Double);
        PARAM(Line);
    }
#undef PARAM

    map<string, const HoleParams *>::iterator it = params.find(in_name);
    if (it == params.end()) return NULL;
    return it->second;
}

#define PI (3.14159265358979)
double Functor_zero1(double x, double y, double t){
    return x*y;
}
double Functor_hole_edge1(double x, double y, double t){
    return 1;
}
double Functor_left1(double x,double y, double t){
    return 2*sin(2*PI*y);
}
double Functor_right1(double x,double y, double t){
    return 1-cos(2*PI*y);
}
double Functor_top1(double x, double y, double t){
    return 0;
}
double Functor_bottom1(double x, double y, double t){
    return 2*sin(2*PI*x);
}
double Functor_edge1(double x, double y, double t){
    return sin(PI*x) + sin(PI*y);
}
double Functor_hole1(double x, double y, double t){
    return sin(100*PI*t);
}
double Functor_foo1(double x, double y, double t){
    return sin(100*PI*t);
}

double Functor_ant(double x, double y, double t){
    return x*x + y*y + sin(100*PI*t);
}
double Functor_ant_foo(double x, double y, double t){
    return 100*PI*cos(PI*t) - 4;
}
double __zero(double x, double y, double t){
    return 0;
}
const Functor &Functor::get(const std::string &in_name)
{
#define FUNCTOR(name) functors.insert(pair<string, Functor>(#name, \
            Functor(Functor_ ## name)));
    static map<string, Functor> functors;
    if (functors.empty ()){
        FUNCTOR(zero1);
        FUNCTOR(hole_edge1);
        FUNCTOR(left1);
        FUNCTOR(right1);
        FUNCTOR(top1);
        FUNCTOR(bottom1);
        FUNCTOR(edge1);
        FUNCTOR(hole1);
        FUNCTOR(ant);
        FUNCTOR(ant_foo);
    }
    static Functor zero(__zero);
    map<string, Functor>::iterator it = functors.find(in_name);
    if (it == functors.end()) return zero;
    return it->second;
}

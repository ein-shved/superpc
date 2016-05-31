#ifndef _HOLE_HPP_
#define _HOLE_HPP_

#include <vector>
#include <utility>
#include <initializer_list>
#include <cstddef>
#include <unistd.h>

class Hole {
public:
    typedef std::pair<double, double> position;
public:
    virtual ~Hole() {}
    virtual bool contains (double i, double j) = 0;
    bool contains (const position &pos)
    {
        return contains(pos.first, pos.second);
    }
    virtual Hole *copy () const = 0;
};

class RectangleHole : public Hole {
public:
    RectangleHole (double i1 = 0, double j1 = 0, double i2 = 0, double j2 = 0)
        : m_i1(i1) , m_j1(j1), m_i2(i2), m_j2(j2)
    {
        if (m_i1 > m_i2) std::swap(m_i1, m_i2);
        if (m_j1 > m_j2) std::swap(m_j1, m_j2);
    }
    RectangleHole (const RectangleHole &other)
        : RectangleHole(other.m_i1, other.m_j1, other.m_i2, other.m_j2)
    {}
    virtual bool contains (double i, double j)
    {
        if (!((i >= m_i1) && (i < m_i2) && (j >= m_j1) && (j < m_j2))) {
            return false;
        }
        return true;
    }
    virtual RectangleHole *copy () const
    {
        return new RectangleHole(*this);
    }
private:
    double m_i1, m_j1, m_i2, m_j2;
};
template <typename F>
class FHole : public Hole {
public:
    FHole(const F &f)
        : m_f(f)
    {}
    operator const F& () const{
        return m_f;
    }
    virtual bool contains (double i, double j) {
        return m_f(i,j);
    }
    virtual Hole *copy () const {
        return new FHole(*this);
    }
private:
    const F &m_f;
};
template <typename F>
static inline FHole<F> fHole (const F &f)
{
    return FHole<F> (f);
}

class Holes : public Hole {
public:
    template <typename ... Args>
    Holes (Args... args)
    {
        append(args...);
    }
    ~Holes()
    {
        for (auto h: m_holes) delete h;
    }
    void append(){};
    template <typename ... Args>
    void append(const Hole &h, Args ... args)
    {
        m_holes.push_back(h.copy());
        append (args...);
    }
    template <typename ... Args>
    void append(const Hole *h, Args ... args)
    {
        append(*h, args...);
    }
    template <typename ... Args>
    void append(const Holes &other, Args ... args)
    {
        for (auto h: other.m_holes) append(h);
        append (args...);
    }
    template <typename ... Args>
    void append(const Holes *h, Args ... args)
    {
        append (*h, args...);
    }
    virtual bool contains (double i, double j)
    {
        for (auto h: m_holes) {
            if (h->contains(i,j)) {
                return true;
            }
        }
        return false;
    }
    virtual Holes *copy() const {
        return new Holes (this);
    }
private:
    std::vector<Hole *> m_holes;
};

#endif /* Hole.hpp */

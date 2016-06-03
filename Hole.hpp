#ifndef _HOLE_HPP_
#define _HOLE_HPP_

#include <vector>
#include <utility>
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
        : m_i1(other.m_i1)
        , m_j1(other.m_j1)
        , m_i2(other.m_i2)
        , m_j2(other.m_j2)
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
    typedef std::vector<Hole *> vector;
public:
    Holes (const Holes *other = NULL) {
        append(other);
    }
    ~Holes()
    {
        for (vector::const_iterator it = m_holes.begin();
            it != m_holes.begin(); ++it)
        {
            append(*it);
        }
    }
    void append(){};
    void append(const Hole &h)
    {
        m_holes.push_back(h.copy());
    }
    void append(const Hole *h)
    {
        if (h == NULL) return;
        append(*h);
    }
    void append(const Holes &other)
    {
        for (vector::const_iterator it = other.m_holes.begin();
            it != other.m_holes.end(); ++it)
        {
            append(*it);
        }
    }
    void append(const Holes *h)
    {
        if (h== NULL) return;
        append (*h);
    }
    virtual bool contains (double i, double j)
    {
        for (vector::const_iterator it = m_holes.begin();
            it != m_holes.end(); ++it)
        {
            if ((*it)->contains(i,j)) {
                return true;
            }
        }
        return false;
    }
    virtual Holes *copy() const {
        return new Holes (this);
    }
private:
    vector m_holes;
};

#endif /* Hole.hpp */

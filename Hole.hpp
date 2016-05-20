#ifndef _HOLE_HPP_
#define _HOLE_HPP_

#include <vector>
#include <utility>

class Hole {
public:
    typedef std::pair<ssize_t, ssize_t> position;
public:
    virtual ~Hole() {}
    virtual bool contains (size_t i, size_t j, bool &edge) = 0;
    bool contains (const position &pos, bool &edge)
    {
        return contains(pos.first, pos.second, edge);
    }
    virtual Hole *copy () const = 0;
};

class RectangleHole : public Hole {
public:
    RectangleHole (size_t i1 = 0, size_t j1 = 0, size_t i2 = 0, size_t j2 = 0)
        : m_i1(i1) , m_j1(j1), m_i2(i2), m_j2(j2)
    {
        if (m_i1 > m_i2) std::swap(m_i1, m_i2);
        if (m_j1 > m_j2) std::swap(m_j1, m_j2);
    }
    RectangleHole (const RectangleHole &other)
        : m_i1(other.m_i1)
        , m_i2(other.m_j1)
        , m_j1(other.m_i2)
        , m_j2(other.m_j2)
    {}
    virtual bool contains (size_t i, size_t j, bool &edge)
    {
        if (!((i >= m_i1) && (i < m_i2) && (j >= m_j1) && (j < m_j2))) {
            return false;
        }
        edge = (i == m_i1) || (i == m_i2-1) || (j == m_j1) || (j == m_j2 - 1);
        return true;
    }
    virtual RectangleHole *copy () const
    {
        return new RectangleHole(*this);
    }
private:
    size_t m_i1, m_j1, m_i2, m_j2;
};

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
    virtual bool contains (size_t i, size_t j, bool &edge)
    {
        bool result = false, e = false, r = false;
        for (vector::const_iterator it = m_holes.begin();
            it != m_holes.end(); ++it)
        {
            r = (*it)->contains(i,j, e);
            result |= r;
            if (r && !e) {
                edge = false;
                return result;
            } else if (r && e) {
                edge = true;
            }
        }
        return result;
    }
    virtual Holes *copy() const {
        return new Holes (this);
    }
private:
    vector m_holes;
};

#endif /* Hole.hpp */

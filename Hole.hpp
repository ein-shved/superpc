#ifndef _HOLE_HPP_
#define _HOLE_HPP_

#include <vector>
#include <utility>
#include <initializer_list>

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

#include <iostream>
class RectangleHole : public Hole {
public:
    RectangleHole (size_t i1 = 0, size_t j1 = 0, size_t i2 = 0, size_t j2 = 0)
        : m_i1(i1) , m_j1(j1), m_i2(i2), m_j2(j2)
    {
        if (m_i1 > m_i2) std::swap(m_i1, m_i2);
        if (m_j1 > m_j2) std::swap(m_j1, m_j2);
    }
    RectangleHole (const RectangleHole &other)
        : RectangleHole(other.m_i1, other.m_j1, other.m_i2, other.m_j2)
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
    virtual bool contains (size_t i, size_t j, bool &edge)
    {
        bool result = false, e = false, r = false;
        for (auto h: m_holes) {
            r = h->contains(i,j, e);
            result |= r;
            if (r & !e) {
                edge = false;
                return result;
            } else if (r & e) {
                edge = true;
            }
        }
        return result;
    }
    virtual Holes *copy() const {
        return new Holes (this);
    }
private:
    std::vector<Hole *> m_holes;
};

#endif /* Hole.hpp */

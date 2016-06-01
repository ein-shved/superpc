#ifndef _VERTEX_HPP_
#define _VERTEX_HPP_

#include <cstring>
#include <cmath>

template <typename T>
static inline T square(T v) {
    return v*v;
}

class Neighbour;
class Vertex {
public:
    enum Type {
        Inner, I = Inner,
        Border, B = Border,
        Edge, E = Edge,
        Neighbour, N = Neighbour,
        Hole, H = Hole,
    };
    enum Direction {
        Top = 0, Right = 1, Bottom = 2, Left = 3,
    };
public:
    Vertex (size_t index, size_t N, size_t M, size_t &step, double val = 0,
            Type type = Inner)
        : m_index(index)
        , m_step(step)
        , m_type(type)
    {
        m_i = m_index % N;
        m_j = m_index / N;
        m_x = (1./N) * m_i;
        m_y = (1./N) * m_j;
        m_value[0] = m_value[1] = val;
        for (size_t i = 0; i < 4; ++i) {
            m_neighbors[i] = NULL;
        }
    }
    Vertex (const Vertex &v)
        : m_index(v.m_index)
        , m_i(v.m_i), m_j(v.m_j)
        , m_x(v.m_x), m_y(v.m_y)
        , m_step(v.m_step)
        , m_type(v.m_type)
    {
        m_value[0] = v.m_value[0];
        m_value[1] = v.m_value[1];
        for (size_t i = 0; i < 4; ++i) {
            m_neighbors[i] = v.m_neighbors[i];
        }
    }

    virtual void set (double value, size_t m_i = 0)
    {
        notify_neighbor();
        m_value[ (m_step + m_i) % 2 ] = value;
    }
    virtual double get (size_t m_i = 1)
    {
        return m_value[ (m_step + m_i) % 2 ];
    }
    operator double ()
    {
        return get();
    }

    void pos (size_t &i, size_t &j) const {
        i = m_i;
        j = m_j;
    }
    void pos (double &x, double &y) const {
        x = m_x;
        y = m_y;
    }
    double x() const {
        return m_x;
    }
    double y() const {
        return m_y;
    }
    size_t i() const {
        return m_i;
    }
    size_t j() const {
        return m_j;
    }
    size_t index () const {
        return m_index;
    }
    Type type () const {
        return m_type;
    }
    static double distance (double x1, double y1, double x2 = 0, double y2 = 0)
    {
        return sqrt (square(x1-x2) + square(y1 - y2));
    }
    double distance (const Vertex &other)
    {
        return distance (m_x, m_y, other.m_x, other.m_y);
    }
    double distance ()
    {
        return distance(m_x,m_y);
    }
    size_t step() {
        return m_step;
    }
    bool inner() {
        return m_type == Inner || m_type == Border;
    }

    void neighbors (Vertex *const nb[4])
    {
        neighbors (nb[Top], nb[Right], nb[Bottom], nb[Left]);
    }
    void neighbors (Vertex *t, Vertex *r, Vertex *b, Vertex *l)
    {
        set_top(t);
        set_right(r);
        set_bottom(b);
        set_left(l);
    }
    void set_top(Vertex *top)
    {
        m_neighbors[Top] = top;
    }
    void set_right(Vertex *right)
    {
        m_neighbors[Right] = right;
    }
    void set_bottom(Vertex *bottom)
    {
        m_neighbors[Bottom] = bottom;
    }
    void set_left(Vertex *left)
    {
        m_neighbors[Left] = left;
    }
    size_t degree(const ::Neighbour *nb);

    Vertex *neighbor(Direction d)
    {
        return m_neighbors[d];
    }
    Vertex *top() { return neighbor(Top); }
    Vertex *right() { return neighbor(Right); }
    Vertex *bottom() { return neighbor(Bottom); }
    Vertex *left() { return neighbor(Left); }
protected:
    void notify(Vertex *) { };
private:
    void check_type (Vertex *nb)
    {
        if (nb == NULL) return;
        if (m_type == Inner && !nb->inner()) {
            m_type = Border;
        }
    }
    void notify_neighbor();

private:
    size_t m_index;
    size_t m_i, m_j;
    double m_x, m_y;
    double m_value[2];
    size_t &m_step;
    Vertex *m_neighbors[4];
    Type m_type;
};

template <typename F, Vertex::Type VertexType>
class CondVertex : public Vertex {
public:
    CondVertex (size_t index, size_t N, size_t M, size_t &step,
                double t, const F &f)
        : Vertex (index, N, M, step, 0, VertexType)
        , m_T(t)
        , m_F (f)
        , m_step(step)
    {
        Vertex::set(m_F(x(), y(), step*m_T));
    }
    virtual void set(double) { };
    virtual double get(double)
    {
        if (m_step != step()){
            m_step = step();
            Vertex::set(m_F(x(), y(), m_step*m_T));
        }
        return Vertex::get();
    };
private:
    F m_F;
    double m_T;
    size_t m_step;
};
template <typename F> class EdgeVertex : public CondVertex<F, Vertex::Edge> {};
template <typename F> class HoleVertex : public CondVertex<F, Vertex::Hole> {};
template <typename F>
static inline EdgeVertex<F> *edgeVertex (size_t index, size_t N, size_t M,
                                         size_t &step, double t, const F &f)
{
    return new EdgeVertex<F>(index, N, M, step, t, f);
}
template <typename F>
static inline HoleVertex<F> *holeVertex (size_t index, size_t N, size_t M,
                                         size_t &step, double t, const F &f)
{
    return new HoleVertex<F>(index, N, M, step, t, f);
}

class NeighbourVertex : public Vertex {
public:
    NeighbourVertex (size_t index, size_t N, size_t M, size_t &step,
                     ::Neighbour &nb)
        : Vertex(index, N, M, step, Vertex::Neighbour)
        , m_neighbour(nb)
    {}
    NeighbourVertex (const NeighbourVertex &v)
        : Vertex(v)
        , m_neighbour(v.m_neighbour)
    {}
    ::Neighbour *neighbour() {
        return &m_neighbour;
    }

protected:
    virtual void notify(Vertex *v);
private:
    ::Neighbour &m_neighbour;
};

#endif /* Vertex.hpp */

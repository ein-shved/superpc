/*
 * Class, represents the thread which interacts with other threads via
 * exchages.
 */

#ifndef _EXCHANGER_HPP_
#define _EXCHANGER_HPP_

#include "Step.hpp"
#include <array>

class Exchanger : public Step {
public:
    typedef typename Step::matrix matrix;
    typedef typename Step::line line;
    typedef typename Step::raw raw;
    typedef typename Step::column column;
    typedef typename Step::value_type value_type;
    typedef typename Step::reference reference;
    typedef typename Step::pointer pointer;
    typedef typename Step::const_reference const_reference;
    typedef typename Step::const_pointer const_pointer;
    typedef typename Step::position position;
    typedef std::vector<line> edge_t;
    typedef std::array<matrix, 4> corner_t;

public:
    enum CType {TL=0, TR=1, BR=2, BL=3};
    enum OType {WRITE=0, W=0, READ=1, R=1};

public:

    template <typename ... Args>
    Exchanger(size_t overlap, size_t len, size_t index, size_t N, size_t M,
              size_t Hi, size_t Hj, Args ... args)
        : Step(len, index, N, M, Hi, Hj, args...)
        , m_top1(overlap, line(Hj))
        , m_top2(overlap, line(Hj))
        , m_bottom1(overlap, line(Hj))
        , m_bottom2(overlap, line(Hj))
        , m_left1(overlap, column(Hi))
        , m_left2(overlap, column(Hi))
        , m_right1(overlap, column(Hi))
        , m_right2(overlap, column(Hi))
        , m_corner1({matrix(overlap, overlap), matrix(overlap, overlap),
                    matrix(overlap, overlap), matrix(overlap, overlap)})
        , m_corner2({matrix(overlap, overlap), matrix(overlap, overlap),
                    matrix(overlap, overlap), matrix(overlap, overlap)})
    {}

    /* This operator gets the indexes from all over the entire net */
    using Step::at;
    double at(size_t i, size_t j) const;
    double at(size_t i, size_t j);
    double at(const position &pos) const;
    double at(const position &pos);

    edge_t &top(OType t);
    edge_t &bottom(OType t);
    edge_t &left(OType t);
    edge_t &right(OType t);
    corner_t &corner(OType t);

    const edge_t &top(OType t) const;
    const edge_t &bottom(OType t) const;
    const edge_t &left(OType t) const;
    const edge_t &right(OType t) const;
    const corner_t &corner(OType t) const;

    double top(const position &pos) const;
    double bottom(const position &pos) const;
    double left(const position &pos) const;
    double right(const position &pos) const;
    double corner(CType c, const position &pos) const;

    position abs(const position &pos) const;
    void swap();

private:
    /* TODO use two buffers for read and write and swap them */
    edge_t m_top1, m_top2;
    edge_t m_bottom1, m_bottom2;
    edge_t m_left1, m_left2;
    edge_t m_right1, m_right2;
    corner_t m_corner1, m_corner2;

    edge_t *m_top[2] = {&m_top1, &m_top2};
    edge_t *m_bottom[2] = {&m_bottom1, &m_bottom2};
    edge_t *m_left[2] = {&m_left1, &m_left2};
    edge_t *m_right[2] = {&m_right1, &m_right2};
    corner_t *m_corner[2] = {&m_corner1, &m_corner2};
};

#endif /* exchanger.hpp */

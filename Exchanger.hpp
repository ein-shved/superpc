/*
 * Class, represents the thread which interacts with other threads via
 * exchages.
 */

#ifndef _EXCHANGER_HPP_
#define _EXCHANGER_HPP_

#include "Step.hpp"

class Exchanger : public Step {
public:
    typedef Step::matrix matrix;
    typedef Step::line line;
    typedef Step::raw raw;
    typedef Step::column column;
    typedef Step::value_type value_type;
    typedef Step::reference reference;
    typedef Step::pointer pointer;
    typedef Step::const_reference const_reference;
    typedef Step::const_pointer const_pointer;
    typedef Step::position position;
    typedef std::vector<line> edge_t;
    typedef std::vector<matrix> corner_t;

public:
    enum CType {TL=0, TR=1, BR=2, BL=3};

public:

    Exchanger(size_t overlap, size_t len, size_t index, size_t N, size_t M,
              size_t Hi, size_t Hj, const_reference val = value_type())
        : Step(len, index, N, M, Hi, Hj, val)
    {
        for (size_t i=0; i<len; ++i) {
            m_bottom.push_back(new edge_t(overlap, line(Hj)));
            m_top.push_back(new edge_t(overlap, line(Hj)));
            m_left.push_back(new edge_t(overlap, column(Hi)));
            m_right.push_back(new edge_t(overlap, column(Hi)));
            m_corner.push_back(new corner_t(4, matrix(overlap, overlap)));
        }
    }
    virtual ~Exchanger();

    net &at(size_t i = 0);
    const net &at(size_t i = 0) const;

    /* This operator gets the indexes from all over the entire net */
    double at(size_t i, size_t j, size_t l = 0) const;
    double at(size_t i, size_t j, size_t l = 0);
    double at(const position &pos, size_t l = 0) const;
    double at(const position &pos, size_t l = 0);

    edge_t &top(size_t t);
    edge_t &bottom(size_t t);
    edge_t &left(size_t t);
    edge_t &right(size_t t);
    corner_t &corner(size_t t);

    const edge_t &top(size_t t) const;
    const edge_t &bottom(size_t t) const;
    const edge_t &left(size_t t) const;
    const edge_t &right(size_t t) const;
    const corner_t &corner(size_t t) const;

    double top(const position &pos, size_t l = 0) const;
    double bottom(const position &pos, size_t l = 0) const;
    double left(const position &pos, size_t l = 0) const;
    double right(const position &pos, size_t l = 0) const;
    double corner(CType c, const position &pos, size_t l = 0) const;

    position abs(const position &pos) const;

public:
    virtual void v_next();
    const static size_t W;

private:
    std::vector<edge_t *> m_top;
    std::vector<edge_t *> m_bottom;
    std::vector<edge_t *> m_left;
    std::vector<edge_t *> m_right;
    std::vector<corner_t *> m_corner;
};

#endif /* exchanger.hpp */

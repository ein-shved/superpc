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

public:

    template <typename ... Args>
    Exchanger(size_t overlap, size_t len, size_t index, size_t N, size_t M,
              size_t Hi, size_t Hj, Args ... args)
        : Step(len, index, N, M, Hi, Hj, args...)
    {
        for (size_t i=0; i<len; ++i) {
            m_bottom.push_back(new edge_t(overlap, line(Hj)));
            m_top.push_back(new edge_t(overlap, line(Hj)));
            m_left.push_back(new edge_t(overlap, column(Hi)));
            m_right.push_back(new edge_t(overlap, column(Hi)));
            m_corner.push_back(new corner_t({
                        matrix(overlap, overlap),
                        matrix(overlap, overlap),
                        matrix(overlap, overlap),
                        matrix(overlap, overlap)}));
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
    const size_t W = 0;

private:
    std::vector<edge_t *> m_top;
    std::vector<edge_t *> m_bottom;
    std::vector<edge_t *> m_left;
    std::vector<edge_t *> m_right;
    std::vector<corner_t *> m_corner;
};

#endif /* exchanger.hpp */

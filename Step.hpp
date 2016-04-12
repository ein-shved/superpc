/*
 * Class, represents the thread and responsible for running each step and
 * watching the states of old steps.
 */

#ifndef _STEP_HPP_
#define _STEP_HPP_

#include "Net.hpp"

class Step : public std::vector<Net<double> *> {
public:
    typedef Net<double> net;
    typedef std::vector<Net<double> *>base_type;
    typedef typename net::matrix matrix;
    typedef typename matrix::line line;
    typedef typename matrix::raw raw;
    typedef typename matrix::column column;
    typedef typename matrix::value_type value_type;
    typedef typename matrix::reference reference;
    typedef typename matrix::pointer pointer;
    typedef typename matrix::const_reference const_reference;
    typedef typename matrix::const_pointer const_pointer;
    typedef typename matrix::position position;

public:
    Step(size_t len, size_t index, size_t N, size_t M, size_t Hi, size_t Hj,
            double val = 0);
    template<typename F, typename ...Args>
    Step(size_t len, size_t index, size_t N, size_t M, size_t Hi, size_t Hj,
            const F &f, Args ... args)
        : base_type(len)
    {
        for (size_t i = 0; i < size(); ++i) {
            base_type::at(i) = new net(index, N, M, Hi, Hj, f, args...);
        }
    }
    ~Step();

    net &at(size_t i = 0);
    const net &at(size_t i = 0) const;

    reference at(size_t i, size_t j);
    const_reference at(size_t i, size_t j) const;

    reference at(const position &pos);
    const_reference at(const position &pos) const;

    reference operator()(size_t i, size_t j);
    const_reference operator()(size_t i, size_t j) const;

    reference operator()(const position &pos);
    const_reference operator()(const position &pos) const;

    net &operator [] (size_t i);
    const net &operator [] (size_t i) const;

    reference operator [] (const position &pos);
    const_reference operator [] (const position &pos) const;

    net &operator()(size_t i = 0);
    const net &operator()(size_t i = 0) const;

    operator net &();
    operator const net &() const;

    virtual void next();

    void send_up(size_t i = 0);
    void send_down(size_t i = 0);
    void send_left(size_t j = 0);
    void send_right(size_t j = 0);
};

#endif /* Step.hpp */

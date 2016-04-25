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
    typedef std::vector<net *>base_type;
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
    template<typename ...Args>
    Step(size_t len, Args ... args)
        : base_type(len, NULL)
    {
        for (size_t i = 0; i < len; ++i) {
            base_type::at(i) = new net(args...);
        }
    }
    virtual ~Step();

    /* Passes indexes from all over the entire net */
    virtual double calc (const position &pos) = 0;

    net &at(size_t i = 0);
    const net &at(size_t i = 0) const;

    /* This operator gets the indexes from all over the entire net */
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

    net *operator ->();
    const net *operator ->() const;

    position global (size_t i, size_t j) const;
    position global (const position &pos) const;
    position local (size_t i, size_t j) const;
    position local (const position &pos) const;

    operator net &();
    operator const net &() const;

    unsigned step() const;

    unsigned next();
    virtual void on_start(unsigned step);
    virtual void on_stop(unsigned step);

    line &store_up(size_t i = 0);
    line &store_down(size_t i = 0);
    column &store_right(size_t j = 0);
    column &store_left(size_t j = 0);

    size_t N() const
    {
        return at().N();
    }
    size_t M() const
    {
        return at().M();
    }
    size_t Hi() const
    {
        return at().Hi();
    }
    size_t Hj() const
    {
        return at().Hj();
    }
    size_t Nc() const
    {
        return at().Nc();
    }
    size_t Mc() const
    {
        return at().Mc();
    }
    size_t I() const
    {
        return at().I();
    }
    size_t J() const
    {
        return at().J();
    }

private:
    unsigned m_step = 0;
    std::vector<line> m_top;
    std::vector<line> m_bottom;
    std::vector<column> m_left;
    std::vector<column> m_right;

};

#endif /* Step.hpp */

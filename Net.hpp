/*
 * Class contains the meta-information of entire net
 * and the data of current cell.
 */

#ifndef _NET_HPP_
#define _NET_HPP__

#include "Matrix.hpp"

template <typename T = double>
class Net : public Matrix <T> {
public:
    typedef Matrix <T> matrix;
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
    Net(size_t index, size_t N, size_t M, size_t Hi, size_t Hj,
            const_reference val = value_type())
        : matrix(Hi, Hj, val)
    {
        p_init(index, N, M, Hi, Hj);
    };
    template<typename F>
    Net (size_t index, size_t N, size_t M, size_t Hi, size_t Hj,
            const F &f)
        : matrix(Hi, Hj, f)
    {
        p_init(index, N, M, Hi, Hj);
    };


    size_t N() const
    {
        return m_N;
    }
    size_t M() const
    {
        return m_M;
    }
    size_t Hi() const
    {
        return matrix::N();
    }
    size_t Hj() const
    {
        return matrix::M();
    }
    size_t Nc() const
    {
        return m_Nc;
    }
    size_t Mc() const
    {
        return m_Mc;
    }
    size_t I() const
    {
        return m_I;
    }
    size_t J() const
    {
        return m_J;
    }

    position matrix_pos(size_t i, size_t j) const
    {
        return position(i / Hi(), j / Hj());
    }
    position matrix_pos(position &pos) const
    {
        return matrix_pos(pos.first, pos.second);
    }
    size_t global_index(size_t i, size_t j) const
    {
        return index(matrix_pos(i, j));
    }
    size_t global_index(const position &pos) const
    {
        return global_index(pos.first, pos.second);
    }

    size_t index() const
    {
        return m_index;
    }
    size_t index(size_t i, size_t j) const
    {
        if (i >= m_Nc) i = m_Nc - 1;
        if (j >= m_Mc) j = m_Mc - 1;
        return m_Mc * i + j;
    }
    position pos_of(size_t index) const
    {
        position pos;
        pos.first = index / m_Mc * Hi();
        pos.second = index % m_Mc * Hj();
        return pos;
    }
    size_t index(const position &pos) const
    {
        return index(pos.first, pos.second);
    }
    ssize_t index_relative(ssize_t i = 0, ssize_t j = 0) const
    {
        ssize_t l_I = m_I + i, l_J = m_J + j;
        if (l_I < 0) return -1;
        if (l_I >= (ssize_t)m_Nc) return -1;
        if (l_J < 0) return -1;
        if (l_J >= (ssize_t)m_Mc) return -1;
        return index(l_I, l_J);
    }
    ssize_t index_top(size_t i = 1) const
    {
        return index_relative(-(ssize_t)i, 0);
    }
    ssize_t index_bottom(size_t i = 1) const
    {
        return index_relative(i, 0);
    }
    ssize_t index_left(size_t j = 1) const
    {
        return index_relative(0, -(ssize_t)j);
    }
    ssize_t index_right(size_t j = 1) const
    {
        return index_relative(0, j);
    }

    position global (size_t i, size_t j) const
    {
        return position(m_I * Hi() + i, m_J * Hj() + j);
    }
    position global (const position &pos) const
    {
        return global(pos.first, pos.second);
    }
    position local (size_t i, size_t j) const
    {
        return position(i - m_I * Hi(), j - m_J * Hj());
    }
    position local (const position &pos) const
    {
        return local(pos.first, pos.second);
    }

    const line &top(size_t i = 1) const {
        return this->at(i);
    }
    line &top(size_t i = 0) {
        return this->at(i);
    }
    const line &bottom(size_t i = 1) const {
        return this->at(Hi() - i);
    }
    line &bottom(size_t i = 1) {
        return this->at(Hi() - i);
    }
    column left(size_t j = 1) {
        return this->get_column(j);
    }
    column right(size_t j = 1) {
        return this->get_column(Hj() - j);
    }
private:
    void p_init(size_t index, size_t N, size_t M, size_t Hi, size_t Hj)
    {
        m_N = N + (Hi - N % Hi) % Hi; /* Make net devidable on step  */
        m_M = M + (Hj - M % Hj) % Hj;
        m_index = index;
        m_Nc = m_N / Hi;
        m_Mc = m_M / Hj;
        m_I = index / m_Mc;
        m_J = index % m_Mc;
    }
private:
    size_t m_N, m_M;
    size_t m_index;

    size_t m_Nc, m_Mc;
    size_t m_I, m_J;
};

#endif /* Net.hpp */

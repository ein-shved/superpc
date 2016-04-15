/*
 *  Matrix is the fixed size vector of fixed sized vectors.
 */


#include <vector>
#include <functional>
#include <utility>
#include <cstddef>

#ifndef _MATRIX_HPP_
#define _MATRIX_HPP_

template <typename T>
class Matrix : public std::vector< std::vector<T> > {
public:
    typedef std::vector<std::vector<T> > base_type;
    typedef std::vector<T> column;
    typedef std::vector<T> line, raw;
    typedef T value_type;
    typedef value_type &reference;
    typedef value_type *pointer;
    typedef const value_type &const_reference;
    typedef const value_type *const_pointer;
    typedef std::pair<size_t, size_t> position;

public:
    Matrix(size_t in_N = 0, size_t in_M = 0,
           const value_type &val = value_type())
        : base_type(in_N,line(in_M, val))
        , m_N(in_N)
        , m_M(in_M)
    {}
    template<typename F, typename ...Args>
    Matrix (size_t in_N, size_t in_M, const F &f, Args... args)
        : base_type(in_N,line(in_M))
        , m_N(in_N)
        , m_M(in_M)
    {
        for (size_t i = 0; i< m_N; ++i) for (size_t j = 0; j < m_M; ++j) {
            (*this)[i][j] = f(i,j, args...);
        }
    }

    size_t N() const
    {
        return m_N;
    }
    size_t M() const
    {
        return m_M;
    }

    using base_type::at;
    reference at(size_t i, size_t j)
    {
        return at(i).at(j);
    }
    const_reference at(size_t i, size_t j) const
    {
        return at(i).at(j);
    }
    reference at(const position &pos)
    {
        return at(pos.first,pos.second);
    }
    const_reference at(const position &pos) const
    {
        return at(pos.first,pos.second);
    }
    reference operator()(size_t i, size_t j)
    {
        return at(i, j);
    }
    const_reference operator()(size_t i, size_t j) const
    {
        return at(i, j);
    }
    reference operator()(const position &pos)
    {
        return at(pos);
    }
    const_reference operator()(const position &pos) const
    {
        return at(pos);
    }
    reference operator  [] (const position &pos)
    {
        return at(pos);
    }
    const_reference operator  [] (const position &pos) const
    {
        return at(pos);
    }
    void fill (const value_type &val)
    {
        line c;
        c.fill(val);
        base_type::fill(c);
    }
    line &operator[] (size_t i)
    {
        return base_type::at(i);
    }
    const line &operator[] (size_t i) const
    {
        return base_type::at(i);
    }
    column get_column(size_t j) const
    {
        column result(m_N);
        for (size_t i = 0; i < m_N; ++i) {
            result[i] = at(i,j);
        }
        return result;
    }
    template<typename F, typename ...Args>
    void foreach (const F &f, Args... args)
    {
        for (line &l : (*this)) {
            for (reference val : l) {
                f(val, args...);
            }
        }
    }
    template<typename F, typename ...Args>
    void foreach (const F &f, Args... args) const
    {
        for (const line &l : (*this)) {
            for (const_reference val : l) {
                f(val, args...);
            }
        }
    }
private:
    size_t m_N, m_M;
};

#endif /* Matrix.hpp */

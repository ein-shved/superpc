/*
 *  Matrix is the fixed size array of fixed sized arrays.
 */


#include <array>
#include <functional>

#ifndef _MATRIX_HPP_
#define _MATRIX_HPP_

template <typename T, size_t N, size_t M>
class Matrix : public std::array<std::array<T, M>, N> {
public:
    typedef std::array<std::array<T, M>, N> base_type;
    typedef std::array<T, N> column;
    typedef std::array<T, M> line, raw;
    typedef T value_type;
    typedef value_type &reference;
    typedef value_type *pointer;
    typedef const value_type &const_reference;
    typedef const value_type *const_pointer;

public:
    Matrix() {}
    Matrix(const T &val)
    {
        fill(val);
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
        column result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = at(i,j);
        }
        return result;
    }
    template<typename ...Args>
    void foreach (const std::function<void(reference, Args...)> &f,
                  Args... args)
    {
        for (line &l : (*this)) {
            for (reference val : l) {
                f(val, args...);
            }
        }
    }
    template<typename ...Args>
    void foreach (const std::function<void (const_reference, Args...)> &f,
                  Args... args) const
    {
        for (const line &l : (*this)) {
            for (const_reference val : l) {
                f(val, args...);
            }
        }
    }
};

#endif /* Matrix.hpp */

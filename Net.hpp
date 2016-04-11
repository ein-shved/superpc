/*
 * Class contains the information of entire net and the data of current cell
 */

#include "Matrix.hpp"

template <typename T = int, size_t N, size_t M, size_t Hy, size_t Hx = Hy>
class Net : Matrix <T, Hx, Hy> {
public:
    typedef Matrix <T, Hy, Hx> matrix;
    using matrix::base_type, matrix::line, matrix::raw, matrix::column;
    using matrix::value_type, matrix::reference, matrix::pointer;
    using matrix::const_reference, matrix::const_pointer;

public:
    Net(const T& val = 0) : matrix(0) {};

    const line &top(size_t i = 0) const {
        return at(i);
    }
    line &top(size_t i = 0) {
        return at(i);
    }
    const line &bottom(size_t i = 0) const {
        return at(Hy - 1 - i);
    }
    line &bottom(size_t i = 0) {
        return at(Hy - 1 - i);
    }

    column left(size_t j = 0) {
        return get_column(j);
    }
    column right(size_t j = 0) {
        return get_column(Hx - 1 - j);
    }
private:

};

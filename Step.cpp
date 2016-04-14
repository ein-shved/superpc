#include "Step.hpp"

using namespace std;
typedef typename Step::net net;
typedef typename Step::line line;
typedef typename Step::raw raw;
typedef typename Step::column column;
typedef typename Step::value_type value_type;
typedef typename Step::reference reference;
typedef typename Step::pointer pointer;
typedef typename Step::const_reference const_reference;
typedef typename Step::const_pointer const_pointer;
typedef typename Step::position position;

Step::Step(size_t len, size_t index, size_t N, size_t M, size_t Hi, size_t Hj,
        double val)
    : base_type(len)
{
    for (size_t i = 0; i < size(); ++i) {
        base_type::at(i) = new net(index, N, M, Hi, Hj, val);
    }
}
Step::~Step()
{
    for (auto n : (*this)) delete n;
}

net &Step::at(size_t i)
{
    return *base_type::at(i);
}
const net &Step::at(size_t i) const
{
    return *base_type::at(i);
}
reference Step::at(size_t i, size_t j)
{
    return at().at(local(i,j));
}
const_reference Step::at(size_t i, size_t j) const
{
    return at().at(local(i,j));
}
reference Step::at(const position &pos)
{
    return at(pos.first,pos.second);
}
const_reference Step::at(const position &pos) const
{
    return at(pos.first,pos.second);
}
reference Step::operator()(size_t i, size_t j)
{
    return at(i,j);
}
const_reference Step::operator()(size_t i, size_t j) const
{
    return at(i,j);
}
reference Step::operator()(const position &pos)
{
    return at(pos);
}
const_reference Step::operator()(const position &pos) const
{
    return at(pos);
}
net &Step::operator [] (size_t i)
{
    return at(i);
}
const net &Step::operator [] (size_t i) const
{
    return at(i);
}
reference Step::operator [] (const position &pos)
{
    return at(pos);
}
const_reference Step::operator [] (const position &pos) const
{
    return at(pos);
}
net &Step::operator()(size_t i)
{
    return at(i);
}
const net &Step::operator()(size_t i) const
{
    return at(i);
}
position Step::global (size_t i, size_t j) const
{
    return at().global(i,j);
}
position Step::global (const position &pos) const
{
    return at().global(pos);
}
position Step::local (size_t i, size_t j) const
{
    return at().local(i,j);
}
position Step::local (const position &pos) const
{
    return at().local(pos);
}

Step::operator net &()
{
    return at();
}
Step::operator const net &() const
{
    return at();
}
unsigned Step::operator ()() const
{
    return m_step;
}
unsigned Step::next()
{
    size_t Hi, Hj;

    net *ptr = base_type::at(size() - 1);
    for (size_t i = size() - 1; i > 0; --i) {
        base_type::at(i) = base_type::at(i-1);
    }
    (*ptr) = at(0);

    ++m_step;
    on_start(m_step);
    Hi = at().Hi();
    Hj = at().Hj();
    for (size_t i = 0; i < Hi; ++i) for (size_t j = 0; j < Hj; ++j) {
        (*ptr)[i][j] = calc(global(i,j));
    }
    base_type::at(0) = ptr;
    on_stop(m_step);

    return m_step;
}
template <typename T>
static T &step_store(vector<T> &v, size_t i)
{
    if (i >= v.size()) {
        v.resize(i+1);
    }
    return v[i];
}
line &Step::store_up(size_t i)
{
    return step_store(m_top, i);
}
line &Step::store_down(size_t i)
{
    return step_store(m_bottom, i);
}
column &Step::store_right(size_t j)
{
    return step_store(m_left, j);
}
column &Step::store_left(size_t j)
{
    return step_store(m_right, j);
}

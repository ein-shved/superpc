#include <iostream>
#include "MPI.hpp"


MPI_Exchanger::~MPI_Exchanger()
{
    if (m_result != NULL) {
        delete m_result;
    }
}
int MPI_Exchanger::send (size_t i, const line &s, int dst, int tag)
{
    return MPI_Send((void *)s.data(), s.size(), MPI_DOUBLE, dst, tag*100 + i, m_comm);
}
int MPI_Exchanger::recv (size_t i, line &r, int dst, int tag)
{
    return MPI_Recv(r.data(), r.size(), MPI_DOUBLE, dst, tag*100 + i, m_comm,
            MPI_STATUS_IGNORE);
}
int MPI_Exchanger::send_recv (size_t i, const line &s, int s_tag,
        line &r, int r_tag, int dst)
{
    return MPI_Sendrecv((void *)s.data(), s.size(), MPI_DOUBLE, dst, s_tag*100 + i,
                r.data(), r.size(), MPI_DOUBLE, dst, r_tag*100 + i,
                m_comm, MPI_STATUS_IGNORE);
}

void MPI_Exchanger::send_top(size_t i)
{
    const line &s = (*this)->top();
    int dst = at(0).index_top();
    if (dst < 0 || dst == (int)at(0).index()) {
        return;
    }
    send(i, s, dst, tag_top);
}
void MPI_Exchanger::recv_top(size_t i)
{
    line &r = top(W)[i];
    int dst = at(0).index_top();
    if (dst < 0 || dst == (int)at(0).index()) {
        return;
    }
    recv(i, r, dst, tag_bottom);
}
void MPI_Exchanger::send_recv_top(size_t i)
{
    const line &s = (*this)->top();
    line &r = top(W)[i];
    int dst = at(0).index_top();
    if (dst < 0 || dst == (int)at(0).index()) {
        return;
    }
    send_recv(i, s, tag_top, r, tag_bottom, dst);
}
void MPI_Exchanger::send_bottom(size_t i)
{
    const line &s = at(0)[at(0).size() - 1 - i];
    int dst = at(0).index_bottom();
    if (dst < 0 || dst == (int)at(0).index()) {
        return;
    }
    send(i, s, dst, tag_bottom);
}
void MPI_Exchanger::recv_bottom(size_t i)
{
    line &r = bottom(W)[i];
    int dst = at(0).index_bottom();
    if (dst < 0 || dst == (int)at(0).index()) {
        return;
    }
    recv(i, r, dst, tag_top);
}
void MPI_Exchanger::send_recv_bottom(size_t i)
{
    const line &s = at(0)[at(0).size() - 1 - i];
    line &r = bottom(W)[i];
    int dst = at(0).index_bottom();
    if (dst < 0 || dst == (int)at(0).index()) {
        return;
    }
    send_recv(i, s, tag_bottom, r, tag_top, dst);
}
void MPI_Exchanger::send_left(size_t i)
{
    const line s = at(0).get_column(i);
    int dst = at(0).index_left();
    if (dst < 0 || dst == (int)at(0).index()) {
        return;
    }
    send(i, s, dst, tag_left);
}
void MPI_Exchanger::recv_left(size_t i)
{
    line &r = left(W)[i];
    int dst = at(0).index_left();
    if (dst < 0 || dst == (int)at(0).index()) {
        return;
    }
    recv(i, r, dst, tag_right);
}
void MPI_Exchanger::send_recv_left(size_t i)
{
    const line s = at(0).get_column(i);
    line &r = left(W)[i];
    int dst = at(0).index_left();
    if (dst < 0 || dst == (int)at(0).index()) {
        return;
    }
    send_recv(i, s, tag_left, r, tag_right, dst);
}
void MPI_Exchanger::send_right(size_t i)
{
    const line s = at(0).get_column(at(0).Hj() - 1 - i);
    int dst = at(0).index_right();
    if (dst < 0 || dst == (int)at(0).index()) {
        return;
    }
    send(i, s, dst, tag_right);
}
void MPI_Exchanger::recv_right(size_t i)
{
    line &r = right(W)[i];
    int dst = at(0).index_right();
    if (dst < 0 || dst == (int)at(0).index()) {
        return;
    }
    recv(i, r, dst, tag_left);
}
void MPI_Exchanger::send_recv_right(size_t i)
{
    const line s = at(0).get_column(at(0).Hj() - 1 - i);
    line &r = right(W)[i];
    int dst = at(0).index_right();
    if (dst < 0 || dst == (int)at(0).index()) {
        return;
    }
    send_recv(i, s, tag_right, r, tag_left, dst);
}
int MPI_Exchanger::index() const
{
    return at(0).index();
}
int MPI_Exchanger::runk() const
{
    return index();
}
MPI_Exchanger::matrix *MPI_Exchanger::sync_results()
{
    if (index() == 0) {
        return sync_master();
    }
    sync_slave();
    return NULL;
}
MPI_Comm MPI_Exchanger::comm() const
{
    return m_comm;
}
MPI_Exchanger::matrix *MPI_Exchanger::sync_master()
{
    if (m_result != NULL) delete m_result;
    m_result = new matrix(at(0).N(), at(0).M());
    sync_put(at(0), index());
    matrix r(at(0).Hi(), at(0).Hj());
    for (size_t i = 1; i < at(0).Nc()*at(0).Mc(); ++i) {
        if (sync_recv(r, i) < 0) return NULL;
        if (sync_put(r, i) < 0) return NULL;
    }
    return m_result;
}
int MPI_Exchanger::sync_recv(matrix &rcv, size_t index)
{
    for (size_t i = 0; i < rcv.size(); ++ i) {
        if (recv (0, rcv[i], index, tag_result) < 0) {
            return -1;
        }
    }
    return 0;
}
int MPI_Exchanger::sync_put(const matrix &rcv, size_t index)
{
    position start = at(0).pos_of(index);
    for (size_t i = 0; i< rcv.size(); ++i) {
        for (size_t j=0; j < rcv[i].size(); ++j) {
            (*m_result)[i + start.first][j + start.second] = rcv[i][j];
        }
    }
    return 0;
}
int MPI_Exchanger::sync_slave()
{
    for (size_t i = 0; i < at().size(); ++ i) {
        if (send (0, at()[i], 0, tag_result) < 0) {
            return -1;
        }
    }
    return 0;
}

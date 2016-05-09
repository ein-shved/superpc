
#include "MPI.hpp"


int MPI_Exchanger::send (size_t i, const line &s, int dst, int tag)
{
    return MPI_Send(s.data(), s.size(), MPI_DOUBLE, dst, tag*100 + i, m_comm);
}
int MPI_Exchanger::recv (size_t i, line &r, int dst, int tag)
{
    return MPI_Recv(r.data(), r.size(), MPI_DOUBLE, dst, tag*100 + i, m_comm,
            MPI_STATUS_IGNORE);
}
int MPI_Exchanger::send_recv (size_t i, const line &s, int s_tag,
        line &r, int r_tag, int dst)
{
    return MPI_Sendrecv(s.data(), s.size(), MPI_DOUBLE, s_tag*100 + i, dst,
                r.data(), r.size(), MPI_DOUBLE, r_tag*100 + i, dst,
                m_comm, MPI_STATUS_IGNORE);
}

void MPI_Exchanger::send_top(size_t i)
{
    const line &s = at(0)[i];
    int dst = at(0).index_top();
    if (dst < 0 || dst == (int)at(0).index()) {
        return;
    }
    send(i, s, dst, tag_top);
}
void MPI_Exchanger::recv_top(size_t i)
{
    line &r = top(0)[i];
    int dst = at(0).index_top();
    if (dst < 0 || dst == (int)at(0).index()) {
        return;
    }
    send(i, r, dst, tag_bottom);
}
void MPI_Exchanger::send_recv_top(size_t i)
{
    const line &s = at(0)[i];
    line &r = top(0)[i];
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
    line &r = bottom(0)[i];
    int dst = at(0).index_bottom();
    if (dst < 0 || dst == (int)at(0).index()) {
        return;
    }
    send(i, r, dst, tag_top);
}
void MPI_Exchanger::send_recv_bottom(size_t i)
{
    const line &s = at(0)[at(0).size() - 1 - i];
    line &r = bottom(0)[i];
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
    line &r = left(0)[i];
    int dst = at(0).index_left();
    if (dst < 0 || dst == (int)at(0).index()) {
        return;
    }
    send(i, r, dst, tag_right);
}
void MPI_Exchanger::send_recv_left(size_t i)
{
    const line s = at(0).get_column(i);
    line &r = left(0)[i];
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
    line &r = right(0)[i];
    int dst = at(0).index_right();
    if (dst < 0 || dst == (int)at(0).index()) {
        return;
    }
    send(i, r, dst, tag_left);
}
void MPI_Exchanger::send_recv_right(size_t i)
{
    const line s = at(0).get_column(at(0).Hj() - 1 - i);
    line &r = right(0)[i];
    int dst = at(0).index_right();
    if (dst < 0 || dst == (int)at(0).index()) {
        return;
    }
    send_recv(i, s, tag_right, r, tag_left, dst);
}
void MPI_Exchanger::on_start(unsigned step)
{}
void MPI_Exchanger::on_stop(unsigned step)
{}

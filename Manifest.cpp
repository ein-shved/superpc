#include "Manifest.hpp"
#include <stdexcept>
#include <cmath>

using namespace std;

double Manifest::calc(const position &pos)
{
    double eps, res;
    try {
        if (step() == 0 || pos.first == 0 || pos.second == 0 ||
                pos.first >= (ssize_t)N() - 1 || pos.second >= (ssize_t)M() - 1)
        {
            return m_conditions(step(), pos.first, pos.second);
        }
    } catch (const std::invalid_argument &) {}
    res = method(pos);
    eps = std::fabs(at(pos) - res);
    if (m_eps < eps ) {
        m_eps = eps;
    }
    return res;
}
void Manifest::on_start(unsigned step)
{
    send_recv_top();
    send_recv_bottom();
    send_recv_left();
    send_recv_right();
    m_eps = 0;
}
void Manifest::on_stop(unsigned step)
{
    double eps = m_eps;

    MPI_Allreduce(&m_eps, &eps, 1, MPI_DOUBLE, MPI_MAX, comm());
    m_eps = eps;
}
double Manifest::eps()
{
    return m_eps;
}

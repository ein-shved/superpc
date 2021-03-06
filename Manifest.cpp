#include "Manifest.hpp"
#include <stdexcept>
#include <cmath>

using namespace std;

double Manifest::calc(const position &pos)
{
    double res;
    if (step() == 1 || pos.first == 0 || pos.second == 0 ||
            pos.first >= (ssize_t)N() - 1 || pos.second >= (ssize_t)M() - 1)
    {
        res = m_conditions(step(), pos.first, pos.second);
        if (res != NAN) {
            return res;
        }
    }
    return method(pos);
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

    MPI_Allreduce(&m_eps, &eps, 1, MPI_DOUBLE, MPI_SUM, comm());
    m_eps = sqrt(eps);
}
double Manifest::eps()
{
    return m_eps;
}
void Manifest::v_next_iterate(Step::net &dst)
{
    size_t Hi = at().Hi();
    size_t Hj = at().Hj();
#pragma omp parallel
    {
#pragma omp for collapse (2)
        for (size_t i = 0; i < Hi; ++i) for (size_t j = 0; j < Hj; ++j) {
            dst[i][j] = calc(global(i,j));
        }
//#pragma omp barrier
        double eps = resid(dst);
        if (eps > m_eps) {
#pragma omp critical
            {
                if (eps > m_eps) {
                    m_eps = eps;
                }
            }
        }
    }
}
double Manifest::resid (Step::net &dst)
{
    size_t Hi = at().Hi();
    size_t Hj = at().Hj();
    double eps = 0;
#pragma omp for collapse (2)
    for (size_t i = 0; i < Hi; ++i) for (size_t j = 0; j < Hj; ++j) {
        double r1 = at(global(i,j)), r2 = dst[i][j];
        if (isnan(r2)) continue;
        eps += (r1 - r2) * (r1 - r2);
    }
    return eps;
}

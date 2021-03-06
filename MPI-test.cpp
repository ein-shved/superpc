/*
 * Test of Step class
 */

#include "Exchanger.hpp"
#include "MPI.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>

using namespace std;

template <typename T> class SimpleNet;

class MPITest : public MPI_Exchanger {
public:
    template <typename ...Args>
    MPITest(Args ...args)
        : MPI_Exchanger(args...)
    {
        if (index() == 0) {
            at(0)[0][0] = 1;
        }
    }
    double calc(const position &pos);
    virtual void on_start(unsigned step);

};
double MPITest::calc(const position &pos)
{
    double result;
    result = at(pos);
    if (pos.first > 0) {
        result += at(pos.first - 1, pos.second);
    }
    if (pos.second > 0) {
        result += at(pos.first, pos.second - 1);
    }
    if (pos.first > 0 && pos.second > 0) {
        result += at(pos.first - 1, pos.second - 1);
    }
    return (long)result % 100000;

}
void MPITest::on_start(unsigned step)
{
    send_recv_top();
    send_recv_bottom();
    send_recv_left();
    send_recv_right();
}
template<typename T>
void print(const Matrix<T> &m)
{
    for (size_t i = 0; i < m.N(); ++i) {
        for (size_t j = 0; j < m.M(); ++j) {
            cout.width(5);
            cout << m[i][j] << " ";
        }
        cout << endl;
    }
}

    #define N 10
    #define M 20
    #define Hi 5
    #define Hj 10
    #define Steps 20
    #define Count (N/Hi * M/Hj)
void mpi(int &argc, char **&argv)
{
    MPI_Init(&argc, &argv);
    MPI_Comm comm = MPI_COMM_WORLD;
    int rank;
    MPI_Comm_rank(comm, &rank);
    cout << "Got runk " << rank << endl;
    MPITest mpi(comm, 1, 2, rank, N, M, Hi, Hj);
    for (int i = 0; i < Steps; ++i) {
        mpi.next();
    }
    Matrix<double> *result = mpi.sync_results();
    if (result != NULL) {
        print(*result);
    }
    MPI_Finalize();
}

int main(int argc, char *argv[])
{
    mpi(argc, argv);
    return 0;
};


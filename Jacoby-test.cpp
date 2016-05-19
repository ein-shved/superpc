/*
 * Test of Step class
 */

#include "Jacoby-Holes.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <cmath>

using namespace std;

#define PI 3.14159265359

template<typename T>
void print(const Matrix<T> &m)
{
    for (size_t i = 0; i < m.N(); ++i) {
        for (size_t j = 0; j < m.M(); ++j) {
            cout.precision(4);
            cout.width(7);
            cout << std::fixed;
            cout << m[i][j] << " ";
        }
        cout << endl;
    }
}

    #define N 10
    #define M 20
    #define Hi 5
    #define Hj 10
    #define Eps 0.01
    #define Count (N/Hi * M/Hj)
void jacoby(int &argc, char **&argv)
{
    MPI_Init(&argc, &argv);
    MPI_Comm comm = MPI_COMM_WORLD;
    int rank;
    auto zero =
        [](double x, double y) -> double { return sqrt(abs(x*x - y*y)); };
    auto left = [](double y) -> double { return sin(2*PI*y); };
    auto right = [](double y) -> double { return 1-cos(2*PI*y); };
    auto top = [](double x) -> double { return 0; };
    auto bottom = [](double x) -> double { return sin(2*PI*x); };
    SplitEdgeCondition edge (N, M);
    Holes holes (RectangleHole(N/2, M/4, N/2 + N/3, (3*M)/4));
    HoleCondition hole_cond(zero, N, M);

    edge.zero(zero);
    edge.left(left);
    edge.right(right);
    edge.top(top);
    edge.bottom(bottom);

    MPI_Comm_rank(comm, &rank);
    cout << "Got runk " << rank << endl;
    Jacoby_Hole jkb(holes, hole_cond, edge, comm, 1, 2, rank, N, M, Hi, Hj);
    jkb.next(); // Fill up borders;
    do {
        jkb.next();
    } while(jkb.eps() > Eps);
    Matrix<double> *result = jkb.sync_results();
    if (result != NULL) {
        print(*result);
    }
    MPI_Finalize();
}

int main(int argc, char *argv[])
{
    jacoby(argc, argv);
    return 0;
};


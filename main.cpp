
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
int run(int rank, int N, int M, int Hi, int Hj, double eps)
{
    auto zero =
        [](double x, double y) -> double { return sqrt(abs(x*x - y*y)); };
    auto left = [](double y) -> double { return sin(2*PI*y); };
    auto right = [](double y) -> double { return 1-cos(2*PI*y); };
    auto top = [](double x) -> double { return 0; };
    auto bottom = [](double x) -> double { return sin(2*PI*x); };
    SplitEdgeCondition edge (N, M);
    Holes holes (RectangleHole(N/4, M/4, N/2 + N/3, (3*M)/4));
    HoleCondition hole_cond(zero, N, M);
    MPI_Comm comm = MPI_COMM_WORLD;

    edge.zero(zero);
    edge.left(left);
    edge.right(right);
    edge.top(top);
    edge.bottom(bottom);

    Jacoby_Hole jkb(holes, hole_cond, edge, comm, 1, 2, rank, N, M, Hi, Hj);
    jkb.next(); // Fill up borders;
    do {
        jkb.next();
    } while(jkb.eps() > eps);
    Matrix<double> *result = jkb.sync_results();
    if (result == NULL) {
        return 0;
    }

    double summ = 0;
    for (size_t i = 0; i < result->N(); ++i) {
        for (size_t j = 0; j < result->M(); ++j) {
            if(!isnan((*result)[i][j])) {
                summ += (*result)[i][j];
            }
        }
    }
    cout << "Summ: " << summ << endl;

    return 0;
}
int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    MPI_Comm comm = MPI_COMM_WORLD;
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);
    int N = 0, M;
    do {
        ++N;
        M=size/N;
    } while (N < M);
    if (rank >= N*M) {
        MPI_Finalize();
        return 0;
    }

    int c;
    double eps = 0.1;
    int cell_size = 512;
    while ( (c = getopt(argc, argv, "n:e:o:")) != -1) switch (c){
    case 'n':
        cell_size = atoi(optarg);
        if (cell_size <=0 ) {
            MPI_Finalize();
            return 1;
        }
        break;
    case 'e':
        eps = atof(optarg);
        break;
    }

    run(rank, N*cell_size, M*cell_size, cell_size, cell_size, eps);

    MPI_Finalize();
    return 0;
}

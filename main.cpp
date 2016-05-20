#include "Jacoby-Holes.hpp"
#include "Heat.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <cmath>
#include <fstream>

using namespace std;
using namespace heat;

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
int draw (Matrix<double> *result, double time, const char *file);
int run(int rank, int N, int M, int Hi, int Hj, double eps,
        const char *file);
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
    const char *file = NULL;
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
    case 'o':
        file = optarg;
        break;
    }

    run(rank, N*cell_size, M*cell_size, cell_size, cell_size, eps, file);

    MPI_Finalize();
    return 0;
}
int run(int rank, int N, int M, int Hi, int Hj, double eps,
        const char *file)
{
    auto zero =
        [](double x, double y) -> double { return x+y - 1; };
    auto hole_edge =
        [](double x, double y) -> double { return 1-x+y; };
    auto left = [](double y) -> double { return sin(2*PI*y); };
    auto right = [](double y) -> double { return 1-cos(2*PI*y); };
    auto top = [](double x) -> double { return 0; };
    auto bottom = [](double x) -> double { return sin(2*PI*x); };
    SplitEdgeCondition edge (N, M);
    Holes holes (
            RectangleHole(N/4, M/8, 3*N/8, M/4),
            RectangleHole(N/8, 5*M/8, N/4, 6*M/8),
            RectangleHole(5*N/8, 6*M/8, 6*N/8, 7*M/8),
            RectangleHole(6*N/8, 3*M/8, 7*N/8, M/2),
            RectangleHole(3*N/8, M/2, N/2, 5*M/8),
            RectangleHole(3*N/8 - N/16, M/2 - M/16, N/2 - N/16, 5*M/8 - M/16));
    HoleCondition hole_cond(hole_edge, N, M);
    MPI_Comm comm = MPI_COMM_WORLD;

    edge.zero(zero);
    edge.left(left);
    edge.right(right);
    edge.top(top);
    edge.bottom(bottom);

    Jacoby_Hole jkb(holes, hole_cond, edge, comm, 1, 2, rank, N, M, Hi, Hj);
    double time = MPI_Wtime();
    jkb.next(); // Fill up borders;
    do {
        jkb.next();
    } while(jkb.eps() > eps);
    time = MPI_Wtime() - time;
    Matrix<double> *result = jkb.sync_results();
    if (result == NULL) {
        return 0;
    }

    return draw(result, time, file);
}
int draw (Matrix<double> *result, double time, const char *file)
{
    double summ = 0;
    double max = NAN, min = NAN;

    for (size_t i = 0; i < result->N(); ++i) {
        for (size_t j = 0; j < result->M(); ++j) {
            double v = (*result)[i][j];
            if(!isnan(v)) summ += v;
            if (max == NAN || max < v) max = v;
            if (min == NAN || min > v) min = v;
        }
    }
    cout.precision(4);
    cout << "Time: " << time << "; Summ: " << summ << endl;

    if(file == NULL) return 0;

    fstream f(file, ios_base::out);
    f << "P3" << endl;
    f << "# " << file << endl;
    f << result->N() << " " << result->M() << endl;
    f << 255 << endl;

    for (size_t i = 0; i < result->N(); ++i) {
        for (size_t j = 0; j < result->M(); ++j) {
            double v = (*result)[i][j];
            f << R(v) << " " << G(v) << " " << B(v) << endl;
        }
    }
    f.close();
    return 0;
}

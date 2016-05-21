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
int draw (Matrix<double> *result, double time, double T, const char *file);
int run(int rank, int N, int M, int Hi, int Hj, double eps, double T,
        const char *file);
int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    MPI_Comm comm = MPI_COMM_WORLD;
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);
    int N = 0, M;

    switch (size) {
    case 8:
        N=2;
        M=4;
        break;
    case 32:
        N=4;
        M=8;
        break;
    case 128:
        N=8;
        M=16;
        break;
    case 512:
        N=16;
        M=32;
    default:
        do {
            ++N;
            M=size/N;
        } while (N < M);
        break;
    }
    if (rank >= N*M) {
        MPI_Finalize();
        return 0;
    }

    int c;
    double eps = 0.1;
    int len = 512;
    const char *file = NULL;
    double T = 0;
    while ( (c = getopt(argc, argv, "n:e:o:t:")) != -1) switch (c){
    case 'n':
        len = atoi(optarg);
        if (len <=0 ) {
            MPI_Finalize();
            return 1;
        }
        break;
    case 'e':
        eps = atof(optarg);
        break;
    case 't':
        T = atof(optarg);
        break;
    case 'o':
        file = optarg;
        break;
    }

    run(rank, len, len, len/N, len/M, eps, T, file);

    MPI_Finalize();
    return 0;
}
int run(int rank, int N, int M, int Hi, int Hj, double eps, double T,
        const char *file)
{
    auto zero =
        [](double x, double y) -> double { return x+y - 1; };
    auto hole_edge =
        [](double x, double y) -> double { return 2; };
    auto left = [](double y) -> double { return 2*sin(2*PI*y); };
    auto right = [](double y) -> double { return 1-cos(2*PI*y); };
    auto top = [](double x) -> double { return 0; };
    auto bottom = [](double x) -> double { return 2*sin(2*PI*x); };
    auto f = [](double x, double y, double t) -> double {return 0;};

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

    Jacoby_Hole jkb(holes, hole_cond, f, edge, comm, 1, 2, rank, N, M, Hi, Hj);
    double time = MPI_Wtime();
    jkb.next(); // Fill up borders;
    do {
        jkb.next();
        if (rank == 0 && jkb.step() % 100 == 0) {
            cout << "Step: " << jkb.step() << "; Eps: " << jkb.eps() <<
                "; T: " << jkb.time() << endl;
        }
    } while((jkb.eps() > eps) && (T <= 0 || jkb.time() < T));
    time = MPI_Wtime() - time;
    Matrix<double> *result = jkb.sync_results();
    if (result == NULL) {
        return 0;
    }

    return draw(result, time, jkb.time(), file);
}
int draw (Matrix<double> *result, double time, double T, const char *file)
{
    double summ = 0;
    double max = NAN, min = NAN;

    for (size_t i = 0; i < result->N(); ++i) {
        for (size_t j = 0; j < result->M(); ++j) {
            double v = (*result)[i][j];
            if(!isnan(v)) summ += v;
            if (isnan(max) || max < v) max = v;
            if (isnan(NAN) || min > v) min = v;
        }
    }
    cout.precision(4);
    cout << "Time: " << time << "; Summ: " << summ << "; T: " << T << endl;

    if(file == NULL) return 0;

    fstream f(file, ios_base::out);
    f << "P3" << endl;
    f << "# " << file << endl;
    f << result->N() << " " << result->M() << endl;
    f << 255 << endl;

    for (size_t i = 0; i < result->N(); ++i) {
        for (size_t j = 0; j < result->M(); ++j) {
            double v = (*result)[i][j];
            v = (v - min)/(max - min);
            f << R(v) << " " << G(v) << " " << B(v) << endl;
        }
    }
    f.close();
    return 0;
}

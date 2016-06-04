#include "Chunk.hpp"
#include "Jacoby.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include <utility>
#include "Heat.hpp"
#include <omp.h>

using namespace std;
using namespace heat;

static pair<double, double> result (string filename, Chunk &chunk, double t,
                    const Functor *an, vector<double> *res);

int main (int argc, char *argv[])
{
    size_t N = 512, M = 512;
    string out, decomposition = ".";
    const Functor *Z = &Functor::get("zero1");
    const Functor *E = &Functor::get("edge1");
    const Functor *H = &Functor::get("hole1");
    const Functor *F = &Functor::get("foo1");
    const Functor *analitic = NULL;
    int c;

    double T = 0.03, t_step = -1, t=0;

    MPI_Init(&argc, &argv);
    MPI_Comm comm = MPI_COMM_WORLD;
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    while ( (c = getopt(argc, argv, "n:m:o:d:t:T:Z:E:H:F:a:")) != -1) {
        switch (c){
        case 'n':
            N = atoi(optarg);
            break;
        case 'm':
            M = atoi(optarg);
            break;
        case 'o':
            out = optarg;
            break;
        case 'd':
            decomposition = optarg;
            break;
        case 'Z':
            Z = &Functor::get(optarg);
            break;
        case 'E':
            E = &Functor::get(optarg);
            break;
        case 'H':
            H = &Functor::get(optarg);
            break;
        case 'F':
            F = &Functor::get(optarg);
            break;
        case 't':
            t_step = atof(optarg);
            break;
        case 'T':
            T = atof(optarg);
            break;
        case'a':
            analitic = &Functor::get(optarg);
            break;
        }
    }
    if (t_step < 0) {
        t_step = T*2;
        t = t_step;
    } else {
        t = 0;
    }

    stringstream lstream;
    lstream << decomposition << "/" << rank;

    fstream local (lstream.str());
    fstream global (decomposition + "/global");

    if (rank == 0) {
        cout <<"Preparing data..." <<endl;
    }
    Jacoby2 jacoby(N, M, *F);
    Chunk chunk(local, global, *Z, *E, *H, jacoby.T());
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0) {
        cout <<"Calculating, using " << omp_get_num_procs()
            << " OpenMP threads." <<endl;
        cout <<"Will stop on T=" << T <<endl;
        if (t_step < T) {
            cout <<"Will generate result on each t=" << t_step <<endl;
        }
    }

    fstream defs;
    if (rank == 0) {
        defs.open((out + "-eps.txt").c_str(), ios_base::out);
    }
    vector<double> results;
    if (!out.empty()) {
        result(out, chunk, jacoby.T(), analitic, NULL);
    };

    double time = MPI_Wtime();
    while (chunk.step()*jacoby.T() < T) {
        bool get_eps = false;
        if (chunk.step()*jacoby.T() > t && !out.empty()){
            get_eps = true;
            const Chunk::Values *v = chunk.result();
            if (v != NULL) {
                results = *v;
            }
            t += t_step;
        }
        chunk.step(jacoby);
        if (rank == 0 && chunk.step() % 100 == 0) {
            cout << "Step=" << chunk.step() << " T=" <<
                chunk.step() *jacoby.T() << endl;
        }
        if (get_eps){
            pair<double, double> def = result(out,
                    chunk, jacoby.T(), analitic, &results);
            if (rank == 0) {
                defs << "Step=" << chunk.step() << " T=" <<
                    chunk.step() *jacoby.T() << " Eps=" << def.first;
                if (analitic != NULL) {
                    defs << " Analytic distance="<< def.second;
                }
                defs << endl;
            }
        }
    }
    time = MPI_Wtime() - time;
    if (rank == 0) {
        cout.precision(5);
        cout <<"Finish on step " << chunk.step() << " with math-time=" <<
            chunk.step() * jacoby.T() << " and run-time=" << time <<
            "s (+/-" << MPI_Wtick() <<"s)"<< endl;
    }

    if (!out.empty()) {
        result(out, chunk, jacoby.T(), analitic, NULL);
    };

    MPI_Finalize();
    return 0;
}
static pair<double, double> result (string filename, Chunk &chunk, double t,
                    const Functor *an, vector<double> *res)
{
    const Chunk::Values *v = chunk.result();
    static size_t n = 0;
    if (v == NULL) return pair<double, double>(0,0);

    stringstream sname;
    stringstream aname;
    sname << filename << "-" << n <<".ppm";
    aname << filename << "-an-" << n++ <<".ppm";
    fstream f (sname.str().c_str(), ios_base::out);
    f << "P3" << endl;
    f << "# " << filename << endl;
    f << chunk.N() << " " << chunk.M() << endl;
    f << 255 << endl;
    fstream a;
    if (an != NULL) {
        a.open(aname.str(), ios_base::out);
        a << "P3" << endl;
        a << "# " << filename << endl;
        a << chunk.N() << " " << chunk.M() << endl;
        a << 255 << endl;
    }

    double min = NAN, max = NAN;
    min = 0;
    max = 2;

    size_t i = 0;
    double d_eps = 0, d_an = 0;
    for (Chunk::Values::const_iterator it = v->begin();
            it != v->end(); ++it, ++i) {
        double v = *it;
        v = (v - min)/(max - min);
        f << R(v) << " " << G(v) << " " << B(v) << endl;
        if (an != NULL){
            pair<double,double> pos= Vertex::dpos(i, chunk.N(), chunk.M());
            double v_an = (*an)(pos.first, pos.second, chunk.step()*t);
            if (!isnan(v)) {
                d_an = std::max(d_an, fabs(v_an - *it));
            }
            v_an = (v_an - min)/(max - min);
            a << R(v_an) << " " << G(v_an) << " " << B(v_an) << endl;
        }
        if (res != NULL && res->size() > 0) {
            if (!isnan(v) && !isnan((*res)[i]) ) {
                d_eps += square((*res)[i] - *it);
            }
        }
    }
    f.close();
    if (an != NULL) {
        a.close();
    }
    cout.precision(5);
    return pair<double, double>(sqrt(d_eps), d_an);
}

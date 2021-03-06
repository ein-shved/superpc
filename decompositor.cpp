#include <unistd.h>
#include "Decompositor.hpp"
#include "Params.hpp"

int main (int argc, char *argv[])
{
    Decompositor d;
    char c;

    while ( (c = getopt(argc, argv, "n:m:p:f:o:P:")) != -1) switch (c){
    case 'n':
        d.N(atoi(optarg));
        break;
    case 'm':
        d.M(atoi(optarg));
        break;
    case 'f':
    case 'o':
        d.path(optarg);
        break;
    case 'p':
        d.proc(atoi(optarg));
        break;
    case 'P':
        d.hole(HoleParams::get(optarg)->hole());
        break;

    }

    return d.run();
}

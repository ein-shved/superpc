#include "Heat.hpp"
#include <iostream>
#include <fstream>

using namespace std;
using namespace heat;

int main ()
{
    fstream v("vert.ppm", ios_base::out);

    v << "P3" << endl;
    v << "# " << "vert.ppm" << endl;
    v << "16 256" << endl;
    v << 255 << endl;


    for (size_t i = 0; i < 256; ++i)
        for (size_t j = 0; j < 16; ++ j)
        {
            v << R(i/255.) << " " << G(i/255.) << " " << B(i/255.) <<endl;
        }
    v.close();

    fstream h("horisontal.ppm", ios_base::out);

    h << "P3" << endl;
    h << "# " << "horisontal.ppm" << endl;
    h << "256 16" << endl;
    h << 255 << endl;
    for (size_t j = 0; j < 16; ++ j)
        for (size_t i = 0; i < 256; ++i)
        {
            h << R(i/255.) << " " << G(i/255.) << " " << B(i/255.) <<endl;
        }
    h.close();
    return 0;
}

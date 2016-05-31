
#ifndef _DECOMPOSITOR_HPP_
#define _DECOMPOSITOR_HPP

#include "Hole.hpp"
#include "Matrix.hpp"
#include "Heat.hpp"
#include <string>
#include <metis.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <set>

class Decompositor {

public:
    typedef Matrix<double>::position position;
public:
    Decompositor()
        : m_hole(new Holes())
        , m_N(256), m_M(256)
        , m_path(".")
        , m_proc(1)
    {}
    ~Decompositor()
    {
        delete m_hole;
    }
    void hole(const Hole *hole)
    {
        if (m_hole != NULL) {
            Holes *h = new Holes (m_hole, hole);
            delete m_hole;
            m_hole = h;
        } else {
            m_hole = hole == NULL ? NULL : hole->copy();
        }
    }
    void N(size_t in_N)
    {
        m_N = in_N;
    }
    void M(size_t in_M)
    {
        m_M = in_M;
    }
    void dimetion (size_t N, size_t M)
    {
        m_N = N;
        m_M = M;
    }
    template <typename T>
    void path (T path)
    {
        m_path = path;
    }
    void proc (size_t proc)
    {
        m_proc = proc;
    }
    int run() const
    {
        idx_t nvtxs = m_N * m_M;
        idx_t ncon = 1;
        idx_t nedjs = nvtxs * 2 - m_N - m_M;
        idx_t *xadj = new idx_t [nvtxs+1];
        idx_t *vwgt = new idx_t [nvtxs];
        idx_t *adjncy = new idx_t [2*nedjs];
        idx_t nparts = m_proc;
        idx_t o_objval;
        idx_t *o_part = new idx_t [nvtxs];
        bool hole;

        xadj[0] = 0;
        for (size_t i=0, j = 0; i< m_N*m_M; ++i) {
            position p = pos(i), n_p;
            hole = m_hole->contains(((double)p.first)/m_N,
                    ((double)p.second)/m_M);
            if (hole) {
                xadj[i+1]  = j;
                vwgt[i] = 0;
                continue;
            }
            ssize_t nb;
            if (( nb = index(position(p.first - 1, p.second))) >= 0) {
                adjncy[j] = nb;
                ++j;
            }
            if (( nb = index(position(p.first + 1, p.second))) >= 0) {
                adjncy[j] = nb;
                ++j;
            }
            if (( nb = index(position(p.first, p.second - 1))) >= 0) {
                adjncy[j] = nb;
                ++j;
            }
            if (( nb = index(position(p.first, p.second + 1))) >= 0) {
                adjncy[j] = nb;
                ++j;
            }
            xadj[i+1]  = j;
            vwgt[i] = 1;
        }

#if 0
        std::cout << "xadj[";
        for (idx_t i = 0; i <= nvtxs; ++i) {
            std::cout << xadj[i] << ",";
        }
        std::cout << "]" <<std::endl;
        std::cout << "adjncy[";
        for (idx_t i = 0; i < 2*nedjs; ++i) {
            std::cout << adjncy[i] << ",";
        }
        std::cout << "]" << std::endl;
#endif

        std::cout << "Running decompositor with nvtxs=" << nvtxs << " nedjs=" <<
            nedjs << " nparts=" << nparts << std::endl;
        int result = METIS_PartGraphKway(&nvtxs, &ncon, xadj, adjncy, vwgt,
                NULL, NULL, &nparts, NULL, NULL, NULL, &o_objval, o_part);
        std::cout << "Decomposition done with result='" << str_result(result)
            <<"' (" << result << ")" << std::endl;
        if (result != METIS_OK) return result;

        std::fstream global (m_path + "/global", std::ios_base::out);
        std::fstream ppm (m_path + "/map.ppm", std::ios_base::out);
        global << m_N << " "<< m_M << std::endl;
        ppm << "P3" << std::endl;
        ppm << "# " << "map.ppm" << std::endl;
        ppm << m_N << " " << m_M << std::endl;
        ppm << 255 << std::endl;
        std::fstream ranks[m_proc];
        std::set<idx_t> empty;
        for (size_t i=0; i< m_proc; ++i) {
            std::stringstream name;
            name << m_path << "/" << i;
            ranks[i].open(name.str(), std::ios_base::out);
            ranks[i] << i << std::endl;
        }
        for (size_t i = 0; i < (size_t)nvtxs; ++i) {
            position p = pos(i);
            hole = m_hole->contains(((double)p.first)/m_N,
                    ((double)p.second)/m_M);
            if (hole) {
                global << -1 << std::endl;
                ppm << 255 << " " << 255 << " " << 255 << std::endl;
            } else {
                global << o_part[i] << std::endl;
                ranks[o_part[i]] << i << std::endl;
                ssize_t n1 =index(position(p.first - 1, p.second));
                ssize_t n2 =index(position(p.first + 1, p.second));
                ssize_t n3 =index(position(p.first, p.second - 1));
                ssize_t n4 =index(position(p.first, p.second + 1));
                if (n1 < 0 || n2 < 0 || n3 < 0 || n4 < 0 ||
                    o_part[i] != o_part[n1] ||
                    o_part[i] != o_part[n2] ||
                    o_part[i] != o_part[n3] ||
                    o_part[i] != o_part[n4])
                {
                    ppm << 0 << " " << 0 << " " << 0 << std::endl;
                } else {
                    double v = o_part[i];
                    v /= m_proc;
                    ppm << heat::R(v) << " " << heat::G(v) << " " << heat::B(v) << std::endl;
                }
            }
        }
        for (size_t i=0; i< m_proc; ++i) {
            ranks[i].close();
        }
        global.close();
        std::cout << "Extra procs: " << empty.size() << std::endl;

        delete [] xadj;
        delete [] adjncy;
        delete [] o_part;
        return 0;
    }
    static const std::string &str_result(int result)
    {
        static const std::string ok ("METIS_OK");
        static const std::string in ("METIS_ERROR_INPUT");
        static const std::string mem ("METIS_ERROR_MEMORY");
        static const std::string er ("METIS_ERROR");
        static const std::string none ("NONE");

        switch (result) {
        case METIS_OK:
            return ok;
        case METIS_ERROR_INPUT:
            return in;
        case METIS_ERROR_MEMORY:
            return mem;
        case METIS_ERROR:
            return er;
        default:
            return none;
        }
    }
    position pos(size_t index) const
    {
        position res = {index / m_M, index %m_M};
        return res;
    }
    ssize_t index (const position &pos) const
    {
        bool hole;
        hole = m_hole->contains(((double)pos.first)/m_N,
                ((double)pos.second)/m_M);
        if (hole) {
            return -1;
        }
        if (pos.first < 0 || pos.second < 0 || pos.first >= (ssize_t)m_N ||
                pos.second >= (ssize_t)m_M)
        {
            return -1;
        }
        size_t res = pos.first * m_M + pos.second;
        return res;
    }
    void color (int val, int c[3]) const
    {
        if (val < 0) {
            c[0] = c[1] = c[2] = 255;
            return;
        }
        c[0] = c[1] = c[2] = 16;
        for (size_t i = 0; i<m_proc/3; ++i) {
            c[val % 3] += 1;
            val /= 3;
        }
        double mul = 255. / m_proc;
        c[0]=(c[0]*mul + m_proc/4);
        c[1]=(c[1]*mul + m_proc/4);
        c[2]=(c[2]*mul + m_proc/4);
        c[0] %= 255;
        c[1] %= 255;
        c[2] %= 255;
    }
private:
    Hole *m_hole;
    size_t m_N, m_M;
    std::string m_path;
    size_t m_proc;
};



#endif /* Decompositor.hpp */

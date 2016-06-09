#ifndef _NEIGHBOUR_HPP_
#define _NEIGHBOUR_HPP_

#include "Vertex.hpp"
#include "Params.hpp"

#include <map>
#include <set>
#include <vector>
#include <mpi.h>

class Chunk;
class Neighbour {
    typedef std::set<NeighbourVertex> NeighborVertexSet;
    typedef std::set<const Vertex *> PtrVertexSet;
    typedef std::vector<double> DoubleVector;
public:
    Neighbour (Chunk &chunk, int rank, const Functor &zero);
    Neighbour (const Neighbour &other);
    ~Neighbour ();
    size_t N() const;
    size_t M() const;
    size_t &step() const;

    void add (size_t index, const Vertex *inner, Vertex::Direction d) const;
    int operator < (int rank) const;
    int operator == (int rank) const;
    int operator > (int rank) const;
    int operator < (const Neighbour &other) const;
    int operator == (const Neighbour &other) const;
    int operator > (const Neighbour &other) const;

    template <typename F>
    bool try_step(const F &f) const
    {
        int flag;
        if (m_step >= step()) return false;
        if (m_step == 0) {
            //sync();
            finish(f);
            return true;
        }
        MPI_Test(&m_rcv_req, &flag, MPI_STATUS_IGNORE);
        if (!flag) return false;
        sync();
        if (!sync_shared()) {
            return false;
        }
        finish(f);
        return true;
    }
    template <typename F>
    void step(const F &f) const
    {
        if (m_step >= step()) return;
        if (m_step == 0) {
            calc(f);
            finish_send();
            return;
        }
        MPI_Wait(&m_rcv_req, MPI_STATUS_IGNORE);
        sync();
        sync_shared(true);
        finish(f);
    }
private:
    template <typename F>
    void calc (F f) const
    {
        ++m_step;
        for (PtrVertexSet::iterator it = m_border.begin(); it != m_border.end();
                ++it)
        {
            (*it)->set(f((*it)->get(),(*it)->top()->get(), (*it)->right()->get(),
                        (*it)->bottom()->get(), (*it)->left()->get(), m_step,
                        (*it)->x(), (*it)->y()));
        }
    }
    template <typename F>
    void finish(F f) const
    {
        finish_rcv();
        calc(f);
        finish_send();
    }
    void finish_rcv() const;
    void finish_send() const;
    void check_shared() const;
    void check_shared(const Vertex *v) const;
    bool sync_shared(bool wait = false) const;
    bool sync_shared(bool wait, const Vertex *inner) const;
    bool synced(bool wait = false) const;
    void sync() const;

private:
    Chunk &m_chunk;
    int m_rank;
    mutable NeighborVertexSet m_vertecies;
    mutable PtrVertexSet m_border, m_shared;
    mutable DoubleVector m_send;
    mutable DoubleVector m_receive;
    const Functor &m_zero;
    mutable size_t m_step;
    mutable MPI_Request m_snd_req, m_rcv_req;
    mutable bool m_synced;
};

#endif /* Neighbour.hpp */

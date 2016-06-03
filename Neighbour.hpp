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
    Neighbour (Chunk &chunk, int rank, const Functor &zero)
        : m_chunk (chunk)
        , m_rank (rank)
        , m_zero(zero)
    {}
    Neighbour (const Neighbour &other)
        : m_chunk (other.m_chunk)
        , m_rank (other.m_rank)
        , m_zero(other.m_zero)
    {}
    size_t N() const;
    size_t M() const;
    size_t &step() const;

    void add (size_t index, const Vertex *inner, Vertex::Direction d) const;
    void notify (NeighbourVertex *my, Vertex *inner) const;
    void init();
    void next() const;
    int operator < (int rank) const;
    int operator == (int rank) const;
    int operator > (int rank) const;
    int operator < (const Neighbour &other) const;
    int operator == (const Neighbour &other) const;
    int operator > (const Neighbour &other) const;

private:
    Chunk &m_chunk;
    int m_rank;
    mutable NeighborVertexSet m_vertecies;
    mutable PtrVertexSet m_border;
    mutable DoubleVector m_send;
    mutable DoubleVector m_receive;
    const Functor &m_zero;
};

#endif /* Neighbour.hpp */

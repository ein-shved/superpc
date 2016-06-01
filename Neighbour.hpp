#ifndef _NEIGHBOUR_HPP_
#define _NEIGHBOUR_HPP_

#include "Vertex.hpp"

#include <map>
#include <vector>
#include <mpi.h>

class Chunk;
class Neighbour {
public:
    Neighbour (Chunk &chunk, int rank)
        : m_chunk (chunk)
        , m_rank (rank)
        , m_degree(0)
    {}
    size_t N();
    size_t M();
    size_t &step();

    void add (size_t index);
    void notify (NeighbourVertex *my, Vertex *inner);
    void init();
    void next();
private:
    Chunk &m_chunk;
    int m_rank;
    std::map<size_t, NeighbourVertex> m_vertecies;
    std::vector<double> m_send[2];
    std::vector<double> m_receive;
    std::map<size_t, size_t> m_indexes;
    size_t m_degree;
};

#endif /* Neighbour.hpp */

#include "Chunk.hpp"

#include <algorithm>
#include <iostream>
#include <omp.h>

using namespace std;

Chunk::Chunk(istream &chunkfile, istream &fullfile,
                const Functor &zero, const Functor &edge, const Functor &hole,
                double t)
    : m_step(0)
    , f_edge(edge)
    , f_hole(hole)
    , f_zero(zero)
    , m_T(t)
{
    size_t index;
    chunkfile >> m_rank;
    fullfile >> m_N;
    fullfile >> m_M;
    fullfile >> m_chunks;

    ItMapping mapping(m_N*m_M,m_vertecies.end());
    while(!chunkfile.eof()) {
        chunkfile >> index;
        VertexSet::iterator it = m_vertecies.insert(
                Vertex(index, m_N, m_M, m_step)).first;
        it->set(f_zero(it->x(), it->y(), 0));
        it->set(it->get(0), 1);
        mapping[index] = it;
    }
    m_decomposition.resize(m_N*m_M);
    for (size_t i = 0; (i < m_N*m_M) && !fullfile.eof(); ++i) {
        fullfile >> m_decomposition[i];
    }
    connect(m_decomposition, mapping);
    if (m_rank != 0) {
        m_decomposition.resize(0);
    }
    for (VertexSet::iterator it = m_vertecies.begin();
            it != m_vertecies.end(); ++it)
    {
        m_iterators.push_back(it);
    }

}
void Chunk::connect(const std::vector<int> &decomposition, const ItMapping &map)
{
    for (VertexSet::iterator it = m_vertecies.begin(); it != m_vertecies.end();
            ++it)
    {
        connect (decomposition, map, *it);
    }
}
void Chunk::connect(const Mapping &decomposition, const ItMapping &map,
        const Vertex &v)
{
    connect (decomposition, map, v, v.i() - 1, v.j(), Vertex::Left);
    connect (decomposition, map, v, v.i() + 1, v.j(), Vertex::Right);
    connect (decomposition, map, v, v.i(), v.j() - 1, Vertex::Top);
    connect (decomposition, map, v, v.i(), v.j() + 1, Vertex::Bottom);
}
void Chunk::connect(const std::vector<int> &decomposition, const ItMapping &map,
        const Vertex &v, size_t i, size_t j, Vertex::Direction d)
{
    size_t index = Vertex::index(i,j,m_N,m_M);
    int rank = decomposition[index];

    if (rank < 0) {
        connect_edge(v, d, index, rank);
    } else {
        if (rank != m_rank) {
            connect_neighbor(v, d, index, rank);
        } else {
            connect_inner(v, d, index, map);
        }
    }
}
void Chunk::connect_edge(const Vertex &v, Vertex::Direction d, size_t index,
        int rank)
{
    CondVertexSet::iterator it = m_hole.insert(
            CondVertex<Functor>(index, m_N, m_M, m_step,m_T,
            rank < -1 ? f_hole : f_edge,
            rank < - 1 ? Vertex::Hole : Vertex::Edge)).first;
    if (it == m_hole.end()) {
        exit(1);
    }
    v.set(*it, d);
    it->set(v, Vertex::reverse(d));
}
void Chunk::connect_neighbor(const Vertex &v, Vertex::Direction d, size_t index,
        int rank)
{
    NeighbourSet::iterator it = m_neighbors.insert(Neighbour(*this, rank,
                f_zero)).first;
    if (it == m_neighbors.end()) {
        exit(1);
    }
    it->add(index, &v, d);
}
void Chunk::connect_inner(const Vertex &v, Vertex::Direction d, size_t index,
        const ItMapping &map)
{
    VertexSet::iterator it = map[index];
    if (it == m_vertecies.end()) exit(1);
    v.set(*it, d);
    it->set(v, Vertex::reverse(d));
}
size_t Chunk::N() const
{
    return m_N;
}
size_t Chunk::M() const
{
    return m_M;
}
size_t &Chunk::step() const
{
    return m_step;
}
const Chunk::Values *Chunk::result()
{
    if (m_rank != 0) {
        result_slave();
        return NULL;
    }
    result_master();
    return &m_result;
}
void Chunk::result_slave()
{
    Values vals;
    result (vals);

    MPI_Send(vals.data(), vals.size(), MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
}
void Chunk::result_master()
{
    typedef pair<size_t, Values>ResPair;
    vector<ResPair> rcv(m_chunks, ResPair(0,Values()));
    m_result.resize(m_N*m_M);
    for (size_t i = 0; i < m_decomposition.size(); ++i) {
        int rank = m_decomposition[i];
        if (rank < 0) continue;
        rcv[rank].first++;
    }
    for (int i = 0; i < (int)m_chunks; ++i) {
        rcv[i].second.resize(rcv[i].first, 0);
        rcv[i].first = 0;
        if (i == m_rank) {
            result (rcv[i].second);
            continue;
        }
        result_master(rcv[i].second, i);
    }
    for (size_t i = 0; i < m_decomposition.size(); ++i) {
        int rank = m_decomposition[i];
        if (rank< 0) {
            m_result[i] = NAN;
            continue;
        }
        m_result[i] = rcv[rank].second[rcv[rank].first];
        rcv[rank].first++;
    }
}
void Chunk::result_master(Values &vals, int rank)
{
    double buf [vals.size()];
    memset (buf, 0, sizeof (buf));
    if (MPI_Recv(buf, vals.size(), MPI_DOUBLE, rank, 1, MPI_COMM_WORLD,
            MPI_STATUS_IGNORE) < 0)
    {
        exit(1);
    }
    for (size_t i=0; i<vals.size(); ++i) {
        vals[i] = buf[i];
    }
}
void Chunk::result (Values &vals)
{
    vals.resize(m_vertecies.size());
    size_t i=0;
    for (VertexSet::iterator it = m_vertecies.begin();
            it != m_vertecies.end(); ++i, ++it)
    {
        vals[i] = *it;
    }
}

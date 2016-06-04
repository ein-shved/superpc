#include <algorithm>
#include <iostream>

#include "Neighbour.hpp"
#include "Chunk.hpp"

using namespace std;

Neighbour::Neighbour (Chunk &chunk, int rank, const Functor &zero)
    : m_chunk (chunk)
    , m_rank (rank)
    , m_zero(zero)
    , m_step(chunk.step())
    , m_snd_req(MPI_REQUEST_NULL)
    , m_rcv_req(MPI_REQUEST_NULL)
{
}
Neighbour::Neighbour (const Neighbour &other)
    : m_chunk (other.m_chunk)
    , m_rank (other.m_rank)
    , m_zero(other.m_zero)
    , m_step(other.m_step)
    , m_snd_req(MPI_REQUEST_NULL)
    , m_rcv_req(MPI_REQUEST_NULL)
{
}
Neighbour::~Neighbour()
{
}

size_t Neighbour::N() const
{
    return m_chunk.N();
}
size_t Neighbour::M() const
{
    return m_chunk.M();
}
size_t &Neighbour::step() const
{
    return m_chunk.step();
}
void Neighbour::add (size_t index, const Vertex *inner, Vertex::Direction d) const
{
    NeighborVertexSet::iterator ptr = m_vertecies.insert(
            NeighbourVertex(index, N(), M(), step(), *this)).first;

    if (ptr == m_vertecies.end()) {
        exit(1);
    }
    if (m_border.insert(inner).first == m_border.end()) {
        exit(1);
    }
    inner->set(*ptr, d);
    ptr->set(inner, Vertex::reverse(d));
    ptr->set(m_zero(ptr->x(), ptr->y(), 0));
    m_send.resize(m_border.size());
    m_receive.resize(m_vertecies.size());
}
int Neighbour::operator < (int rank) const
{
    return m_rank < rank;
}
int Neighbour::operator == (int rank) const
{
    return m_rank == rank;
}
int Neighbour::operator > (int rank) const
{
    return m_rank > rank;
}
int Neighbour::operator < (const Neighbour &other) const
{
    return m_rank < other.m_rank;
}
int Neighbour::operator == (const Neighbour &other) const
{
    return m_rank == other.m_rank;
}
int Neighbour::operator > (const Neighbour &other) const
{
    return m_rank > other.m_rank;
}
void Neighbour::finish_rcv() const
{
    size_t i = 0;
    if (m_step > 0) {
        for (NeighborVertexSet::iterator it = m_vertecies.begin();
                it != m_vertecies.end(); ++it, ++i)
        {
            it->set(m_receive[i]);
        }
    }
    MPI_Irecv(m_receive.data(), m_receive.size(),MPI_DOUBLE, m_rank, 0,
            MPI_COMM_WORLD, &m_rcv_req);
}
void Neighbour::finish_send() const
{
    size_t i = 0;
    if (m_snd_req != MPI_REQUEST_NULL) {
        MPI_Wait(&m_snd_req, MPI_STATUS_IGNORE);
    }
    for (PtrVertexSet::iterator it = m_border.begin(); it != m_border.end();
            ++it, ++i)
    {
        m_send[i] = (*it)->get(0);
    }
    MPI_Isend(m_send.data(), i, MPI_DOUBLE, m_rank, 0, MPI_COMM_WORLD,
            &m_snd_req);
}

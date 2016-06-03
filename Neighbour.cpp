#include <algorithm>
#include <iostream>

#include "Neighbour.hpp"
#include "Chunk.hpp"

using namespace std;

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
void Neighbour::notify (NeighbourVertex *my, Vertex *inner) const
{
    /* TODO */
}
void Neighbour::init()
{
    /* TODO */
}
void Neighbour::next() const
{
    size_t i =0;
    for (PtrVertexSet::iterator it = m_border.begin(); it != m_border.end();
            ++it, ++i)
    {
        m_send[i] = (*it)->get(0);
    }
    if (MPI_Sendrecv(m_send.data(), i, MPI_DOUBLE, m_rank, 0, m_receive.data(),
            m_receive.size(),  MPI_DOUBLE, m_rank, 0, MPI_COMM_WORLD,
            MPI_STATUS_IGNORE) < 0)
    {
        exit(1);
    }
    i=0;
    for (NeighborVertexSet::iterator it = m_vertecies.begin();
            it != m_vertecies.end(); ++it, ++i)
    {
        it->set(m_receive[i]);
    }
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

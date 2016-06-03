#ifndef _CHUNK_HPP_
#define _CHUNK_HPP_

#include <istream>
#include <vector>
#include <set>

#include "Vertex.hpp"
#include "Neighbour.hpp"
#include "Params.hpp"

class Chunk {
public:
    typedef std::set<Vertex> VertexSet;
    typedef std::set<Neighbour> NeighbourSet;
    typedef std::set<CondVertex<Functor> > CondVertexSet;
    typedef std::vector<double> Values;
    typedef std::vector<int> Mapping;
    typedef std::vector<VertexSet::iterator> ItMapping;
    typedef std::vector<VertexSet::iterator> Iterators;
public:
    Chunk (std::istream &chunkfile, std::istream &fullfile,
            const Functor &zero, const Functor &edge, const Functor &hole,
            double t);

    template <typename F>
    size_t step(const F &f)
    {
        int size = m_iterators.size();

        ++m_step;
#pragma omp parallel
        {
#pragma omp for
            for (int i = 0; i<size; ++i){
                VertexSet:: iterator &it = m_iterators[i];
                it->set(f(*it, *it->top(), *it->right(), *it->bottom(),
                            *it->left(), m_step, it->x(), it->y()));
            }
        }
        for (NeighbourSet::iterator it = m_neighbors.begin();
                it != m_neighbors.end(); ++it)
        {
            it->next();
        }
        return m_step;
    }
    size_t N() const;
    size_t M() const;
    size_t &step() const;

    const Values *result();
private:
    void connect(const Mapping &decomposition, const ItMapping &);
    void connect(const Mapping &decomposition, const ItMapping &,
            const Vertex &v);
    void connect(const Mapping &decomposition, const ItMapping &,
            const Vertex &v,
            size_t i, size_t j, Vertex::Direction d);
    void connect_edge(const Vertex &v, Vertex::Direction, size_t index,
                        int rank);
    void connect_neighbor(const Vertex &v, Vertex::Direction, size_t index,
                        int rank);
    void connect_inner(const Vertex &v, Vertex::Direction, size_t index,
            const ItMapping &);
    void result_slave();
    void result_master();
    void result_master(Values &vals, int rank);
    void result (Values &vals);
private:
    size_t m_N, m_M, m_chunks;
    int m_rank;
    mutable size_t m_step;
    VertexSet m_vertecies;
    NeighbourSet m_neighbors;
    CondVertexSet m_hole;
    const Functor &f_edge, &f_hole, &f_zero;
    double m_T;
    Values m_result;
    Mapping m_decomposition;
    Iterators m_iterators;
};

#endif /* Chunk.hpp */

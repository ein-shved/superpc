#include "Vertex.hpp"
#include "Neighbour.hpp"

size_t Vertex::degree(const ::Neighbour *nb)
{
    size_t result = 0;
    for (size_t i = 0; i < 4; ++i) {
        Vertex *v = neighbor((Direction)i);
        if (v == NULL) continue;
        NeighbourVertex *nv = (NeighbourVertex *)v;
        if (v->m_type == Neighbour && nv->neighbour() == nb) {
            ++result;
        }
    }
    return result;
}

#include "Vertex.hpp"
#include "Neighbour.hpp"

void Vertex::notify_neighbor()
{
    if (m_type != Border) return;
    top()->notify(this);
    right()->notify(this);
    bottom()->notify(this);
    left()->notify(this);
}
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
void NeighbourVertex::notify (Vertex *nb)
{
    m_neighbour.notify(this, nb);
}

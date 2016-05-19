#ifndef _MPI_HPP_
#define _MPI_HPP_

#include <mpi.h>
#include "Exchanger.hpp"

class MPI_Exchanger : public Exchanger {
public:
    typedef Exchanger::matrix matrix;
    typedef Exchanger::line line;
    typedef Exchanger::raw raw;
    typedef Exchanger::column column;
    typedef Exchanger::value_type value_type;
    typedef Exchanger::reference reference;
    typedef Exchanger::pointer pointer;
    typedef Exchanger::const_reference const_reference;
    typedef Exchanger::const_pointer const_pointer;
    typedef Exchanger::position position;
    typedef Exchanger::edge_t edge_t;

public:
    MPI_Exchanger(const MPI_Comm &comm, size_t overlap, size_t len, 
                  size_t index, size_t N, size_t M,
                  size_t Hi, size_t Hj, 
                  const_reference val = value_type())
        : Exchanger(overlap, len, index, N, M, Hi, Hj, val)
        , m_comm(comm)
        , m_result (NULL)
    {}
    virtual ~MPI_Exchanger();

enum {
    tag_top = 0,
    tag_bottom = 1,
    tag_left = 2,
    tag_right = 3,
    tag_result = 4,
};

public:
    void send_top(size_t i = 0);
    void recv_top(size_t i = 0);
    void send_recv_top(size_t i = 0);

    void send_bottom(size_t i = 0);
    void recv_bottom(size_t i = 0);
    void send_recv_bottom(size_t i = 0);

    void send_left(size_t i = 0);
    void recv_left(size_t i = 0);
    void send_recv_left(size_t i = 0);

    void send_right(size_t i = 0);
    void recv_right(size_t i = 0);
    void send_recv_right(size_t i = 0);
    int index() const;
    int runk() const;
    matrix *sync_results();
    MPI_Comm comm() const;
private:
    int send (size_t i, const line &s, int dst, int tag);
    int recv (size_t i, line &r, int dst, int tag);
    int send_recv (size_t i, const line &s, int s_tag, line &r, int r_tag,
            int dst);
    matrix *sync_master();
    int sync_recv(matrix &rcv, size_t index);
    int sync_put(const matrix &rcv, size_t index);
    int sync_slave();

private:
    MPI_Comm m_comm;
    matrix *m_result;
};


#endif /* MPI_Exchanger.hpp */

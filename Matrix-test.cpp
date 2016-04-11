/*
 * Test of Matrix class
 */

#include "Matrix.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

template<typename T>
void print(const T &ref)
{
    for (auto val : ref) {
        cout << val << ' ';
    }
}
template<typename T, size_t N, size_t M>
void print(const Matrix<T, N, M> &m)
{
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < M; ++j) {
            cout << m[i][j] << " ";
        }
        cout << endl;
    }
}

int main()
{
    #define N 10
    #define M 20

    srand(time(0));

    Matrix<int, N, M> m(0);

    cout << "Matrix\n";
    m.foreach(std::function<void(int &)>([](int &val) { val = rand() % 10; }));
    print(m);

    cout << "T_Matrix\n";
    for (size_t j = 0; j < M; ++j) {
        print (m.get_column(j));
        cout << endl;
    }
};

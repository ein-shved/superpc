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
template<typename T>
void print(const Matrix<T> &m)
{
    for (size_t i = 0; i < m.N(); ++i) {
        for (size_t j = 0; j < m.M(); ++j) {
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

    for (size_t m = 0; m < M; m++) {
        for (size_t n = 0; n < N; n++) {
            cout << "Matrix " << n << " x " << m << endl;
            Matrix<int> mx(n, m,
                    [](size_t, size_t) -> int { return rand() % 10; });
            cout << "Matrix\n";
            print(mx);

            cout << "T_Matrix\n";
            for (size_t j = 0; j < m; ++j) {
                print (mx.get_column(j));
                cout << endl;
            }
            if (m < M - 1 || n < N - 1) {
                cout << endl << endl;
            }
        }
    }
};

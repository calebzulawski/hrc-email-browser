#ifndef _SPARSE_HPP_
#define _SPARSE_HPP_

#include <unordered_map>
#include <exception>
#include <sstream>

template<typename T>
class SparseMatrix {
public:
    SparseMatrix()
        : SparseMatrix(0,0)
    {}

    SparseMatrix(size_t NR, size_t NC)
        : NR(NR)
        , NC(NC)
    {}

    T& operator()(size_t r, size_t c) {
        if (r >= NR || c >= NC) {
            std::stringstream ss;
            ss << "(" << r << ", " << c << ") is not within SparseMatrix of shape (" << NR << ", " << NC << ")";
            throw std::out_of_range(ss.str());
        }
        return storage[r + c * NR];
    }

    bool isZero(size_t r, size_t c) {
        if (storage.count(r + c * NR) == 0) {
            return true;
        } else if (storage[r + c * NR] == 0) {
            return true;
        } else {
            return false;
        }
    }

private:
    size_t NR;
    size_t NC;

    std::unordered_map<size_t, T> storage;
};

#endif /* _SPARSE_HPP_ */

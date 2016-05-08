#ifndef _LDA_HPP_
#define _LDA_HPP_

#include "sparse.hpp"
#include <random>
#include <vector>

class LDA {

public:

    LDA(uint K, double alpha = 0.1, double eta = 0.01, uint seed = 0);

    void process(const std::vector<size_t>& tokens,
                 const std::vector<size_t>& documents,
                 size_t n_tokens,
                 size_t n_docs,
                 uint epochs = 100);


private:

    void gibbsSample(const std::vector<size_t>& tokens,
                     const std::vector<size_t>& documents,
                     std::vector<size_t>& topics);

    //number of topics
    uint K;
    // alpha parameter for dirichlet prior
    double alpha;
    double eta;

    SparseMatrix<uint> topic_token;
    SparseMatrix<uint> doc_topic;
    std::vector<uint>  topic_count;
    std::vector<size_t>  topic_per_token;

    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution;
};

#endif /* _LDA_HPP_ */

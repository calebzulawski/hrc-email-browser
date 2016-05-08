#include "lda.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>

LDA::LDA(uint K, double alpha, double eta, uint seed)
    : K(K)
    , alpha(alpha)
    , eta(eta)
    , distribution(0.0, 1.0) // uniform random
{
    if (seed == 0)
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    generator = std::default_random_engine(seed);
}

void LDA::process(const std::vector<size_t>& tokens,
                  const std::vector<size_t>& documents,
                  size_t n_tokens,
                  size_t n_docs,
                  uint epochs) {

    // initialize some values
    topic_per_token.resize(tokens.size());
    topic_count.resize(K, 0);
    topic_token = SparseMatrix<uint>(K, n_tokens);
    doc_topic = SparseMatrix<uint>(n_docs, K);
    for (uint i = 0; i < tokens.size(); i++) {
        size_t topic = i % K;
        doc_topic(documents[i], topic)++;
        topic_token(topic, tokens[i])++;
        topic_count[topic]++;
        topic_per_token[i] = topic;
    }

    // cluster over epochs or until convergence
    for (uint i = 0; i < epochs || epochs == 0; i++) {
        std::cout << "Epoch " << i << "/" << epochs << std::endl;
        gibbsSample(tokens, documents, topic_per_token);
        // TODO: break on convergence
    }
}

void LDA::gibbsSample(const std::vector<size_t>& tokens,
                      const std::vector<size_t>& documents,
                      std::vector<size_t>& topics) {

    double dist_cum;
    auto dist_sum = new double[K];
    for (uint i = 0; i < tokens.size(); i++) {
        topic_token(topics[i], tokens[i])--;
        doc_topic(documents[i], topics[i])--;
        topic_count[i]--;

        dist_cum = 0;
        for (uint k = 0; k < K; k++) {
            dist_cum += (topic_token(topics[i], tokens[i]) + eta)
                      * (doc_topic(documents[i], topics[i]) + alpha)
                      / (topic_count[topics[i]] + eta * tokens.size());

            dist_sum[k] = dist_cum;
        }

        double r = distribution(generator) * dist_cum;
        size_t new_topic = std::upper_bound(dist_sum, dist_sum + K, r) - dist_sum;

        topics[i] = new_topic;
        topic_token(topics[i], tokens[i])++;
        doc_topic(documents[i], topics[i])++;
        topic_count[i]++;
    }
    delete[] dist_sum;
}

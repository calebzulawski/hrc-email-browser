#include "lda.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <queue>
#include <math.h>

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

void LDA::setInitialState(const std::vector<size_t>& tokens,
                          const std::vector<size_t>& documents,
                          size_t n_tokens,
                          size_t n_docs,
                          int num_tokens){
    //initalize values
    topic_per_token.resize(num_tokens);
    topic_count.resize(K, 0);
    topic_token = SparseMatrix<uint>(K, n_tokens);
    doc_topic = SparseMatrix<uint>(n_docs, K);
    for (uint i = 0; i < tokens.size(); i++) {
        //std::cout << documents[i] << " " <<  tokens[i] << std::endl;
        size_t topic = i % K;
        doc_topic(documents[i], topic)++;
        topic_token(topic, tokens[i])++;
        topic_count[topic]++;
        topic_per_token[i] = topic;
    }
}

void LDA::process(const std::vector<size_t>& tokens,
                  const std::vector<size_t>& documents,
                  size_t n_tokens,
                  size_t n_docs,
                  uint epochs) {

    // set initial state
    setInitialState(tokens,documents,n_tokens,n_docs,tokens.size());

    double conv_limit = 5e2/n_tokens;
    double last_ll = 0;
    double curr_ll = 0;
    int conv_count = 0;

    // cluster over epochs or until convergence
    for (uint i = 0; i < epochs || epochs == 0; i++) {
        std::cout << "Epoch " << i << "/" << epochs << std::endl;
        gibbsSample(tokens, documents, topic_per_token);
        curr_ll = loglikelihood(tokens,documents,n_tokens,n_docs);
        std::cout << "ll: " << fabs(curr_ll - last_ll) << std::endl;
        if (fabs(curr_ll - last_ll) < conv_limit){
            conv_count++;
            if (conv_count >= 50){
                break;
            }
        }
        else{
            conv_count = 0;
        }
        last_ll = curr_ll;
        std::cout << "conv_count: "<< conv_count << " " << conv_limit << std::endl;
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
        topic_count[topics[i]]--;

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
        topic_count[topics[i]]++;
    }
    delete[] dist_sum;
}

// Thanks to the user vog on stackoverflow!
std::string LDA::escape_json(const std::string &s) {
    std::ostringstream o;
    for (auto c = s.cbegin(); c != s.cend(); c++) {
        switch (*c) {
        case '"': o << "\\\""; break;
        case '\\': o << "\\\\"; break;
        case '\b': o << "\\b"; break;
        case '\f': o << "\\f"; break;
        case '\n': o << "\\n"; break;
        case '\r': o << "\\r"; break;
        case '\t': o << "\\t"; break;
        default:
            if ('\x00' <= *c && *c <= '\x1f') {
                o << "\\u"
                  << std::hex << std::setw(4) << std::setfill('0') << (int)*c;
            } else {
                o << *c;
            }
        }
    }
    return o.str();
}

void LDA::writeTokenJSON(std::vector<std::string> vocabulary, std::string filename) {
    std::ofstream json;
    json.open(filename);

    json << "{" << std::endl;
    json << "  \"vocabulary\": [";
    for (size_t token = 0; token < vocabulary.size(); token++) {
        if (token != 0)
            json << ", ";
        json << "\"" << escape_json(vocabulary[token]) << "\"";
    }
    json << "]," << std::endl;

    json << "  \"distribution\": {" << std::endl;
    auto values = new double[vocabulary.size()];
    double sum;
    for (size_t topic = 0; topic < K; topic++) {
        if (topic != 0)
            json << "," << std::endl;
        json << "    \"" << topic << "\": [";
        sum = 0;
        for (size_t token = 0; token < vocabulary.size(); token++) {
            values[token] = topic_token(topic, token) + eta;
            sum += values[token];
        }
        for (size_t token = 0; token < vocabulary.size(); token++) {
            if (token != 0)
                json << ", ";
            json << values[token]/sum;
        }
        json << "]";
    }
    json << std::endl << "  }" << std::endl << "}" << std::endl;
    json.close();
    delete[] values;
}

void LDA::writeDocumentJSON(std::vector<std::string> documents, std::string filename) {
    std::ofstream json;
    json.open(filename);
    json << "{" << std::endl;

    auto values = new double[K];
    double sum;
    for (size_t doc = 0; doc < documents.size(); doc++) {
        sum = 0;
        if (doc != 0)
            json << "," << std::endl;
        json << "  \"" << escape_json(documents[doc]) << "\": [";
        for (size_t topic = 0; topic < K; topic++) {
            values[topic] = doc_topic(doc, topic) + alpha;
            sum += values[topic];
        }
        for (size_t topic = 0; topic < K; topic++) {
            if (topic != 0)
                json << ", ";
            json << values[topic]/sum;
        }
        json << "]";
    }
    json << std::endl << "}" << std::endl;
    json.close();
    delete[] values;
}

double LDA::perplexity(const std::vector<size_t>& tokens,
                       const std::vector<size_t>& documents,
                       size_t n_tokens,
                       size_t n_docs,
                       uint epochs){
    // set initial state

    std::vector<size_t> training_tokens (tokens.begin(),tokens.begin()+tokens.size()/2);
    setInitialState(training_tokens,documents,n_tokens,n_docs,training_tokens.size());

    std::cout << "state 2" << std::endl;
    // cluster over epochs or until convergence
    for (uint i = 0; i < epochs || epochs == 0; i++) {
        std::cout << "Epoch " << i << "/" << epochs << std::endl;
        gibbsSample(training_tokens, documents, topic_per_token);
    }

    std::vector<size_t> holdout_tokens (tokens.begin()+tokens.size()/2,tokens.end());
    return loglikelihood(holdout_tokens,documents,n_tokens,n_docs);
}

double LDA::loglikelihood(const std::vector<size_t>& tokens,
                          const std::vector<size_t>& documents,
                          size_t n_tokens,
                          size_t n_docs){

    uint doc_count[n_docs] = {0};

    for (uint doc = 0; doc < n_docs; doc++) {
        for (uint topic = 0; topic < K; topic++) {
            if (!doc_topic.isZero(doc,topic))
                doc_count[doc] += doc_topic(doc,topic);
        }
    }

    double lg_alpha = std::lgamma(alpha);
    double lg_eta = std::lgamma(eta);
    double ll = K * std::lgamma(eta * n_tokens);

    for (uint topic = 0; topic < K; topic++) {
        ll -= std::lgamma(eta * n_tokens + topic_count[topic])/n_tokens;
        for (uint token = 0; token < n_tokens; token++) {
            if (!topic_token.isZero(topic, token))
                ll += (std::lgamma(eta + topic_token(topic,token)) - lg_eta)/n_tokens;
        }
    }

    for (uint doc = 0; doc < n_docs; doc++) {
        ll += (std::lgamma(alpha * K) - std::lgamma(alpha * K + doc_count[doc]))/n_tokens;
        for (uint topic = 0; topic < K; topic++) {
            if (!doc_topic.isZero(doc,topic))
                ll += (std::lgamma(eta + doc_topic(doc,topic)) - lg_alpha)/n_tokens;
        }
    }
    return ll;
}

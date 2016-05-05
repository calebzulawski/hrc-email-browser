#include <map>
#include <cmath>

typedef std::map<std::pair<uint, uint>, uint> Map_t;

double loglikelihood(Map_t& topic_word,
                     Map_t& doc_topic,
                     uint* topic_count,
                     uint n_docs,
                     uint n_topics,
                     uint n_words,
                     double alpha,
                     double eta)
{
    uint doc_count[n_docs] = {0};
    
    for (uint d = 0; d < n_docs; d++) {
        for (uint k = 0; k < n_topics; k++) {
            auto key = std::make_pair(d, k);
            if (doc_topic.count(key) != 0)
                doc_count[d] += doc_topic[key];
        }
    }        
    
    double lg_alpha = std::lgamma(alpha);
    double lg_eta = std::lgamma(eta);
    double ll = n_topics * std::lgamma(eta * n_words);
    
    for (uint k = 0; k < n_topics; k++) {
        ll -= std::lgamma(eta * n_words + topic_count[k]);
        for (uint w = 0; w < n_words; k++) {
            auto key = std::make_pair(k, w);
            if (topic_word.count(key) != 0)
                ll += std::lgamma(eta + topic_word[key]) - lg_eta;
        }
    }
    
    for (uint d = 0; d < n_docs; d++) {
        ll += std::lgamma(alpha * n_topics) - std::lgamma(alpha * n_topics + doc_count[d]);
        for (uint k = 0; k < n_topics; k++) {
            auto key = std::make_pair(d, k);
            if (doc_topic.count(key) != 0)
                ll += std::lgamma(eta + doc_topic[key]) - lg_alpha;
        }
    }
    return ll;    
}

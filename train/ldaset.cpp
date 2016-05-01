#include "ldaset.h"
#include <climits>
#include <cmath>

template <class T>
LDAset<T>::LDAset(int num_topics){
    K = num_topics;
    total_tokens = 0;
    srand(time(NULL));
    tokens_per_topic.resize(K,0);
    return;
}

template <class T>
LDAset<T>::~LDAset() {
    return;
}

template <class T>
void LDAset<T>::insertInitValue(T token, std::string doc) {

    // add document and token to list
    int curr_topic = rand()%K;
    token_list[token]++;
    doc_list[doc][token]++;
    doc_list_topics[doc][token] = curr_topic;
    tokens_per_doc[doc]++;
    tokens_in_topic[token][curr_topic]++;
    tokens_per_topic[curr_topic]++;
}

template <class T>
void LDAset<T>::process(int epochs){

    bool force_converge = !epochs;
    while(epochs || force_converge){
        for (auto &document: doc_list){
            std::string curr_doc = document.first;
            for (auto &token: document.second){
                int maxtopic = doc_list_topics[curr_doc][token];
                int maxprob = INT_MIN;
                int tokens_in_curr_doc = doc_list[curr_doc][token];
                tokens_in_topic[token][maxtopic] -= token_in_curr_doc;
                for (int curr_topic = 0; curr_topic < K; curr_topic++){
                    double p_t_d = log(tokens_in_curr_doc);
                    p_t_d -= log(tokens_per_doc[curr_doc]);
                    double p_w_t = log(tokens_in_topic[token][curr_topic] + token_in_curr_doc);
                    p_w_t -= log(tokens_per_topic[curr_topic]);

                    double total_prob = p_t_d * p_w_t;
                    if (total_prob > maxprob){
                        maxtopic = curr_topic;
                        maxprob = total_prob;
                        // readjust tokens_in_topic
                    }
                }
                tokens_in_topic[token][maxtopic] += token_in_curr_doc;
                doc_list_topics[curr_doc][token] = maxtopic;
            }
        }
    }

}

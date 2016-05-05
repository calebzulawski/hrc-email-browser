#include "ldaset.h"
#include <typeinfo>
#include <iomanip>

template <class T>
LDAset<T>::LDAset(int K_init, double alpha_init, double eta_init){
    alpha = alpha_init;
    eta = eta_init;
    K = K_init;
    tokens_per_topic.assign(K,0);

    srand(time(NULL));
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
    if(token_to_int.find(token) == token_to_int.end()){
        int mapped_int = token_to_int.size();
        token_to_int[token] = mapped_int;
        int_to_token[mapped_int] = token;
    }
    if(doc_to_int.find(doc) == doc_to_int.end()){
        int mapped_int = doc_to_int.size();
        doc_to_int[doc] = mapped_int;
        int_to_doc[mapped_int] = doc;
    }

    int doc_int = doc_to_int[doc];
    int token_int = token_to_int[token];

    doc_topic[std::make_pair(doc_int,curr_topic)]++;
    topic_token[std::make_pair(curr_topic,token_int)]++;
}

template <class T>
void LDAset<T>::process(int epochs, int gibbs_epochs){

    // cluster over epochs or until convergence
    bool force_converge = !epochs;
    while(epochs || force_converge){
        gibbsSample(gibbs_epochs);
        epochs--;
    }
}

template <class T>
void LDAset<T>::gibbsSample(int epochs){
    for (int doc = 0; doc < int_to_doc.size(); doc++){
        for (int token = 0; token < int_to_token.size(); token++){
            for (int topic = 0; topic < K; topic++){
                std::pair<int,int> dt = std::make_pair(doc,topic);
                std::pair<int,int> tw = std::make_pair(topic,token);
                if (doc_topic.find(dt) == doc_topic.end()
                || doc_topic[dt] == 0
                || topic_token.find(tw) == topic_token.end()
                || topic_token[tw] == 0
                )
                    continue;

                // decrement all current counts and resample token
                doc_topic[dt]--;
                topic_token[tw]--;
                tokens_per_topic[topic]--;

                double dist_cum = 0;
                std::vector<double> dist_sum;

                int new_topic = 0;
                for (new_topic = 0; new_topic < K; new_topic++){
                    std::pair<int,int> new_dt = std::make_pair(doc,new_topic);
                    std::pair<int,int> new_tw = std::make_pair(new_topic,token);

                    double tmp = (topic_token[new_tw] + eta)/(tokens_per_topic[new_topic]+ eta*int_to_token.size());
                    tmp *= (doc_topic[new_dt] + alpha);

                    dist_cum += tmp;
                    dist_sum.push_back(dist_cum);
                }

                new_topic = pullFromDist(dist_sum,dist_cum);

                // increment count for new token topic
                doc_topic[std::make_pair(doc,new_topic)]++;
                topic_token[std::make_pair(new_topic,token)]++;
                tokens_per_topic[new_topic]++;
            }
        }
    }
}

template <class T>
int LDAset<T>::pullFromDist(std::vector<double> dist, double sum){
    double r = ((double)rand()/(double)RAND_MAX) * sum;
    int i = 0;
    while (r > 0){
        r -= dist[i];
        i++;
    }
    return i - 1;
}

template <class T>
void LDAset<T>::setTotalTokenCount(int count){
    return;
}

template <class T>
void LDAset<T>::dumpResults(){
    return;
}

int main(){
    // LDAset<std::string> test_set(3);
    // std::cout << "created new LDAset\n";
    //
    // std::map<std::string,std::vector<std::string>> docset = {
    //     {"doc01", {"cat", "cat", "cat", "cat"}},
    //     {"doc02", {"dog", "dog", "dog", "dog"}},
    //     {"doc03", {"rat", "rat", "rat", "rat"}},
    //     {"doc04", {"dog", "dog", "dog", "dog"}},
    //     {"doc05", {"cat", "cat", "cat", "cat"}},
    //     {"doc06", {"rat", "rat", "rat", "rat"}},
    //     {"doc07", {"cat", "cat", "cat", "cat"}},
    //     {"doc08", {"dog", "dog", "dog", "dog"}},
    //     {"doc09", {"rat", "rat", "rat", "rat"}},
    //     {"doc10", {"dog", "dog", "dog", "dog"}},
    //     {"doc11", {"cat", "cat", "cat", "cat"}},
    //     {"doc12", {"rat", "rat", "rat", "rat"}},
    //     {"doc13", {"cat", "cat", "cat", "cat"}},
    //     {"doc14", {"dog", "dog", "dog", "dog"}},
    //     {"doc15", {"rat", "rat", "rat", "rat"}},
    //     {"doc16", {"dog", "dog", "dog", "dog"}},
    //     {"doc17", {"cat", "cat", "cat", "cat"}},
    //     {"doc18", {"rat", "rat", "rat", "rat"}},
    // };
    //
    // for (auto &doc: docset){
    //     for (auto &token: doc.second){
    //         test_set.insertInitValue(token,doc.first);
    //         std::cout << doc.first << " ";
    //         std::cout << token << " ";
    //         std::cout << test_set.doc_list_topics[doc.first][token] << "\n";
    //     }
    // }
    //
    // test_set.process(5);
    //
    // std::cout << "trained classes\n";
    // for (auto &doc: test_set.doc_list_topics){
    //     for (auto &token: doc.second){
    //         std::cout << doc.first << " ";
    //         std::cout << token.first << " ";
    //         std::cout << test_set.doc_list_topics[doc.first][token.first] << "\n";
    //     }
    // }
    //
    // test_set.dumpResults();
}

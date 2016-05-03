#include "ldaset.h"
#include <typeinfo>

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
    if (doc_list_topics[doc].find(token) == doc_list_topics[doc].end()){
        doc_list_topics[doc][token] = curr_topic;
    }
    else{
        curr_topic = doc_list_topics[doc][token];
    }
    doc_topic_spread[doc][curr_topic]++;
    tokens_per_doc[doc]++;
    tokens_in_topic[token][curr_topic]++;
    tokens_per_topic[curr_topic]++;
}

template <class T>
void LDAset<T>::process(int epochs){

    int num_docs = doc_list.size();
    double corpus_prob = 0;
    double last_prob = corpus_prob;


    // cluster over epochs or until convergence
    bool force_converge = !epochs;
    while(epochs || force_converge){

        for (auto &document: doc_list){
            std::string curr_doc = document.first;
            double doc_prob;
            for (auto &token_pair: document.second){
                T token = token_pair.first;
                int maxtopic = doc_list_topics[curr_doc][token];
                int maxprob = INT_MIN;
                int tokens_in_curr_doc = doc_list[curr_doc][token];
                tokens_in_topic[token][maxtopic] -= tokens_in_curr_doc;

                doc_topic_spread[curr_doc][maxtopic] -= tokens_in_curr_doc;

                for (int curr_topic = 0; curr_topic < K; curr_topic++){
                    double p_t_d = log(doc_topic_spread[curr_doc][curr_topic] + 1);
                    p_t_d -= log(tokens_per_doc[curr_doc]);
                    double p_w_t = log(tokens_in_topic[token][curr_topic] + tokens_in_curr_doc + 1);
                    p_w_t -= log(tokens_per_topic[curr_topic]);

                    double total_prob = p_t_d + p_w_t;
                    if (total_prob > maxprob){
                        maxtopic = curr_topic;
                        maxprob = total_prob;
                        // readjust tokens_in_topic
                    }
                }
                if (force_converge){
                    doc_prob += maxprob/doc_list[curr_doc].size();
                }

                doc_topic_spread[curr_doc][maxtopic] += tokens_in_curr_doc;
                tokens_in_topic[token][maxtopic] += tokens_in_curr_doc;
                doc_list_topics[curr_doc][token] = maxtopic;
            }
            if (force_converge){
                corpus_prob += doc_prob/doc_list.size();
            }
        }


        if (force_converge){
            if (abs(last_prob - corpus_prob) < 100){
                force_converge = false;
            }
            last_prob = corpus_prob;
            corpus_prob = 0;
        }

        epochs--;
    }

    // iterate through all documents and calculate most probable class
    std::vector<int> topic_counts (K,0);
    for (auto &document: doc_list){
        std::string curr_doc = document.first;
        int maxtopic = K;
        for (auto &token_pair: doc_list[curr_doc]){
            T token = token_pair.first;
            topic_counts[doc_list_topics[curr_doc][token]] += doc_list[curr_doc][token];
        }
        maxtopic = std::distance(topic_counts.begin(),std::max_element(topic_counts.begin(), topic_counts.end()));
        doc_topics[curr_doc] = maxtopic;

        topic_counts.assign(K,0);
    }

}

template <class T>
void LDAset<T>::setTotalTokenCount(int count){
    total_tokens = count;
}

template <class T>
void LDAset<T>::dumpResults(){
    for (auto doc: doc_topics){
        std::cout << doc.first << ": " << doc.second << "\n";
    }
}

int main(){
    LDAset<std::string> test_set(3);
    std::cout << "created new LDAset\n";

    std::map<std::string,std::vector<std::string>> docset = {
        {"doc1", {"cat", "cat", "cat", "cat"}},
        {"doc2", {"dog", "dog", "dog", "dog"}},
        {"doc3", {"rat", "rat", "rat", "rat"}},
        {"doc4", {"dog", "dog", "dog", "dog"}},
        {"doc5", {"cat", "cat", "cat", "cat"}},
        {"doc6", {"rat", "rat", "rat", "rat"}},
        {"doc7", {"cat", "cat", "cat", "cat"}},
        {"doc8", {"dog", "dog", "dog", "dog"}},
        {"doc9", {"rat", "rat", "rat", "rat"}},
        {"doc10", {"dog", "dog", "dog", "dog"}},
        {"doc11", {"cat", "cat", "cat", "cat"}},
        {"doc12", {"rat", "rat", "rat", "rat"}},
        {"doc13", {"cat", "cat", "cat", "cat"}},
        {"doc14", {"dog", "dog", "dog", "dog"}},
        {"doc15", {"rat", "rat", "rat", "rat"}},
        {"doc16", {"dog", "dog", "dog", "dog"}},
        {"doc17", {"cat", "cat", "cat", "cat"}},
        {"doc18", {"rat", "rat", "rat", "rat"}},
    };

    for (auto &doc: docset){
        for (auto &token: doc.second){
            test_set.insertInitValue(token,doc.first);
            std::cout << doc.first << " ";
            std::cout << token << " ";
            std::cout << test_set.doc_list_topics[doc.first][token] << "\n";
        }
    }

    test_set.process(5);

    std::cout << "trained classes\n";
    for (auto &doc: test_set.doc_list_topics){
        for (auto &token: doc.second){
            std::cout << doc.first << " ";
            std::cout << token.first << " ";
            std::cout << test_set.doc_list_topics[doc.first][token.first] << "\n";
        }
    }

    test_set.dumpResults();
}

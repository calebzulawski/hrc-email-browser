#include <iostream>
#include <cstdlib>
#include <map>
#include <vector>
#include <string>
#include <tuple>
#include <ctime>
#include <utility>


template <class T>
class LDAset {
//number of topics
int K;

//total number of tokens
int total_tokens;

std::map<T,std::map<int,int>> tokens_in_topic;
std::map<std::string,int> tokens_per_doc;
std::vector<int> tokens_per_topic;

std::map<T,int> token_list;
std::map<std::string,std::map<T,int>> doc_list;
std::map<std::string,std::map<T,int>> doc_list_topics;

//maps documents to topic
//output of process function will be stored here
std::map<std::string,int> doc_topics;

public:
    LDAset(int num_topics);
    ~LDAset();
    void insertInitValue(T token, std::string doc);
    void process(int epochs = 0);
};

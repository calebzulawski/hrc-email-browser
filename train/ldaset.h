#ifndef __LDASET_H
#define __LDASET_H

#include <iostream>
#include <cstdlib>
#include <map>
#include <vector>
#include <string>
#include <tuple>
#include <ctime>
#include <utility>
#include <climits>
#include <cmath>
#include <algorithm>
#include <iterator>
#include <list>

template <class T>
class LDAset {
//number of topics
int K;
// alpha parameter for dirichlet prior
double alpha;
double eta;

public:
    // remember to move these to be private variables after testing is complete
    int token_count;
    int doc_count;
    std::map<int,T> int_to_token;
    std::map<T,int> token_to_int;
    std::map<int,std::string> int_to_doc;
    std::map<std::string,int> doc_to_int;
    std::map<std::pair<int,int>,int> topic_token;
    std::map<std::pair<int,int>,int> doc_topic;
    std::vector<int> tokens_per_topic;

    LDAset(int num_topics, double alpha = 0.1, double eta = 0.01);
    ~LDAset();
    void insertInitValue(T token, std::string doc);
    void process(int epochs = 0, int gibbs_epochs = 100);
    void gibbsSample(int epochs = 100);
    int  pullFromDist(std::vector<double> dist, double sum);
    void setTotalTokenCount(int count);
    void dumpResults();
};

#endif

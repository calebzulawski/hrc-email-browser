#ifndef _PREPROCESS_HPP_
#define _PREPROCESS_HPP_

#include <mitie.h>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <array>

template<size_t N>
class Dataset {
public:
    // typedef std::array<std::string, N> NGRAM;
    typedef std::string NGRAM; // unigram

    std::vector<size_t> ngramIndices;
    std::vector<size_t> documentIndices;
    std::vector<NGRAM> vocabulary;
    std::vector<std::string> documents;
};

template<size_t N>
class Preprocessor {
public:
    // typedef std::array<std::string, N> NGRAM;
    typedef std::string NGRAM; // unigram

    void addDocument(const char* text, const char* document)
    {
        char** tokens = mitie_tokenize(text);

        std::string docstring = document;

        for (char** token = tokens; *token != nullptr; token++) {
            std::string tokenstring = *token;
            ngrams.push_back(tokenstring);
            documents.push_back(docstring);
        }

        // create n-grams
        // NGRAM ngram;
        // for (char** token = tokens; *token != nullptr; token++) {
        //     // shift n-gram
        //     for (uint i = 0; i < N - 1; i++)
        //         ngram[i] = ngram[i+1];
        //
        //     // insert new token
        //     ngram[N-1] = *token;
        //
        //     // add n-gram
        //     ngrams.push_back(ngram);
        //     documents.push_back(docstring);
        // }
        //
        // // get final n-grams
        // for (uint i = N - 1; i > 0; i--) {
        //     for (uint j = 0; j < i - 1; j++) {
        //         ngram[j] = ngram[j+1];
        //     }
        //     // Shift in empty token for end of document
        //     ngram[i] = "";
        //
        //     // add n-gram
        //     ngrams.push_back(ngram);
        //     documents.push_back(docstring);
        // }

        // free the tokens and return the counts
        mitie_free(tokens);
    }

    Dataset<N> getDataset() {
        Dataset<N> dataset;

        // get new ngram vector
        index(ngrams, dataset.ngramIndices, dataset.vocabulary);
        // erase original vector
        std::vector<NGRAM>().swap(ngrams);

        // get new document vector
        index(documents, dataset.documentIndices, dataset.documents);
        // erace original vector
        std::vector<std::string>().swap(documents);
        return dataset;
    }

private:

    template<typename T>
    static void index(const std::vector<T>& in, std::vector<size_t>& indices, std::vector<T>& values) {
        std::set<T> valueSet(in.begin(), in.end());
        values.assign(valueSet.begin(), valueSet.end());
        std::map<T, size_t> valueMap;
        for (size_t i = 0; i < values.size(); i++)
            valueMap[values[i]] = i;

        for (const auto& value : in)
            indices.push_back(valueMap[value]);
    }

    std::vector<NGRAM> ngrams;
    std::vector<std::string> documents;
};

#endif /* _PREPROCESS_HPP_ */

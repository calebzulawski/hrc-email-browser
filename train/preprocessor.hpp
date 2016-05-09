#ifndef _PREPROCESS_HPP_
#define _PREPROCESS_HPP_

#include <mitie.h>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <array>
#include <regex>

class Dataset {
public:
    std::vector<size_t> tokenIndices;
    std::vector<size_t> documentIndices;
    std::vector<std::string> vocabulary;
    std::vector<std::string> documents;
};

class Preprocessor {
public:
    void addDocument(const char* text, const char* document)
    {
        // only select tokens that contain letters or numbers
        std::regex filter_regex(".*[a-zA-Z0-9].*");

        char** inputTokens = mitie_tokenize(text);

        std::string docstring = document;

        for (char** token = inputTokens; *token != nullptr; token++) {
            std::string tokenstring = *token;

            if (!std::regex_match(tokenstring, filter_regex))
                continue;

            tokens.push_back(tokenstring);
            documents.push_back(docstring);
        }

        // free the tokens and return the counts
        mitie_free(inputTokens);
    }

    Dataset getDataset() {
        Dataset dataset;

        // get new token vector
        index(tokens, dataset.tokenIndices, dataset.vocabulary);
        // erase original vector
        std::vector<std::string>().swap(tokens);

        // get new document vector
        index(documents, dataset.documentIndices, dataset.documents);
        // erace original vector
        std::vector<std::string>().swap(documents);
        return dataset;
    }

private:
    static void index(const std::vector<std::string>& in,
                      std::vector<size_t>& indices,
                      std::vector<std::string>& values) {
        std::set<std::string> valueSet(in.begin(), in.end());
        values.assign(valueSet.begin(), valueSet.end());
        std::map<std::string, size_t> valueMap;
        for (size_t i = 0; i < values.size(); i++)
            valueMap[values[i]] = i;

        for (const auto& value : in)
            indices.push_back(valueMap[value]);
    }

    std::vector<std::string> tokens;
    std::vector<std::string> documents;
};

#endif /* _PREPROCESS_HPP_ */

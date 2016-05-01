#ifndef _PREPROCESS_HPP_
#define _PREPROCESS_HPP_

#include <map>
#include <mitie.h>

template<size_t N>
class Preprocessor {
public:
    typedef std::array<std::string, N> NGRAM;
    std::map<NGRAM, int> bagOfWords(const char* text)
    {
        std::map<NGRAM, int> counts;
        char** tokens = mitie_tokenize(text);

        // create n-grams
        NGRAM ngram;
        for (char** token = tokens; *token != nullptr; token++) {
            // shift n-gram
            for (uint i = 0; i < N - 1; i++)
                ngram[i] = ngram[i+1];

            // insert new token
            ngram[N-1] = *token;

            // add to map (works because int initializes to 0)
            counts[ngram]++;
        }

        // get final n-grams
        for (uint i = N - 1; i > 0; i--) {
            for (uint j = 0; j < i - 1; j++) {
                ngram[j] = ngram[j+1];
            }
            // Shift in empty token for end of document
            ngram[i] = "";
            counts[ngram]++;
        }

        // free the tokens and return the counts
        mitie_free(tokens);
        return counts;
    }
};

#endif /* _PREPROCESS_HPP_ */

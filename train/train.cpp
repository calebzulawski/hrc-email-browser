#include <sqlite3.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include "preprocessor.hpp"
#include "lda.hpp"

int callback(void* passptr, int ncol, char** contents, char** names) {
    auto preprocessor = static_cast<Preprocessor*>(passptr);
    std::cout << contents[0] << std::endl; // docID
    preprocessor->addDocument(contents[1], contents[0]); // docText
    return 0;
}

int main() {
    Preprocessor preprocessor;

    // sqlite database
    sqlite3* db;
    int rc; // return code

    // Open the database
    rc = sqlite3_open_v2("../hrcemail.sqlite", &db, SQLITE_OPEN_READONLY, nullptr);
    if (rc != SQLITE_OK) {
        std::cout << "Error opening sqlite3 database: " << sqlite3_errmsg(db) << std::endl;
        abort();
    }

    // Run the callback on each row
    char* error = nullptr;
    rc = sqlite3_exec(db,
                      "select docID, docText from document",
                      &callback,
                      (void*)&preprocessor,
                      &error);

    if (error != nullptr) {
        std::cout << "sqlite3 error: " << error << std::endl;
        sqlite3_free(error);
        abort();
    }

    Dataset dataset = preprocessor.getDataset();

    auto lda = LDA(10, 1e-3, 1e-2); // 10 topics, alpha = 1e-3, eta = 1e-2 
    lda.process(dataset.tokenIndices,
                dataset.documentIndices,
                dataset.vocabulary.size(),
                dataset.documents.size(),
                100); // epochs

    lda.writeTokenJSON(dataset.vocabulary, "tokens.json");
    lda.writeDocumentJSON(dataset.documents, "documents.json");

    // calculate perplexity
    std::vector<double> perps = {};
    for (uint i = 2; i < 10; i++){
        auto lda2 = LDA(i);
        double perp = lda2.perplexity(dataset.tokenIndices,
                                     dataset.documentIndices,
                                     dataset.vocabulary.size(),
                                     dataset.documents.size(),
                                     1000);
        perps.push_back(perp);
    }
    for(uint i = 0; i < perps.size(); i++){
        std::cout << "topics: " << i << " perplexity: " << perps[i] << std::endl;
    }
}

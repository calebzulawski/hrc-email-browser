#include <sqlite3.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include "preprocessor.hpp"
#include "lda.hpp"

int callback(void* passptr, int ncol, char** contents, char** names) {
    auto preprocessor = static_cast<Preprocessor<2>*>(passptr);
    std::cout << contents[0] << std::endl; // docID
    preprocessor->addDocument(contents[1], contents[0]); // docText
    return 0;
}

int main() {
    // Create bigrams
    static constexpr size_t N = 1;

    Preprocessor<N> preprocessor;

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

    Dataset<N> dataset = preprocessor.getDataset();

    auto lda = LDA(10); // 10 topics
    lda.process(dataset.ngramIndices,
                dataset.documentIndices,
                dataset.vocabulary.size(),
                dataset.documents.size());
}

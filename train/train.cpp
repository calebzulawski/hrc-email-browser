#include <sqlite3.h>
#include <iostream>
#include <cstdlib>

int test_callback(void*, int ncol, char** contents, char** names) {
    for (int i = 0; i < ncol; i++) {
        std::cout << names[i] << ": " << std::endl << (contents[i] ? contents[i] : "") << std::endl << std::endl;
    }
    std::cout << "====================================" << std::endl;
    return 0;
}

int main() {
    int rc; // return codes
    sqlite3* db;
    rc = sqlite3_open_v2("../hrcemail.sqlite", &db, SQLITE_OPEN_READONLY, nullptr);
    if (rc != SQLITE_OK) {
        std::cout << "Error opening sqlite3 database: " << sqlite3_errmsg(db) << std::endl;
        abort();
    }

    char* error = nullptr;
    rc = sqlite3_exec(db,
                      "select * from document limit 2",
                      &test_callback,
                      nullptr, // first argument of the callback--we can ignore this
                      &error);

    if (error != nullptr) {
        std::cout << "sqlite3 error: " << error << std::endl;
        sqlite3_free(error);
        abort();
    }
}

#include <sqlite3.h>
#include <iostream>
#include <cstdlib>

int main() {
    sqlite3* db;
    int rc = sqlite3_open_v2("../hrcemail.sqlite", &db, SQLITE_OPEN_READONLY, nullptr);
    if (rc != SQLITE_OK) {
        std::cout << "Error opening sqlite3 database: " << sqlite3_errmsg(db) << std::endl;
        abort();
    }
}

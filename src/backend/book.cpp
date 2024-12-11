#include <cstdint>
#include <mutex>
#include <memory>
#include <vector>
#include <set>
#include "engine.hpp"
#include "book.hpp"

Book::Book()
    : buys(std::set<SamePriceQueue>()), sells(std::set<SamePriceQueue>()) {}

void Book::send_order(ClientCommand command) {}

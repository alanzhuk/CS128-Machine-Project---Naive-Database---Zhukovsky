#include <iostream>

#include "db.hpp"
#include "db_table.hpp"

int main() {
  // write informal tests here
  auto db = Database();
  db.CreateTable("alan");
  std::cout << db.GetTable("alan");
}
#include "db.hpp"
//std::map<std::string, DbTable*> tables_; 
void Database::CreateTable(const std::string& table_name){
    auto* hold = new DbTable();
    tables_.insert({table_name, hold});
}
void Database::DropTable(const std::string& table_name){
    if (!tables_.contains(table_name)) {
        throw std::invalid_argument(""); 
    }
    delete tables_.at(table_name);
    tables_.erase(table_name);
}
DbTable& Database::GetTable(const std::string& table_name){
    if (!tables_.contains(table_name)) {
        throw std::invalid_argument(""); 
    }
    DbTable& hold = *tables_.at(table_name);
    return hold;
}
Database::Database(const Database& rhs){
    for (auto const& [key,value] : rhs.tables_) {
        auto* hold = new DbTable(*value);
        tables_.insert({key, hold});
    }
}
Database& Database::operator=(const Database& rhs){
    if (&rhs == this) {
        return *this;
    }
    std::vector<std::string> keys;
    for (auto const& [key,value] : tables_) {
        keys.push_back(key);
        delete value;
    }
    for (unsigned int i = 0; i < keys.size(); i++) {
        tables_.erase(keys[i]);
    }
    for (auto const& [key,value] : rhs.tables_) {
        auto* hold = new DbTable(*value);
        tables_.insert({key, hold});
    }
    return *this;
}
Database::~Database(){
    for (auto const& [key,value] : tables_) {
        delete value;
    }
}
std::ostream& operator<<(std::ostream& os, const Database& db){
    for (auto const& [key,value] : db.tables_) {
        os << *value << '\n'; 
    }
    return os; 
}

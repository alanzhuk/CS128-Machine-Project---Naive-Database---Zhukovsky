#include "db_table.hpp"

// const unsigned int kRowGrowthRate = 2;
/*
unsigned int next_unique_id_ = 0;
  unsigned int row_col_capacity_ = 2;
  std::map<unsigned int, void**> rows_;
  std::vector<std::pair<std::string, DataType>> col_descs_;
*/
void DbTable::UpSize() {
  row_col_capacity_ *= 2;
  for (auto const& [key,value] : rows_) {
    void** hold = new void*[row_col_capacity_];
    for (unsigned int i = 0; i < row_col_capacity_ / 2; i++) {
      hold[i] = value[i]; 
    }
    delete[] value;
    rows_[key] = hold;
  }
}
void DbTable::AddColumn(const std::pair<std::string, DataType>& col_desc){
  if(col_descs_.size() == row_col_capacity_) {
    UpSize();
  }
  col_descs_.push_back(col_desc);
  if (col_desc.second == DataType::kDouble) {
    for (auto const& [key,value] : rows_) {
      value[col_descs_.size()-1] = static_cast<void*>(new double(0.0));
    }
  } else if (col_desc.second == DataType::kString) {
    for (auto const& [key,value] : rows_) {
      value[col_descs_.size()-1] = static_cast<void*>(new std::string(""));
    }
  } else {
    for (auto const& [key,value] : rows_) {
      value[col_descs_.size()-1] = static_cast<void*>(new int(0));
    }
  }
}
void DbTable::DeleteColumnByIdx(unsigned int col_idx){
  if (col_idx >= col_descs_.size() || col_idx < 0) {
    throw std::out_of_range("col_idx is outside the range of col_descs_.");
  }
  if (col_descs_.size() == 1 && !rows_.empty()) {
    throw std::runtime_error("exception: any table with at least one row must have at least one column.");
  }
  for (auto const& [key,value] : rows_) {
    if (col_descs_[col_idx].second == DataType::kDouble) {
      delete static_cast<double*>(value[col_idx]);
    } else if (col_descs_[col_idx].second == DataType::kInt) {
      delete static_cast<int*>(value[col_idx]);
    } else {
      delete static_cast<std::string*>(value[col_idx]);
    }
    for (unsigned int i = col_idx; i < col_descs_.size() - 1; i++) {
      value[i] = value[i + 1];
    }
  }
  col_descs_.erase(col_descs_.begin() + col_idx);
}

void DbTable::AddRow(const std::initializer_list<std::string>& col_data){
  if (col_data.size() != col_descs_.size()) {
    throw std::invalid_argument("");
  }
  void** hold = new void*[row_col_capacity_];
  rows_.insert({next_unique_id_, hold});
  int i = 0;
  for(const std::string& str : col_data) {
    if(col_descs_[i].second == DataType::kDouble) {
      rows_[next_unique_id_][i] = static_cast<void*>(new double(std::stod(str)));
    } else if(col_descs_[i].second == DataType::kInt) {
      rows_[next_unique_id_][i] = static_cast<void*>(new int(std::stoi(str)));
    } else {
      rows_[next_unique_id_][i] = static_cast<void*>(new std::string(str));
    }
    i++;
  }
  next_unique_id_++;
}
void DbTable::DeleteRowById(unsigned int id){
  if (!rows_.contains(id)) {
    throw std::invalid_argument("");
  }
  for (unsigned int i = 0; i < col_descs_.size(); i++) {
    if (col_descs_[i].second == DataType::kDouble) {
      delete static_cast<double*>(rows_[id][i]);
    } else if (col_descs_[i].second == DataType::kInt) {
      delete static_cast<int*>(rows_[id][i]);
    } else {
      delete static_cast<std::string*>(rows_[id][i]);
    }
  }/*
  for (unsigned int i = col_descs_.size(); i < row_col_capacity_; i++) { //this for loop might cause errors and may be useless
    delete rows_[id][i];
  } */
  delete[] rows_[id];
  rows_.erase(id);
}
/*
unsigned int next_unique_id_ = 0;
  unsigned int row_col_capacity_ = 2;
  std::map<unsigned int, void**> rows_;
  std::vector<std::pair<std::string, DataType>> col_descs_;
*/
DbTable::DbTable(const DbTable& rhs): next_unique_id_(rhs.next_unique_id_), row_col_capacity_(rhs.row_col_capacity_){
  col_descs_ = rhs.col_descs_;
  for (auto const& [key,value] : rhs.rows_) {
    if (value != nullptr) {
      rows_.insert({key, nullptr});
      void** hold = new void*[row_col_capacity_];
      for (unsigned int i = 0; i < col_descs_.size(); i++) {
        if(col_descs_[i].second == DataType::kDouble) { 
          hold[i] = static_cast<void*>(new double(*(static_cast<double*>(value[i]))));
        } else if(col_descs_[i].second == DataType::kInt) {
          hold[i] = static_cast<void*>(new int(*(static_cast<int*>(value[i]))));
        } else {
          hold[i] = static_cast<void*>(new std::string(*(static_cast<std::string*>(value[i]))));
        }
      }
      rows_[key] = hold;
    }
  }
}
DbTable& DbTable::operator=(const DbTable& rhs){
  if (&rhs == this) {
    return *this;
  }
  for (auto const& [id,value] : rows_) {
    for (unsigned int i = 0; i < col_descs_.size(); i++) {
      if (col_descs_[i].second == DataType::kDouble) {
        delete static_cast<double*>(value[i]);
      } else if (col_descs_[i].second == DataType::kInt) {
        delete static_cast<int*>(value[i]);
      } else {
        delete static_cast<std::string*>(value[i]);
      }
    }
    delete[] rows_[id];
  }
  for (unsigned int i = 0; i < next_unique_id_; i++) {
    rows_.erase(i);
  }
  row_col_capacity_ = rhs.row_col_capacity_;
  col_descs_ = rhs.col_descs_;
  next_unique_id_ = rhs.next_unique_id_;
  for (auto const& [key,value] : rhs.rows_) {
    if (value != nullptr) {
      rows_.insert({key, nullptr});
      void** hold = new void*[row_col_capacity_];
      for (unsigned int i = 0; i < col_descs_.size(); i++) {
        if(col_descs_[i].second == DataType::kDouble) { 
          hold[i] = static_cast<void*>(new double(*(static_cast<double*>(value[i]))));
        } else if(col_descs_[i].second == DataType::kInt) {
          hold[i] = static_cast<void*>(new int(*(static_cast<int*>(value[i]))));
        } else {
          hold[i] = static_cast<void*>(new std::string(*(static_cast<std::string*>(value[i]))));
        }
      }
      rows_[key] = hold;
    }
  }
  return *this;
}
DbTable::~DbTable(){
  for (auto const& [id,value] : rows_) {
    for (unsigned int i = 0; i < col_descs_.size(); i++) {
      if (col_descs_[i].second == DataType::kDouble) {
        delete static_cast<double*>(value[i]);
      } else if (col_descs_[i].second == DataType::kInt) {
        delete static_cast<int*>(value[i]);
      } else {
        delete static_cast<std::string*>(value[i]);
      }
    }
    delete[] value;
    rows_.at(id) = nullptr;
  }
  for (unsigned int i = 0; i < next_unique_id_; i++) {
    rows_.erase(i);
  }
}
/*
unsigned int next_unique_id_ = 0;
  unsigned int row_col_capacity_ = 2;
  std::map<unsigned int, void**> rows_;
  std::vector<std::pair<std::string, DataType>> col_descs_;
*/
std::ostream& operator<<(std::ostream& os, const DbTable& table){
  for (unsigned int i = 0; i < table.col_descs_.size(); i++) {
    os << table.col_descs_[i].first << "(";
    if (table.col_descs_[i].second == DataType::kDouble) {
      os << "double)";
    } else if (table.col_descs_[i].second == DataType::kInt) {
      os << "int)";
    } else {
      os << "std::string)";
    }
    if (i != table.col_descs_.size() - 1) {
      os << ", ";
    }
  }
  os << '\n';
  for (unsigned int w = 0; w < table.next_unique_id_; w++) {
    for (unsigned int i = 0; i < table.col_descs_.size(); i++) {
      if (table.col_descs_[i].second == DataType::kDouble) {
        os << *(static_cast<double*>(table.rows_.at(w)[i]));
      } else if (table.col_descs_[i].second == DataType::kInt) {
        os << *(static_cast<int*>(table.rows_.at(w)[i]));
      } else {
        os << *(static_cast<std::string*>(table.rows_.at(w)[i]));
      }
      if (i != table.col_descs_.size() - 1) {
        os << ", ";
      }
    }
    os << '\n';
  }
  return os;
}

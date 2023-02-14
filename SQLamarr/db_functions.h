#pragma once
#include <memory>
#include <string>
#include <string_view>
#include "sqlite3.h"

namespace SQLamarr
{
  /// Unique pointer to the sqlite3 connection
  typedef std::unique_ptr<sqlite3, void(*)(sqlite3*)> SQLite3DB;

  /// Initialize the database
  SQLite3DB make_database (
      std::string filename,
      int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI,
      std::string init = std::string()
      );

  /// Prpare a statement
  sqlite3_stmt* prepare_statement (SQLite3DB& db, const std::string_view query);

  /// Dump the result of a query to a string
  std::string dump_table(SQLite3DB& db, std::string_view query);

  /// Read a column field from a sqlite3 statement and convert it to float
  float read_as_float(sqlite3_stmt*, int);

  /// Ensure a token is alphanumeric
  void validate_token(const std::string& token);
}



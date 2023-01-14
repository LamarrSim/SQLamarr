#pragma once
#include <memory>
#include <string_view>
#include "sqlite3.h"

namespace Lamarr
{
  typedef std::unique_ptr<sqlite3, void(*)(sqlite3*)> SQLite3DB;
  SQLite3DB make_database (std::string filename);

  sqlite3_stmt* prepare_statement (SQLite3DB& db, const std::string_view query);

  std::string dump_table(SQLite3DB& db, std::string_view query);
}



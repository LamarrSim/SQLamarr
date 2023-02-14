// STL
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

// SQLite3
#include "sqlite3.h"

// SQLamarr
#include "SQLamarr/CleanEventStore.h"

namespace SQLamarr
{
  //============================================================================
  // execute
  //============================================================================
  void CleanEventStore::execute()
  {
    sqlite3_stmt* list_tables = get_statement("list_tables",
        "SELECT name FROM main.sqlite_schema WHERE type='table'");
    sqlite3_stmt* list_temp_tables = get_statement("list_temp_tables",
        "SELECT name FROM temp.sqlite_schema WHERE type='table'");

    std::vector<std::string> tables;
    while (exec_stmt(list_tables))
      tables.push_back(
          reinterpret_cast<const char *>(sqlite3_column_text(list_tables, 0))
          );

    while (exec_stmt(list_temp_tables))
      tables.push_back(
          reinterpret_cast<const char *>(sqlite3_column_text(list_temp_tables, 0))
          );

    char buffer[1024];
    for (auto& table: tables)
    {
      sprintf(buffer, "DELETE FROM %s", table.c_str());
      exec_stmt(get_statement(buffer, buffer));
    }
  }
}

#include "Lamarr/db_functions.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include "schema.sql"

namespace Lamarr
{
  //==========================================================================
  // _string_field (internal helper function)
  //==========================================================================
  template <typename T>
    std::string _string_field (T* s, int length, int column_type)
    {
      std::stringstream ret;

      int iCh;
      for (iCh = 0; s[iCh] != '\0'; ++iCh);
      const int s_len = iCh;

      if (s_len < length)
      {
        ret << s;
        for (iCh = s_len; iCh < length; iCh++)
          ret << ' ';
      }
      else if (column_type == SQLITE_TEXT)
      {
        for (iCh = 0; iCh < length/2 - 1; iCh++)
          ret << s[iCh];
        ret << "..";
        for (iCh = s_len - length/2 + 1; iCh < s_len; iCh++)
          ret << s[iCh];
      }
      else if (column_type == SQLITE_INTEGER || column_type == SQLITE_FLOAT)
      {
        char buf[1024];
        sprintf(buf, "%g%100s", atof(reinterpret_cast<const char *>(s)), "");
        buf[length+1] = '\0';
        ret << buf;
      }

      return ret.str();
    }


  //==========================================================================
  // _get_field_length (internal helper function)
  //==========================================================================
  int _get_field_length (int column_type)
  {
    switch (column_type)
    {
      case SQLITE_INTEGER:
      case SQLITE_FLOAT:
      case SQLITE_NULL:
        return 12;
      case SQLITE_TEXT:
        return 30;

    }
    return 8;
  }

  //==========================================================================
  // make_database
  //==========================================================================
  SQLite3DB make_database (std::string filename)
  {
    sqlite3* db;
    char *zErrMsg;
    int retcode;

    retcode = sqlite3_open(filename.c_str(), &db);
    if (retcode) 
      throw std::logic_error("Failed to instantiate SQLite3 DB");

    retcode = sqlite3_exec(db, SQL_CREATE_SCHEMA, nullptr, nullptr, &zErrMsg);
    if (retcode)
    {
      std::cerr << sqlite3_errmsg(db) << std::endl;
      throw (std::logic_error("SQL Error in make_database"));
    }

    return SQLite3DB(
        db,
        [](sqlite3* ptr) {
        sqlite3_close(ptr);
        }
        );
  }

  //==========================================================================
  // prepare_statement
  //==========================================================================
  sqlite3_stmt* prepare_statement (SQLite3DB& db, const std::string_view query)
  {
    sqlite3_stmt* stmt;

    int retcode = sqlite3_prepare_v2(
        db.get(), query.data(), -1, &stmt, nullptr
        );

    if (retcode != SQLITE_OK)
    {
      std::cerr << sqlite3_errmsg(db.get()) << std::endl;
      throw std::logic_error("Failed to compile query");
    }

    return stmt;
  }

  //==========================================================================
  // dump_table
  //==========================================================================
  std::string dump_table(SQLite3DB& db, std::string_view query)
  {
    std::stringstream ret;
    constexpr char SEPARATOR[] = "  ";

    sqlite3_stmt* stmt = prepare_statement(db, query.data());

    bool first_row = true;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
      const int nColumns = sqlite3_column_count(stmt);
      int iCol;

      if (first_row)
      {
        for (iCol = 0; iCol < nColumns; ++iCol)
        {
          auto s = sqlite3_column_name (stmt, iCol);
          auto t = sqlite3_column_type (stmt, iCol);
          auto n = _get_field_length(t);
          auto f = _string_field(s, n, SQLITE_TEXT);
          ret << f << SEPARATOR;
        }
        ret << std::endl;
        first_row = false;
      }


      for (iCol = 0; iCol < nColumns; ++iCol)
      {
        auto t = sqlite3_column_type (stmt, iCol);
        auto n = _get_field_length(t);
        if (t == SQLITE_NULL)
        {
          ret << "NaN";
          for (int iCh = 0; iCh < n-3; ++iCh) ret << " ";
          ret << SEPARATOR;
          continue;
        }
        
        auto s = sqlite3_column_text (stmt, iCol);
        auto f = _string_field(s, n, t);
        ret << f << SEPARATOR;
      }
      ret << std::endl;
    }

    return ret.str();
  }

}

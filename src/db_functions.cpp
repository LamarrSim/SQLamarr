// (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration.
//
// This software is distributed under the terms of the GNU General Public
// Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
//
// In applying this licence, CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.


#include <iostream>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <cmath>

#include "SQLamarr/db_functions.h"
#include "SQLamarr/GlobalPRNG.h"
#include "SQLamarr/SQLiteError.h"
#include "SQLamarr/custom_sql_functions.h"
#include "schema.sql"

namespace SQLamarr
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
  SQLite3DB make_database (std::string filename, int flags, std::string init)
  {
    sqlite3* db;
    char *zErrMsg;
    int retcode;

    if (init == "")
      init = SQL_CREATE_SCHEMA;

    retcode = sqlite3_open_v2(filename.c_str(), &db, flags, nullptr);
    if (retcode) 
    {
      std::cerr << "Failure while initializing " << filename << ": "
        << sqlite3_errmsg(db)
        << std::endl;
     
      throw SQLiteError("Failed to instantiate SQLite3 DB");
    }

    retcode = sqlite3_exec(db, init.c_str(), nullptr, nullptr, &zErrMsg);
    if (retcode)
    {
      std::cerr << sqlite3_errmsg(db) << std::endl;
      throw (SQLiteError("SQL Error in make_database"));
    }

    return SQLite3DB(
        db,
        [](sqlite3* ptr) {
        SQLamarr::GlobalPRNG::release(ptr);
        int ret = sqlite3_close(ptr);
        if (ret != SQLITE_OK)
        {
          std::cerr << "sqlite3_close returned errorcode: " << ret << std::endl;
          throw (SQLiteError("Failed closing the connection"));
        }
      }
      );
  }

  //==========================================================================
  // prepare_statement
  //==========================================================================
  sqlite3_stmt* prepare_statement (SQLite3DB& db, const std::string& query)
  {
    sqlite3_stmt* stmt;

    int retcode = sqlite3_prepare_v2(
        db.get(), query.c_str(), -1, &stmt, nullptr
        );

    if (retcode != SQLITE_OK)
    {
      std::cerr << sqlite3_errmsg(db.get()) << std::endl;
      throw SQLiteError("Failed to compile query");
    }

    return stmt;
  }

  //==========================================================================
  // dump_table
  //==========================================================================
  std::string dump_table(SQLite3DB& db, const std::string& query)
  {
    std::stringstream ret;
    constexpr char SEPARATOR[] = "  ";

    sqlite3_stmt* stmt = prepare_statement(db, query.c_str());
    int nRows = 0;
    int nColumns = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
      nColumns = sqlite3_column_count(stmt);
      int iCol;
      nRows ++;

      if (nRows == 1)
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
      }

      if (nRows >= 10)
        continue;



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

    ret << "\n[" << nRows << " rows x " << nColumns << " columns]" << std::endl;

    sqlite3_finalize(stmt);
    return ret.str();
  }

  //==========================================================================
  // read_as_float
  //==========================================================================
  float read_as_float(sqlite3_stmt* stmt, int iCol)
  {
    float buf;
    switch (sqlite3_column_type(stmt, iCol))
    {
      case SQLITE_INTEGER:
        buf = static_cast<float>(sqlite3_column_int(stmt, iCol));
        break;
      case SQLITE_FLOAT:
        buf = static_cast<float>(sqlite3_column_double(stmt, iCol));
        break;
      default:
        buf = NAN;
    }

    return buf;
  }


  //==========================================================================
  // validate_token
  //==========================================================================
  void validate_token(const std::string& token)
  {
    const int forbidden = std::count_if(token.begin(), token.end(),
        [](unsigned char c){ return !std::isalnum(c) && c != '_'; }
        );

    if (forbidden)
    {
      std::cerr 
        << "Found non alphanumeric token in SQL query: " 
        << token
        << std::endl;
      throw std::runtime_error("Invalid token");
    }
  }

  
  //==========================================================================
  // update_db_connection
  //==========================================================================
  void update_db_connection(SQLite3DB& old_db, const std::string& db_uri, int flags)
  {
    // Create the new connection to the database
    SQLite3DB new_database = make_database(db_uri, flags);

    // Generate a seed for the new database, using the chain of random numbers
    // of the previous one.
    auto old_generator = GlobalPRNG::get_or_create(old_db.get());
    std::uniform_int_distribution<long> distribution(0, 0xFFFFFFFFL);
    uint64_t new_seed = distribution(*old_generator);
    GlobalPRNG::get_or_create(new_database.get(), new_seed);

    // Define the SQLamarr-custom SQLite functions for the new connection
    sqlamarr_create_sql_functions(new_database.get());

    // Replace the old db connection with the new one, and then destroy it
    old_db.swap(new_database);
  }

}

#include <stddef.h>
#include <math.h>
#include <random>
#include "sqlite3.h"
#include "SQLamarr/custom_sql_functions.h"


void sqlamarr_create_sql_functions (sqlite3 *db)
{
  int nPars;

  sqlite3_create_function(db,
      "norm2", 3,
      SQLITE_UTF8, NULL, &_sqlamarr_sql_norm2, NULL, NULL
      );

  sqlite3_create_function(db,
      "pseudorapidity", 3,
      SQLITE_UTF8, NULL, &_sqlamarr_sql_pseudorapidity, NULL, NULL
      );
  
  sqlite3_create_function(db,
      "azimuthal", 3,
      SQLITE_UTF8, NULL, &_sqlamarr_sql_azimuthal, NULL, NULL
      );

  sqlite3_create_function(db,
      "polar", 3,
      SQLITE_UTF8, NULL, &_sqlamarr_sql_polar, NULL, NULL
      );

  sqlite3_create_function(db,
      "propagation_charge", 1,
      SQLITE_UTF8, NULL, &_sqlamarr_sql_propagation_charge, NULL, NULL
      );

  sqlite3_create_function(db,
      "random_uniform", 0,
      SQLITE_UTF8, NULL, &_sqlamarr_sql_random_uniform, NULL, NULL
      );
}

void _sqlamarr_sql_norm2 (
    sqlite3_context *context,
    int argc,
    sqlite3_value **argv
    )
{
  double res = 0;
  int iPar = 0;
  double buf;

  for (iPar = 0; iPar < argc; ++iPar)
  {
    buf = sqlite3_value_double(argv[iPar]);
    res += buf*buf;
  }
  res = sqrt(buf);

  sqlite3_result_double(context, res);
}

void _sqlamarr_sql_pseudorapidity (
    sqlite3_context *context,
    int argc,
    sqlite3_value **argv
    )
{
  double x, y, z, theta, eta;

  if (argc != 3)
  {
    sqlite3_result_null(context);
    return;
  }

  x = sqlite3_value_double(argv[0]);
  y = sqlite3_value_double(argv[1]);
  z = sqlite3_value_double(argv[2]);

  theta = atan2(x*x + y*y, z);
  eta = -log(tan(0.5*theta));

  sqlite3_result_double(context, eta);
}

void _sqlamarr_sql_azimuthal (
    sqlite3_context *context,
    int argc,
    sqlite3_value **argv
    )
{
  double x, y, z;

  if (argc != 3)
  {
    sqlite3_result_null(context);
    return;
  }

  x = sqlite3_value_double(argv[0]);
  y = sqlite3_value_double(argv[1]);
  z = sqlite3_value_double(argv[2]);

  sqlite3_result_double(context, atan2(y, x));
}

void _sqlamarr_sql_polar (
    sqlite3_context *context,
    int argc,
    sqlite3_value **argv
    )
{
  double x, y, z;

  if (argc != 3)
  {
    sqlite3_result_null(context);
    return;
  }

  x = sqlite3_value_double(argv[0]);
  y = sqlite3_value_double(argv[1]);
  z = sqlite3_value_double(argv[2]);

  sqlite3_result_double(context, atan2(x*x + y*y, z));
}

void _sqlamarr_sql_propagation_charge (
    sqlite3_context *context,
    int argc,
    sqlite3_value **argv
    )
{
  long int pid;

  if (argc != 1)
  {
    sqlite3_result_null(context);
    return;
  }

  pid = sqlite3_value_int(argv[0]);

  // Leptons 
  if (abs(pid) == 11 || abs(pid) == 13 || abs(pid) == 15)
    sqlite3_result_int(context, (pid > 0) ? -1 : +1);
  
  // Hadron Tracks
  else if (abs(pid) == 211 || abs(pid) == 321 || abs(pid) == 2212)
    sqlite3_result_int(context, (pid > 0) ? +1 : -1);

  // Photons and neutrons
  else if (abs(pid) == 22 || abs(pid) == 2112) 
    sqlite3_result_int(context, 0);
  
  else 
    sqlite3_result_null(context);
}


void _sqlamarr_sql_random_uniform (
    sqlite3_context *context,
    int argc,
    sqlite3_value **argv
    )
{
  static std::mt19937 generator;
  std::uniform_real_distribution<double> uniform;

  sqlite3_result_double(context, uniform(generator));
}

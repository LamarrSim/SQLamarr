// (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration.
//
// This software is distributed under the terms of the GNU General Public
// Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
//
// In applying this licence, CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.


#include <stddef.h>
#include <math.h>
#include <random>
#include "sqlite3.h"
#include "SQLamarr/custom_sql_functions.h"
#include "SQLamarr/GlobalPRNG.h"


//==============================================================================
// sqlamarr_create_sql_functions: define all the other functions in SQL
//==============================================================================
void sqlamarr_create_sql_functions (sqlite3 *db)
{
  int nPars;

  sqlite3_create_function(db,
      "log", 1,
      SQLITE_UTF8, NULL, &_sqlamarr_sql_log, NULL, NULL
      );

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
      "slopes_to_cartesian", 4,
      SQLITE_UTF8, NULL, &_sqlamarr_sql_slopes_to_cartesian, NULL, NULL
      );

  sqlite3_create_function(db,
      "random_uniform", 0,
      SQLITE_UTF8, NULL, &_sqlamarr_sql_random_uniform, NULL, NULL
      );

  sqlite3_create_function(db,
      "random_normal", 0,
      SQLITE_UTF8, NULL, &_sqlamarr_sql_random_normal, NULL, NULL
      );

  for (nPars = 1; nPars < 10; ++nPars)
    sqlite3_create_function(db,
        "random_category", nPars,
        SQLITE_UTF8, NULL, &_sqlamarr_sql_random_category, NULL, NULL
        );

  sqlite3_create_function(db,
      "z_closest_to_beam", 5,
      SQLITE_UTF8, NULL, &_sqlamarr_sql_z_closest_to_beam, NULL, NULL
      );

}

//==============================================================================
// log
//==============================================================================
void _sqlamarr_sql_log (
    sqlite3_context *context,
    int /*argc*/,
    sqlite3_value **argv
    )
{
  double res = 0;
  res = log(sqlite3_value_double(argv[0]));
  sqlite3_result_double(context, res);
}

//==============================================================================
// norm2
//==============================================================================
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
  res = sqrt(res);

  sqlite3_result_double(context, res);
}

//==============================================================================
// z_closest_to_beam
//==============================================================================
void _sqlamarr_sql_z_closest_to_beam (
    sqlite3_context *context,
    int argc,
    sqlite3_value **argv
    )
{
  if (argc != 5)
  {
    sqlite3_result_null(context);
    return;
  }

  int i = 0;
  const double x0 = sqlite3_value_double(argv[i++]);
  const double y0 = sqlite3_value_double(argv[i++]);
  const double z0 = sqlite3_value_double(argv[i++]);
  const double tx = sqlite3_value_double(argv[i++]);
  const double ty = sqlite3_value_double(argv[i++]);

  const double ctb_z = (tx*tx*z0 - tx*x0 + ty*ty*z0 - ty*y0)/(tx*tx + ty*ty);
    
  sqlite3_result_double(context, ctb_z);
}


//==============================================================================
// pseudorapidity
//==============================================================================
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

  theta = atan2(sqrt(x*x + y*y), z);
  eta = -log(tan(0.5*theta));

  sqlite3_result_double(context, eta);
}

//==============================================================================
// azimuthal
//==============================================================================
void _sqlamarr_sql_azimuthal (
    sqlite3_context *context,
    int argc,
    sqlite3_value **argv
    )
{
  double x, y/*, z*/;

  if (argc != 3)
  {
    sqlite3_result_null(context);
    return;
  }

  x = sqlite3_value_double(argv[0]);
  y = sqlite3_value_double(argv[1]);
  /* z is unused but it is expected for interface consistency 
  z = sqlite3_value_double(argv[2]);
  */

  sqlite3_result_double(context, atan2(y, x));
}

//==============================================================================
// polar
//==============================================================================
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

//==============================================================================
// propagation_charge
//==============================================================================
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

//==============================================================================
// slopes_to_cartesian
//==============================================================================
void _sqlamarr_sql_slopes_to_cartesian (
    sqlite3_context *context,
    int argc,
    sqlite3_value **argv
    )
{
  int coord;
  double norm, tx, ty;
  double ret[3];

  if (argc != 4)
  {
    sqlite3_result_null(context);
    return;
  }

  coord = sqlite3_value_int(argv[0]);
  if (coord < 0 or coord > 2)
  {
    sqlite3_result_error(context, "Invalid coord. Choose 012 for xyz.", -1);
    return;
  }

  norm = sqlite3_value_double(argv[1]);
  if (norm < 0)
  {
    sqlite3_result_error(context, "Negative norm", -1);
    return;
  }

  tx = sqlite3_value_double(argv[2]);
  ty = sqlite3_value_double(argv[3]);

  ret[2] = norm/sqrt(1 + tx*tx + ty*ty);
  ret[0] = ret[2]*tx;
  ret[1] = ret[2]*ty;

  sqlite3_result_double(context, ret[coord]);
}


//==============================================================================
// random_uniform
//==============================================================================
void _sqlamarr_sql_random_uniform (
    sqlite3_context *context,
    int /*argc*/,
    sqlite3_value ** /*argv*/
    )
{
  auto generator = SQLamarr::GlobalPRNG::get_or_create(context);
  std::uniform_real_distribution<double> uniform;

  sqlite3_result_double(context, uniform(*generator));
}

//==============================================================================
// random_normal
//==============================================================================
void _sqlamarr_sql_random_normal (
    sqlite3_context *context,
    int /*argc*/,
    sqlite3_value ** /*argv*/
    )
{
  auto generator = SQLamarr::GlobalPRNG::get_or_create(context);
  std::normal_distribution<double> gaussian;

  sqlite3_result_double(context, gaussian(*generator));
}

//==============================================================================
// random_category
//==============================================================================
void _sqlamarr_sql_random_category (
    sqlite3_context *context,
    int argc,
    sqlite3_value **argv
    )
{
  auto generator = SQLamarr::GlobalPRNG::get_or_create(context);
  std::uniform_real_distribution<float> uniform;
  float sum = 0; 
  float r = uniform(*generator);
  int iArg;
  int ret = argc;

  // Check probability is non-negative
  for (iArg = 0; iArg < argc; ++iArg)
    if (sqlite3_value_double(argv[iArg]) < 0)
    {
      sqlite3_result_error(context, "Negative probability", -1);
      return;
    }

  // Compute cumulative and corresponding category
  for (iArg = 0; iArg < argc; ++iArg)
  {
    sum += static_cast<float>(sqlite3_value_double(argv[iArg]));
    if (r < sum && ret == argc) ret = iArg;
  }
  // Check sum of probabilities is normalized or normalizable
  if (sum > 1.)
    sqlite3_result_error(context, "Sum of probabilities larger than 1", -1);
  else
    sqlite3_result_int(context, ret);
}

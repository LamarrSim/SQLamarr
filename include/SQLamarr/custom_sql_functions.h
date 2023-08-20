// (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration.
//
// This software is distributed under the terms of the GNU General Public
// Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
//
// In applying this licence, CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.


#pragma once

#include "sqlite3.h"

void sqlamarr_create_sql_functions (sqlite3*);

void _sqlamarr_sql_log (sqlite3_context*, int, sqlite3_value**);
void _sqlamarr_sql_norm2 (sqlite3_context*, int, sqlite3_value**);
void _sqlamarr_sql_pseudorapidity (sqlite3_context*, int, sqlite3_value**);
void _sqlamarr_sql_polar (sqlite3_context*, int, sqlite3_value**);
void _sqlamarr_sql_azimuthal (sqlite3_context*, int, sqlite3_value**);
void _sqlamarr_sql_propagation_charge (sqlite3_context*, int, sqlite3_value**);
void _sqlamarr_sql_random_uniform (sqlite3_context*, int, sqlite3_value**);
void _sqlamarr_sql_random_normal (sqlite3_context*, int, sqlite3_value**);
void _sqlamarr_sql_random_category (sqlite3_context*, int, sqlite3_value**);
void _sqlamarr_sql_z_closest_to_beam (sqlite3_context*, int, sqlite3_value**);
void _sqlamarr_sql_slopes_to_cartesian (sqlite3_context*, int, sqlite3_value**);


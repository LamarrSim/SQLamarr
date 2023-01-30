#pragma once

#include "sqlite3.h"

void sqlamarr_create_sql_functions (sqlite3*);

void _sqlamarr_sql_norm2 (sqlite3_context*, int, sqlite3_value**);
void _sqlamarr_sql_pseudorapidity (sqlite3_context*, int, sqlite3_value**);
void _sqlamarr_sql_polar (sqlite3_context*, int, sqlite3_value**);
void _sqlamarr_sql_azimuthal (sqlite3_context*, int, sqlite3_value**);
void _sqlamarr_sql_propagation_charge (sqlite3_context*, int, sqlite3_value**);
void _sqlamarr_sql_random_uniform (sqlite3_context*, int, sqlite3_value**);


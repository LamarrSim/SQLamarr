# (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration. 
#                                                                             
# This software is distributed under the terms of the GNU General Public
# Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
#                                                                             
# In applying this licence, CERN does not waive the privileges and immunities
# granted to it by virtue of its status as an Intergovernmental Organization  
# or submit itself to any jurisdiction.

import ctypes
from ctypes import POINTER 
from SQLamarr import clib, c_TransformerPtr
from typing import List, Union

from SQLamarr.db_functions import SQLite3DB

clib.new_TemporaryTable.argtypes = (
    ctypes.c_void_p,        # void *db,
    ctypes.c_char_p,        # const char* output_table,
    ctypes.c_char_p,        # const char* comma_separated_outputs,
    ctypes.c_char_p,        # const char* query,
    ctypes.c_bool,          # bool make_persistent
    )

clib.new_TemporaryTable.restype = c_TransformerPtr

class TemporaryTable:
  """Creates a temporary table from an SQL query. Persitency can be enabled.

  Python binding of `SQLamarr::TemporaryTable`.
  """

  def __init__ (
      self, 
      db: SQLite3DB,
      output_table: str,
      outputs: List[str],
      query: Union[str, List[str]],
      make_persistent: bool = False,
      ):
    """
    Acquires the reference to an open connection to the DB and configure the
    Transformer.

    @param db: An open database connection;
    @param output_table: name of the table where the query output is stored;
    @param outputs: list of the output column names for further reference;
    @param query: SQL query (or queries) defining the output columns;
    @param make_persistent: mark the TABLE as persistent.
    """
    if isinstance(query, str):
      query = [query]

    self._self = clib.new_TemporaryTable(
        db.get(),
        output_table.encode('ascii'),
        ";".join(outputs).encode('ascii'),
        ";".join([q.replace(";", " ") for q in query]).encode('ascii'),
        make_persistent,
        )
  
  def __del__(self):
    """@private: Release the bound class instance"""
    clib.del_Transformer(self._self)

  @property
  def raw_pointer(self):
    """@private: Return the raw pointer to the algorithm."""
    return self._self




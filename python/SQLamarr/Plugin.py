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
from SQLamarr import clib
from typing import List

from SQLamarr.db_functions import SQLite3DB

clib.new_Plugin.argtypes = (
    ctypes.c_void_p,        # void *db,
    ctypes.c_char_p,        # const char* library_path,
    ctypes.c_char_p,        # const char* function_name,
    ctypes.c_char_p,        # const char* query,
    ctypes.c_char_p,        # const char* output_table,
    ctypes.c_char_p,        # const char* comma_separated_outputs,
    ctypes.c_char_p,        # const char* comma_separated_references 
    )
    
clib.new_Plugin.restype = ctypes.c_void_p

clib.del_Plugin.argtypes = (ctypes.c_void_p,)

class Plugin:
  """
  Wrap an external function as defined in a compiled shared library.

  Python bindings for SQLamarr::Plugin.

  The C function `function_name` is selected from the shared object
  `library_path` and evaluated for each row obtained executing the
  `query` on the database `db`. Results are stored in the temporary 
  table `output_table` whose columns are named after the list of `outputs`.
  The columns returned by the `query` 
  are all interpreted as inputs to the external functions, unless 
  they are listed as `references`, in that case they are copied to the output
  table, easying JOIN operations with other tables in the database.

  """
  def __init__ (
      self, 
      db: SQLite3DB,
      library_path: str,
      function_name: str,
      query: str,
      output_table: str,
      outputs: List[str],
      references: List[str]
      ):
    """
    Acquire the db and configure the interface with the compiled function.


    @param db: An open database connection;
    @param library_path: path-like string defining the library defining the
      function to execute;
    @param function_name: string defining the name of the function to execute;
    @param query: SQL query defining the input and reference columns;
    @param output_table: name of the table where reference and output are
      stored;
    @param outputs: list of the output column names for further reference;
    @param references: list of columns selected by `query` to be used as
      reference indices instead of passing as inputs to the external function.
    

    """
    self._self = clib.new_Plugin(
        db.get(),
        library_path.encode('ascii'),
        function_name.encode('ascii'),
        query.encode('ascii'),
        output_table.encode('ascii'),
        ",".join(outputs).encode('ascii'),
        ",".join(references).encode('ascii'),
        )
  
  def __del__(self):
    """@private: Release the bound class instance"""
    clib.del_Plugin(self._self)

  @property
  def raw_pointer(self):
    """@private: Return the raw pointer to the algorithm."""
    return self._self



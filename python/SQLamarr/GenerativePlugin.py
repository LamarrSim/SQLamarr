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
from typing import List

from SQLamarr.db_functions import SQLite3DB

clib.new_GenerativePlugin.argtypes = (
    ctypes.c_void_p,        # void *db,
    ctypes.c_char_p,        # const char* library_path,
    ctypes.c_char_p,        # const char* function_name,
    ctypes.c_char_p,        # const char* query,
    ctypes.c_char_p,        # const char* output_table,
    ctypes.c_char_p,        # const char* comma_separated_outputs,
    ctypes.c_int,           # int n_random,
    ctypes.c_char_p,        # const char* comma_separated_references 
    )
    
clib.new_GenerativePlugin.restype = c_TransformerPtr

class GenerativePlugin:
  """
  Wrapper to a compiled parametrization taking conditions and noise as input.

  Python bindings for SQLamarr::GenerativePlugin.
  """
  def __init__ (
      self, 
      db: SQLite3DB,        
      library_path: str,
      function_name: str,
      query: str,
      output_table: str,
      outputs: List[str],
      nRandom: int,
      references: List[str]
      ):
    """
    Configure a `Transformer` to wrap a parametrization function defined 
    in an external library.

    @param db: An open database connection;
    @param library_path: path-like position of the shared library;
    @param function_name: linker symbol of the function to wrap;
    @param query: SQL query defining the reference indices and the inputs to
                  be passed to the wrapped function;
    @param output_table: name of the output TEMPORARY TABLE where outputs are
                  stored toghether with the reference indices;
    @param outputs: list of the output column names for further reference;
    @param nRandom: number of normally distributed random noise values;
    @param references: list of reference indices `SELECT`ed by the `query`,
                       but not part of the input to the wrapped function.
    """
    self._self = clib.new_GenerativePlugin(
        db.get(),
        library_path.encode('ascii'),
        function_name.encode('ascii'),
        query.encode('ascii'),
        output_table.encode('ascii'),
        ";".join(outputs).encode('ascii'),
        nRandom,
        ";".join(references).encode('ascii'),
        )
  
  def __del__(self):
    """@private: Release the bound class instance"""
    clib.del_Transformer(self._self)

  @property
  def raw_pointer(self):
    """@private: Return the raw pointer to the algorithm."""
    return self._self




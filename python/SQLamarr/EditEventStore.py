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

from SQLamarr.db_functions import SQLite3DB

from typing import List, Union

clib.new_EditEventStore.argtypes = (
    ctypes.c_void_p,        # void *db,
    ctypes.c_char_p,        # const char* semicolon_separated_queries,
    )

clib.new_EditEventStore.restype = c_TransformerPtr

class EditEventStore:
  """
  Execute a transaction with multiple custom queries without 
  returning any output.

  Refer to SQLamarr::EditEventStore for implementation details.
  """
  def __init__ (self, db: SQLite3DB, queries: Union[str,List[str]]):
    """
    Configure a Transformer to edit the schema of the EventStore

    @param db: An open database connection.
    @param queries: One or more queries to be processed in single transaction
    """
    if isinstance(queries, str):
      queries = [queries]

    self._self = clib.new_EditEventStore(
        db.get(),
        ";".join(queries).encode('ascii')
        )
  
  def __del__(self):
    """@private: Release the bound class instance"""
    clib.del_Transformer(self._self)

  @property
  def raw_pointer(self):
    """@private: Return the raw pointer to the algorithm."""
    return self._self





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

clib.new_UpdateDBConnection.argtypes = (ctypes.c_void_p,)
clib.new_UpdateDBConnection.restype = c_TransformerPtr

class UpdateDBConnection:
  """
  Update the reference to the DB Connection, to ensure synchronization with disk.

  Refer to SQLamarr::UpdateDBConnection for implementation details.
  """
  def __init__ (self, db: SQLite3DB):
    """
    Configure a Transformer to update the connection to the DB.

    @param db: An open database connection.
    """
    self._self = clib.new_UpdateDBConnection(db.get(), db.path.encode('ascii'))
  
  def __del__(self):
    """@private: Release the bound class instance"""
    clib.del_Transformer(self._self)

  @property
  def raw_pointer(self):
    """@private: Return the raw pointer to the algorithm."""
    return self._self





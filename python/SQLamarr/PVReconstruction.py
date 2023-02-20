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

from SQLamarr.db_functions import SQLite3DB

clib.new_PVReconstruction.argtypes = (
    ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p
    )
clib.new_PVReconstruction.restype = ctypes.c_void_p

clib.del_PVReconstruction.argtypes = (ctypes.c_void_p,)

class PVReconstruction:
  """
  Transform the MCVertices objects identified as primary vertices into Vertices,
  smearing their position according to a triple-Gaussian, diagonal
  resolution function.

  Python bindings to SQLamarr::PVReconstruction.

  The parameters used to parametrize the smearing where obtained from detailed
  simulation and are stored in an SQLite3 table with the schema documented in 
  SQLamarr::PVReconstruction.
  """
  def __init__ (self, 
      db: SQLite3DB, 
      file_name: str, 
      table_name: str, 
      condition: str
      ):
    """
    Acquires the reference to an open connection to the database and 
    connection details to a PV reconstruction database.

    @param db: An open database connection;
    @param file_name: string providing the path to a SQLite3 file;
    @param table_name: string providing the name of the TABLE where the
      parametrizations for the PV reconstruction are stored;
    @param condition: string identifier of the row to read the parametrization
      from.
    """
    self._self = clib.new_PVReconstruction(db.get(), 
        file_name.encode('ascii'),
        table_name.encode('ascii'),
        condition.encode('ascii'),
        )
  
  def __del__(self):
    """@private: Release the bound class instance"""
    clib.del_PVReconstruction(self._self)

  @property
  def raw_pointer(self):
    """@private: Return the raw pointer to the algorithm."""
    return self._self


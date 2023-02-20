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

clib.new_PVFinder.argtypes = (ctypes.c_void_p, ctypes.c_int)
clib.new_PVFinder.restype = c_TransformerPtr

class PVFinder:
  """Identify the primary vertex of each GenEvent (collision process)

  Python binding of `SQLamarr::PVFinder`.
  """
  def __init__ (self, db: SQLite3DB, signal_status_code: int = 889):
    """
    Acquires the reference to an open connection to the DB.

    @param db: SQLite3DB reference to an open connection
    @param signal_status_code: HepMC status code identifying a signal canidate
    """
    self._self = clib.new_PVFinder(db.get(), signal_status_code)
    print (f"PVFinder {self._self.p:x}")
  
  def __del__(self):
    """@private: Release the bound class instance"""
    print (f"Delete PVFinder {self._self.p:x} (type: {self._self.dtype})")
    clib.del_Transformer(self._self)

  @property
  def raw_pointer(self):
    """@private: Return the raw pointer to the algorithm."""
    return self._self



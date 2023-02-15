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

clib.new_PVFinder.argtypes = (ctypes.c_void_p, ctypes.c_int)
clib.new_PVFinder.restype = ctypes.c_void_p

clib.del_PVFinder.argtypes = (ctypes.c_void_p,)

class PVFinder:
  def __init__ (self, db, signal_status_code=889):
    self._self = clib.new_PVFinder(db.get(), signal_status_code)
  
  def __del__(self):
    clib.del_PVFinder(self._self)

  @property
  def raw_pointer(self):
    return self._self



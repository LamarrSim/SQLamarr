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

clib.new_CleanEventStore.argtypes = (ctypes.c_void_p,)
clib.new_CleanEventStore.restype = ctypes.c_void_p

clib.del_CleanEventStore.argtypes = (ctypes.c_void_p,)

class CleanEventStore:
  def __init__ (self, db):
    self._self = clib.new_CleanEventStore(db.get())
  
  def __del__(self):
    clib.del_CleanEventStore(self._self)

  @property
  def raw_pointer(self):
    return self._self




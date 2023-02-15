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

clib.new_PVReconstruction.argtypes = (
    ctypes.c_void_p, ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p
    )
clib.new_PVReconstruction.restype = ctypes.c_void_p

clib.del_PVReconstruction.argtypes = (ctypes.c_void_p,)

class PVReconstruction:
  def __init__ (self, db, file_name: str, table_name: str, condition: str):
    self._self = clib.new_PVReconstruction(db.get(), 
        file_name.encode('ascii'),
        table_name.encode('ascii'),
        condition.encode('ascii'),
        )
  
  def __del__(self):
    clib.del_PVReconstruction(self._self)

  @property
  def raw_pointer(self):
    return self._self




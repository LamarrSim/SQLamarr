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




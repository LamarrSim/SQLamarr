import ctypes
from ctypes import POINTER 
from SQLamarr import clib
from typing import List

clib.new_TemporaryTable.argtypes = (
    ctypes.c_void_p,        # void *db,
    ctypes.c_char_p,        # const char* output_table,
    ctypes.c_char_p,        # const char* comma_separated_outputs,
    ctypes.c_char_p,        # const char* query,
    ctypes.c_bool,          # bool make_persistent
    )

clib.new_TemporaryTable.restype = ctypes.c_void_p

clib.del_TemporaryTable.argtypes = (ctypes.c_void_p,)

class TemporaryTable:
  def __init__ (
      self, 
      db,
      output_table: str,
      outputs: str,
      query: str,
      make_persistent: bool,
      ):
    self._self = clib.new_TemporaryTable(
        db.get(),
        output_table.encode('ascii'),
        ",".join(outputs).encode('ascii'),
        query.encode('ascii'),
        make_persistent,
        )
  
  def __del__(self):
    clib.del_TemporaryTable(self._self)

  @property
  def raw_pointer(self):
    return self._self




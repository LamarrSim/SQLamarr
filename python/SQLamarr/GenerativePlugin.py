import ctypes
from ctypes import POINTER 
from SQLamarr import clib
from typing import List

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
    
clib.new_GenerativePlugin.restype = ctypes.c_void_p

clib.del_GenerativePlugin.argtypes = (ctypes.c_void_p,)

class GenerativePlugin:
  def __init__ (
      self, 
      db,
      library_path: str,
      function_name: str,
      query: str,
      output_table: str,
      outputs: List[str],
      nRandom: int,
      references: List[str]
      ):
    self._self = clib.new_GenerativePlugin(
        db.get(),
        library_path.encode('ascii'),
        function_name.encode('ascii'),
        query.encode('ascii'),
        output_table.encode('ascii'),
        ",".join(outputs).encode('ascii'),
        nRandom,
        ",".join(references).encode('ascii'),
        )
  
  def __del__(self):
    clib.del_GenerativePlugin(self._self)

  @property
  def raw_pointer(self):
    return self._self




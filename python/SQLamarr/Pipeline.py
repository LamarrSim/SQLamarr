import ctypes
from ctypes import POINTER 
from SQLamarr import clib

from typing import List, Any


clib.execute_pipeline.argtypes = (ctypes.c_int, POINTER(ctypes.c_void_p))

class Pipeline:
  def __init__(self, algoritms: List[Any]):
    self._algorithms = algoritms

  def execute(self):
    ArrayOfAlgos = ctypes.c_void_p * len(self._algorithms)
    buf = ArrayOfAlgos(*[a.raw_pointer for a in self._algorithms])
    clib.execute_pipeline (len(self._algorithms), buf)



import ctypes
from ctypes import POINTER 
from SQLamarr import clib

from typing import List, Any


clib.execute_pipeline.argtypes = (ctypes.c_int, POINTER(ctypes.c_void_p))

class Pipeline:
  def __init__(self, algoritms: List[Any]):
    self._algorithms = algoritms

  @staticmethod
  def _exec_chunk (chunk):
    ArrayOfAlgos = ctypes.c_void_p * len(chunk)
    buf = ArrayOfAlgos(*chunk)
    clib.execute_pipeline (len(chunk), buf)

  def execute(self):
    chunk = []
    for alg in self._algorithms:
      if hasattr(alg, '__call__'):
        self._exec_chunk(chunk)
        chunk = []
        alg()
      elif hasattr(alg, 'raw_pointer'):
        chunk.append(alg.raw_pointer)
      else:
        raise TypeError(
            f"Unexpected algorithm {alg} ({alg.__class__.__name__})"
            )

    self._exec_chunk(chunk)



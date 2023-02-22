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

from typing import List, Any


clib.execute_pipeline.argtypes = (ctypes.c_int, POINTER(c_TransformerPtr))
clib.execute_pipeline.restype = ctypes.c_int

SQL_ERRORSHIFT = 10000
LOGIC_ERRORSHIFT = 20000

class SQLiteError (RuntimeError):
  pass


class Pipeline:
  """
  The `Pipeline` object defines the envelop for running C++ transformers from
  Python. 
  
  One or multiple algorithms can be enqueed at construction time and executed 
  by calling the method `execute()`.

  C++-bounded transformer and `PyTransformer`s can be mixed up in the pipeline,
  however note that passing the control from a C++ algorithm to another C++
  algorithm has a much less overhead than passing the control to or from a
  Python algorithm. 
  
  Hence, if logically possible, one should avoid interleaving C++ and Python
  algorithms.
  """
  def __init__(self, algoritms: List[Any]):
    """
    Acquire the list of algorithms
    """
    self._algorithms = algoritms


  @staticmethod
  def _exec_chunk (chunk):
    """@private Execute a sequence of C++-only transformers"""
    ArrayOfAlgos = c_TransformerPtr * len(chunk)
    buf = ArrayOfAlgos(*[c.raw_pointer for c in chunk])
    ret = clib.execute_pipeline (len(chunk), buf)

    if ret >= SQL_ERRORSHIFT and ret < SQL_ERRORSHIFT + len(chunk):
      raise SQLiteError(f"Failed executing {chunk[ret-SQL_ERRORSHIFT]}")
    elif ret >= LOGIC_ERRORSHIFT and ret < LOGIC_ERRORSHIFT + len(chunk):
      raise RuntimeError(f"Failed executing {chunk[ret-LOGIC_ERRORSHIFT]}")
    elif ret != 0:
      raise Exception("Unknown error code from pipeline exec")

  def execute(self):
    """Execute the list of algorithms"""
    chunk = []
    for alg in self._algorithms:
      if hasattr(alg, '__call__'):
        self._exec_chunk(chunk)
        chunk = []
        alg()
      elif hasattr(alg, 'raw_pointer'):
        chunk.append(alg)
      else:
        raise TypeError(
            f"Unexpected algorithm {alg} ({alg.__class__.__name__})"
            )

    self._exec_chunk(chunk)



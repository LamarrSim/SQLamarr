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

from typing import List, Any


clib.execute_pipeline.argtypes = (ctypes.c_int, POINTER(ctypes.c_void_p))

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
    ArrayOfAlgos = ctypes.c_void_p * len(chunk)
    buf = ArrayOfAlgos(*chunk)
    clib.execute_pipeline (len(chunk), buf)

  def execute(self):
    """Execute the list of algorithms"""
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



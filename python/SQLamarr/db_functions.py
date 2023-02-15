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
import sqlite3
import contextlib

clib.make_database.argtypes = (POINTER(ctypes.c_char),)
clib.make_database.restype = ctypes.c_void_p

clib.del_database.argtypes = (ctypes.c_void_p,)

clib.GlobalPRNG_get_or_create.argtypes = (ctypes.c_void_p, ctypes.c_int)


class SQLite3DB:
  def __init__ (self, path: str = "file::memory:?cache=shared"):
    self._path = path
    self._pointer = clib.make_database(path.encode('ascii'))
  
  def __del__(self):
    clib.del_database(self._pointer)

  def get(self):
    return self._pointer

  def seed(self, seed: int):
    clib.GlobalPRNG_get_or_create(self._pointer, seed)

  @contextlib.contextmanager
  def connect(self):
    if self._path == ":memory:":
      raise NotImplementedError(
          "Cannot connect to an in-memory database without cache sharing"
          )

    with sqlite3.connect(self._path, uri=True) as db:
      yield db




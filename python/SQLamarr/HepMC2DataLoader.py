import ctypes
import os
from ctypes import POINTER 
from SQLamarr import clib
import sqlite3
import contextlib

clib.new_HepMC2DataLoader.argtypes = (ctypes.c_void_p,)
clib.new_HepMC2DataLoader.restype = ctypes.c_void_p

clib.del_HepMC2DataLoader.argtypes = (ctypes.c_void_p,)

clib.HepMC2DataLoader_load.argtypes = (
    ctypes.c_void_p, ctypes.c_char_p, ctypes.c_size_t, ctypes.c_size_t
    ) 

class HepMC2DataLoader:
  def __init__(self, db):
    self._self = clib.new_HepMC2DataLoader(db.get())
  
  def __del__ (self):
    clib.del_HepMC2DataLoader(self._self)

  def load(self, filename: str, runNumber: int, evtNumber: int):
    if not os.path.exists(filename):
      raise FileNotFoundError(filename)

    clib.HepMC2DataLoader_load(
        self._self, filename.encode('ascii'), runNumber, evtNumber
        )

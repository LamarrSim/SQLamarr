import ctypes
from ctypes import POINTER 
from SQLamarr import clib

clib.new_CleanEventStore.argtypes = (ctypes.c_void_p,)
clib.new_CleanEventStore.restype = ctypes.c_void_p

clib.del_CleanEventStore.argtypes = (ctypes.c_void_p,)

class CleanEventStore:
  def __init__ (self, db):
    self._self = clib.new_CleanEventStore(db.get())
  
  def __del__(self):
    clib.del_CleanEventStore(self._self)

  @property
  def raw_pointer(self):
    return self._self




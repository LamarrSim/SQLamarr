import ctypes
from ctypes import POINTER 
from SQLamarr import clib

clib.new_MCParticleSelector.argtypes = (ctypes.c_void_p,)
clib.new_MCParticleSelector.restype = ctypes.c_void_p

clib.del_MCParticleSelector.argtypes = (ctypes.c_void_p,)

class MCParticleSelector:
  def __init__ (self, db):
    self._self = clib.new_MCParticleSelector(db.get())
  
  def __del__(self):
    clib.del_MCParticleSelector(self._self)

  @property
  def raw_pointer(self):
    return self._self


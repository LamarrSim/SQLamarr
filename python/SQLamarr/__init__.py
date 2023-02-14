import ctypes
import glob
import os

cwd = os.path.dirname(__file__)

# Load the C++ library with bindings
resolution_attempts = (
    glob.glob(os.path.join(cwd, "libSQLamarr.*.so")) + 
    [
      "libSQLamarr.so",
      os.path.join(cwd, "libSQLamarr.so"),
      ]
    )



clib = None
for resolution_attempt in resolution_attempts:
  try:
    clib = ctypes.CDLL(resolution_attempt)
  except OSError:
    continue 
  else:
    break 

if clib is None:
  raise OSError("Failed loading libSQLamarr.so")

clib.get_version.restype = ctypes.c_char_p
version = str(clib.get_version(), "ascii")

## Database 
from SQLamarr.db_functions import SQLite3DB

## DataLoaders
from SQLamarr.HepMC2DataLoader import HepMC2DataLoader

## Transformers
from SQLamarr.PVFinder import PVFinder
from SQLamarr.MCParticleSelector import MCParticleSelector
from SQLamarr.PVReconstruction import PVReconstruction
from SQLamarr.Plugin import Plugin
from SQLamarr.GenerativePlugin import GenerativePlugin
from SQLamarr.TemporaryTable import TemporaryTable
from SQLamarr.CleanEventStore import CleanEventStore

## Python Transfomer
from SQLamarr.PyTransformer import PyTransformer

## Pipeline 
from SQLamarr.Pipeline import Pipeline


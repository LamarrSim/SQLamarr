# (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration. 
#                                                                             
# This software is distributed under the terms of the GNU General Public
# Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
#                                                                             
# In applying this licence, CERN does not waive the privileges and immunities
# granted to it by virtue of its status as an Intergovernmental Organization  
# or submit itself to any jurisdiction.

import ctypes
import os
from ctypes import POINTER 
from SQLamarr import clib
import sqlite3
import contextlib

from SQLamarr.db_functions import SQLite3DB

clib.new_HepMC2DataLoader.argtypes = (ctypes.c_void_p,)
clib.new_HepMC2DataLoader.restype = ctypes.c_void_p

clib.del_HepMC2DataLoader.argtypes = (ctypes.c_void_p,)

clib.HepMC2DataLoader_load.argtypes = (
    ctypes.c_void_p, ctypes.c_char_p, ctypes.c_size_t, ctypes.c_size_t
    ) 

class HepMC2DataLoader:
  """
  Data loader for HepMC2-format ASCII files.

  Example.
  ```python
  # Create the database connection
  db = SQLamarr.SQLite3DB()

  # Create the data loader
  data_loader = HepMCDataLoader(db)

  # Create the pipeline with a CleanEventStore algorithm
  clean_all = SQLamarr.Pipeline([
    SQLamarr.CleanEventStore(db)
    ])

  # For each file in an input file list,
  for file in my_list_of_files:
    # load the file
    data_loader.load(file)
    # ...do something...

    # clean the database
    clean_all.execute()
  ```
  """
  def __init__(self, db: SQLite3DB):
    """Acquires the reference to an open connection to the DB"""
    self._db = db
  
  def load(self, filename: str, runNumber: int, evtNumber: int):
    """Loads an ASCII file with
    [HepMC3::ReaderAsciiHepMC2](http://hepmc.web.cern.ch/hepmc/classHepMC3_1_1ReaderAsciiHepMC2.html).
    """
    if not os.path.exists(filename):
      raise FileNotFoundError(filename)

    _self = clib.new_HepMC2DataLoader(self._db.get())
    clib.HepMC2DataLoader_load(
        _self, filename.encode('ascii'), runNumber, evtNumber, self._db.path.encode('ascii')
        )
    clib.del_HepMC2DataLoader(_self)

# (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration. 
#                                                                             
# This software is distributed under the terms of the GNU General Public
# Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
#                                                                             
# In applying this licence, CERN does not waive the privileges and immunities
# granted to it by virtue of its status as an Intergovernmental Organization  
# or submit itself to any jurisdiction.

from SQLamarr.db_functions import SQLite3DB

class PyTransformer:
  """
  Decorator transforming a python function in a Transformer that can be
  pipelined to C++ algorithms in a `SQLamarr.Pipeline`.

  Example.
  ```python
  import SQLamarr
  import pandas as pd

  db = SQLamarr.SQLite3DB().seed(42)

  loader = SQLamarr.HepMC2DataLoader(db)
  
  pv_finder = SQLamarr.PVFinder(db)
  mcps = SQLamarr.MCParticleSelector(db)

  @SQLamarr.PyTransformer(db)
  def selector (connection):
    df = pd.read_sql_query("SELECT * FROM MCParticles", connection)
    df = df[df.pz > 10000]  # Apply a selection
    df.to_sql("MCParticles", connection, if_exists='replace')

  pipeline = SQLamarr.Pipeline((pv_finder, mcps, selector)) 
  
  loader.load("my_file_0.mc2", 42, 0)
  pipeline.execute()

  loader.load("my_file_1.mc2", 42, 1)
  pipeline.execute()

  ```


  """
  def __init__(self, db: SQLite3DB):
    """@private: Acquire the database connection"""
    self._db = db

  def __call__(self, f):
    """@private: Wrap a function in an algorithm"""
    def wrapped():
      with self._db.connect() as c:
        f (c)
    return wrapped


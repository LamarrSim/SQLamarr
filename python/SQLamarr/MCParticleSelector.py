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

from SQLamarr.db_functions import SQLite3DB

clib.new_MCParticleSelector.argtypes = (ctypes.c_void_p,)
clib.new_MCParticleSelector.restype = c_TransformerPtr

class MCParticleSelector:
  """
  Converts GenParticles into MCParticles preserving the graph structure.

  The event described with GenParticles and GenVertices is described with much
  more detail than what is needed to match reconstructed particles to their
  MC-true couterparts. `MCParticleSelector` is a Transformer copying the 
  GenParticles table into an MCParticle table, "skipping" particles irrelevant
  for MC matching while preserving the tree structure of the decay
  representation.

  Note that the `GenParticles` graph is a DAG, but not a tree, with vertices
  (graph nodes) defining multiple inputs (describing interactions) while the
  `MCParticle` graph is a tree, with each vertex (node) accepting a single input
  particle (decay vertex).
  """
  def __init__ (self, db: SQLite3DB):
    """Acquires the reference to an open connection to the DB"""
    self._self = clib.new_MCParticleSelector(db.get())
  
  def __del__(self):
    """@private: Release the bound class instance"""
    clib.del_Transformer(self._self)

  @property
  def raw_pointer(self):
    """@private: Return the raw pointer to the algorithm."""
    return self._self


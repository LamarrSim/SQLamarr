# (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration. 
#                                                                             
# This software is distributed under the terms of the GNU General Public
# Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
#                                                                             
# In applying this licence, CERN does not waive the privileges and immunities
# granted to it by virtue of its status as an Intergovernmental Organization  
# or submit itself to any jurisdiction.

import os
import ctypes
import glob

def _find_CDLL():
  """@private: resolve the compiled DLL"""
  cwd = os.path.dirname(__file__)

  # Define resolution strategy for the C++ shared object
  resolution_attempts = (
      # Search in the same directory as this source file, first (pip-installed)
      glob.glob(os.path.join(cwd, "libSQLamarr.*.so")) + 
      [
        # Search for the bare name without platform modifiers (manually-installed)
        os.path.join(cwd, "libSQLamarr.so"),
        # Search in the system lib/ directory (CMake-installed)
        "libSQLamarr.so",
        ]
      )


  ## Resolution logic
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

  return clib



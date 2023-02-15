# (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration. 
#                                                                             
# This software is distributed under the terms of the GNU General Public
# Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
#                                                                             
# In applying this licence, CERN does not waive the privileges and immunities
# granted to it by virtue of its status as an Intergovernmental Organization  
# or submit itself to any jurisdiction.

import ctypes

## Load the C++ library
from SQLamarr._find_CDLL import _find_CDLL
clib = _find_CDLL()

## Setup the version of the python package by reading the version of the CDLL
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


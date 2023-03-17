# (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration. 
#                                                                             
# This software is distributed under the terms of the GNU General Public
# Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
#                                                                             
# In applying this licence, CERN does not waive the privileges and immunities
# granted to it by virtue of its status as an Intergovernmental Organization  
# or submit itself to any jurisdiction.

from setuptools import Extension, setup
from glob import glob

ext = Extension(
    name="SQLamarr.libSQLamarr", 
    sources=[f for f in glob("src/*.cpp") if "main" not in f],
    include_dirs=["include"],
    language="c++" ,
    libraries=["m", "dl", "HepMC3", "sqlite3"],
    extra_compile_args=["-std=c++11"],
    )

setup(ext_modules=[ext])

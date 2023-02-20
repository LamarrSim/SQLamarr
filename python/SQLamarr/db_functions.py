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
from SQLamarr import clib
import sqlite3
import contextlib

clib.make_database.argtypes = (POINTER(ctypes.c_char),)
clib.make_database.restype = ctypes.c_void_p

clib.del_database.argtypes = (ctypes.c_void_p,)

clib.GlobalPRNG_get_or_create.argtypes = (ctypes.c_void_p, ctypes.c_int)


class SQLite3DB:
    """
    A database connection handler easying sharing the DB between C++ and Python.
    """

    def __init__(self, path: str = "file::memory:?cache=shared"):
        """
        Open the connection for the C++ application, with shared cache to ease 
        access from Python to the same tables.

        @param path: path-like or URI identifying the target resource; by 
          default, an non-threadsafe connection to an in-memory database is opened.

        ### Examples
        Connecting to an existing, prepared database stored on file `mydata.db`
        ```python
          db = SQLamarr.SQLite3DB("mydata.db")
        ```

        Connecting to an empty, in-memory database for prototyping
        ```python
          db = SQLamarr.SQLite3DB()
        ```

        Connecting to an empty, in-memory database in a multithreaded application
        with each thread working on its own db.
        ```python
          db_wn1 = SQLamarr.SQLite3DB("file:wn1?mode=memory&cache=shared")
          db_wn2 = SQLamarr.SQLite3DB("file:wn2?mode=memory&cache=shared")
        ```
        """
        self._path = path if path.startswith(
            "file:") else f"file:{path}?cache=shared"

        self._pointer = clib.make_database(path.encode('ascii'))

    def __del__(self):
        """@private: Return the raw pointer to the algorithm."""
        clib.del_database(self._pointer)

    def get(self):
        """@private: Return the raw pointer to the database."""
        return self._pointer

    def seed(self, seed: int):
        """
        Define the seed for the random number generated from Transformers
        interacting with the database through this connection.
        
        @param seed: integer seed for randomization

        @returns SQLite3DB (self) instance
        """
        clib.GlobalPRNG_get_or_create(self._pointer, seed)
        return self

    @contextlib.contextmanager
    def connect(self):
        """
        Python-like connection with a context-manager.

        Example.
        ```python

        # Define the DB handler and seeds the random number generator
        db = SQLamarr.SQLite3DB().seed(42)

        # Load some event from HepMC2 ASCII file
        loader = SQLamarr.HepMC2DataLoader(db)
        loader.load(<file_path>, <evtNumber>,<runNumber>)

        # Connect to the DB (while still open in C++ memory!)
        with db.connect() as c:
          # Loads in a pandas DataFrame the object
          df = pandas.read_sql_query("SELECT * FROM GenParticles")

        ```

        """
        if self._path == ":memory:":
            raise NotImplementedError(
                "Cannot connect to an in-memory database without cache sharing"
            )

        with sqlite3.connect(self._path, uri=True) as db:
            yield db

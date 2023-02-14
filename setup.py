from setuptools import Extension, setup
from glob import glob

ext = Extension(
    name="SQLamarr.libSQLamarr", 
    sources=[f for f in glob("src/*.cpp") if "main" not in f],
    include_dirs=["."],
    language="c++" ,
    libraries=["m", "dl", "HepMC3", "sqlite3"],
    )

setup(ext_modules=[ext])

# SQLamarr
*The stand-alone ultra-fast simulation option for LHCb*

The detailed simulation of the hadron collisions at the LHC, and of the 
interaction of the generated particles with the detector material
dominates the cost for the computing infrastructure pledged to the 
LHCb Collaboration.

Among the various options explored towards a faster simulation, 
there is Lamarr, a framework defining a pipeline of parametrizations
transforming generator-level quantities to reconstructed, analysis-level 
features. 
Most of the parametrizations are defined using *machine-learning*, and 
in particular Deep Neural Networks and Gradient Boosted Decision Trees,
with a traing procedure defined in independent packages (*e.g.* 
[landerlini/lb-trksim-train](https://github.com/landerlini/lb-trksim-train)
and [mbarbetti/lb-pidsim-train](https://github.com/mbarbetti/lb-pidsim-train)).

To be integrated in the LHCb software stack, models must be queried 
from a C++ application, running in the Gaudi framework, which includes a 
dedicated multithreading scheduler which was found to conflict with 
the schedulers of TensorFlow and ONNX runtimes.
In addition, since the models are relatively simple and fast to 
evaluate, the overhead of context switching from Gaudi to a dedicated 
runtime was observed to be unaccptably large.
Hence, models are converted into compatible C code using the 
[landerlini/scikinC](https://github.com/landerlini/scikinC)
package and distributed through the CernVM FileSystem releasing 
the [LamarrData package](https://gitlab.cern.ch/lhcb-datapkg/LamarrData).

While crucial to the applications within LHCb, the integration with
Gaudi and [Gauss](https://gitlab.cern.ch/lhcb/Gauss) makes the adoption 
of Lamarr unappealing for researchers outside of the LHCb community 
approaching the LHCb simulation to evaluate 
the experiment sensitivity to new physics phenomena or studying the 
recently-released LHCb Open Data.
The [landerlini/SQLamarr](https://github.com/landerlini/SQLamarr)
package aims at decoupling Lamarr from Gaudi providing a stand-alone 
application with minimal dependencies that can be easily set up and 
run in any Linux machine.
The parametrizations are shared between the Gauss-embedded implementation
[LbLamarr](https://gitlab.cern.ch/lhcb/Gauss/-/tree/master/Sim/LbLamarr) 
and `SQLamarr`.
In the future, the exact same package might be integrated within Gaudi 
to reduce the maintainance effort.

To replace the ROOT-based TransientEventStore concept defind in Gaudi,
`SQLamarr` adopts the SQLite3 package, enabling vectorized processing 
of batches of events, for a better performance.

To avoid dependencies on ROOT, also the persistency is handled using 
SQLite3, writing the reconstructed (or intermediate) quantities in the 
form of SQLite3 databases. 
Note that converting an SQLite3 table to a ROOT nTuple requires no more 
than 3 lines of Python:

```python
import sqlite3, uproot, pandas
with sqlite3.connect("SomeInput.db") as conn:
  uproot.open("SomeFile.root", "RECREATE")["myTree"] = pandas.read_sql_table("myTable", conn)
```

## Dependencies
 * [SQLite3](https://www.sqlite.org/index.html) with C/C++ headers
 * [HepMC3](http://hepmc.web.cern.ch/hepmc/) as a standard interface
  to event generators.

## Build from source
Make sure you have conda (or similar) installed, if not 
get [miniconda3](https://docs.conda.io/en/latest/miniconda.html).
Create and activate a dedicated conda environment, say `sqlamarr`:
```bash
conda create -y -n sqlamarr -c conda-forge python=3.10 gxx gxx_linux-64 hepmc3 doxygen
conda activate sqlamarr
```

Create a out-of-source directory:
```bash
mkdir build
cd build
```

Configure and build
```bash
cmake .. 
cmake --build .
```


## How to use SQLamarr
The project is not mature enough to provide a good user experience.
For the time being, clone the repository and compile the package with CMake, 
then edit the file `src/main.cpp` to
define the desired pipeline, by using the building blocks provided by
the package.

## BlockLib
The namespace `SQLamarr::BlockLib` groups functions defining specialized blocks
making assumptions on the workflow in which these blocks will be deployed.
While useful for testing and for organizing the code defining pipelines, 
it is not supposed to be stable (as it is being modified while the workflow
under test is modified) and should not be used as part of other packages. 

Other packages, however, may take inspiration from the structure of `SQLamarr::BlockLib`
to design specialized blocks, resident in their codebase, in a more 
organized way than having everything pipelined in a single file.

To test the completeness of the feature set in the main part of the library,
`SQLamarr::BlockLib` is designed to only include functions, accessing public methods 
of the objects defined in the main part of the library.


## Copyright and Licence
(c) Copyright 2022 CERN for the benefit of the LHCb Collaboration. 
                                                                            
This software is distributed under the terms of the GNU General Public
Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
                                                                            
In applying this licence, CERN does not waive the privileges and immunities
granted to it by virtue of its status as an Intergovernmental Organization  
or submit itself to any jurisdiction.

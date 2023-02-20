# (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration. 
#                                                                             
# This software is distributed under the terms of the GNU General Public
# Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
#                                                                             
# In applying this licence, CERN does not waive the privileges and immunities
# granted to it by virtue of its status as an Intergovernmental Organization  
# or submit itself to any jurisdiction.

import sys
sys.path.append("../python")

import SQLamarr
print (SQLamarr.version)

import os
#filename = "pytmp.db"
filename = "file:memdb?mode=memory&cache=shared"
if os.path.exists(filename):
  os.remove(filename)

db = SQLamarr.SQLite3DB(filename)
db.seed(123)


loader = SQLamarr.HepMC2DataLoader(db)
loader.load("../temporary_data/HepMC2-ascii/DSt_Pi.hepmc2/evt0.mc2", 456, 0)

pv_finder = SQLamarr.PVFinder(db)
mcps = SQLamarr.MCParticleSelector(db)
pvreco = SQLamarr.PVReconstruction(db, 
    "../temporary_data/PrimaryVertex/PrimaryVertexSmearing.db",
    "PVSmearing", "2016_pp_MagUp")

acceptance_model = SQLamarr.Plugin(db,
        "../temporary_data/models/lhcb.trk.2016MU.so",
        "acceptance", 
        """
        SELECT 
          mcparticle_id,
          ov.x AS mc_x, 
          ov.y AS mc_y, 
          ov.z AS mc_z,
          log10(norm2(p.px, p.py, p.pz)) AS mc_log10_p,
          p.px/p.pz AS mc_tx, 
          p.py/p.pz AS mc_ty,
          pseudorapidity(p.px, p.py, p.pz) AS mc_eta,
          azimuthal(p.px, p.py, p.pz) AS mc_phi,
          abs(p.pid) == 11 AS mc_is_e,
          abs(p.pid) == 13 AS mc_is_mu,
          (
            abs(p.pid) == 211 OR abs(p.pid) == 321 OR abs(p.pid) == 2212  
          ) AS mc_is_h,
          propagation_charge(p.pid) AS mc_charge
        FROM MCParticles AS p
        INNER JOIN MCVertices AS ov ON p.production_vertex = ov.mcvertex_id
        WHERE 
          p.pz > 1.
          AND
          propagation_charge(p.pid) <> 0.
        """,
        "tmp_acceptance_out", ["acceptance"], ["mcparticle_id"]
      )


@SQLamarr.PyTransformer(db)
def my_del (c):
  c.execute("SELECT * FROM GenParticles")

@SQLamarr.PyTransformer(db)
def get_version(c):
  for r in c.execute("SELECT sqlite_version();"):
    print (r)

clean_all = SQLamarr.CleanEventStore(db)
pipeline = SQLamarr.Pipeline((pv_finder, mcps, pvreco, acceptance_model)) 
pipeline.execute()


import pandas as pd
with db.connect() as c:
  for table in "GenVertices", "GenParticles", "MCParticles":
    df = pd.read_sql_query(f"SELECT * FROM {table}", c)
    print (table)
    print ("="*80)
    print (df)


--------------------------------------------------------------------------------
-- (c) Copyright 2021 CERN for the benefit of the LHCb Collaboration          --
--                                                                            --
-- This software is distributed under the terms of the GNU General Public     --
-- Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".  --
--                                                                            --
-- In applying this licence, CERN does not waive the privileges and immunities -
-- granted to it by virtue of its status as an Intergovernmental Organization --
-- or submit itself to any jurisdiction.                                      --
---------------------------------------------------------------------------------
-- See also: https://its.cern.ch/jira/browse/LHCBGAUSS-1287
-- Execute with:
--  $ sqlite3 -init PrimaryVertexSmearing.sql PrimaryVertexSmearing.db .quit

DROP TABLE IF EXISTS PVSmearing;

CREATE TABLE PVSmearing (
  pvsmearing_id INTEGER PRIMARY KEY,
  condition TEXT,
  coord TEXT,
  mu REAL,
  f1 REAL,
  f2 REAL,
  sigma1 REAL,
  sigma2 REAL,
  sigma3 REAL
);

INSERT INTO PVSmearing 
 (condition,       coord,        mu,      f1,      f2,  sigma1,  sigma2,  sigma3)
VALUES 
 (  '2015_pp_MagUp', 'X', -0.000033, 0.71440, 0.07935, 0.01109, 0.04393, 0.02087),
 (  '2015_pp_MagUp', 'Y',  0.000013, 0.69546, 0.07852, 0.01063, 0.04060, 0.19191),
 (  '2015_pp_MagUp', 'Z', -0.000190, 0.01556, 0.22276, 0.37696, 0.13423, 0.06089),
 ('2015_pp_MagDown', 'X',  0.000041, 0.70373, 0.08552, 0.01082, 0.04187, 0.02012),
 ('2015_pp_MagDown', 'Y', -0.000033, 0.70820, 0.07844, 0.01041, 0.04072, 0.01930),
 ('2015_pp_MagDown', 'Z', -0.000180, 0.01368, 0.21009, 0.38345, 0.13362, 0.06049),
 (  '2016_pp_MagUp', 'X',  0.000015, 0.39575, 0.13945, 0.02246, 0.04501, 0.01176),
 (  '2016_pp_MagUp', 'Y', -0.000054, 0.39621, 0.13293, 0.02129, 0.04236, 0.01130),
 (  '2016_pp_MagUp', 'Z',  0.000230, 0.03295, 0.73535, 0.42375, 0.07201, 0.17024),
 ('2016_pp_MagDown', 'X',  0.000015, 0.39575, 0.13945, 0.02246, 0.04501, 0.01176),
 ('2016_pp_MagDown', 'Y',  0.000018, 0.56669, 0.16150, 0.01160, 0.04330, 0.02243),
 ('2016_pp_MagDown', 'Z',  0.000890, 0.27633, 0.04973, 0.16142, 0.40333, 0.06978),
 (  '2017_pp_MagUp', 'X', -0.000033, 0.50859, 0.18137, 0.01238, 0.04589, 0.02336),
 (  '2017_pp_MagUp', 'Y',  0.000041, 0.58180, 0.15396, 0.01223, 0.04545, 0.02425),
 (  '2017_pp_MagUp', 'Z',  0.000200, 0.03839, 0.69368, 0.42427, 0.07200, 0.16817),
 ('2017_pp_MagDown', 'X', -0.000074, 0.53837, 0.16596, 0.01232, 0.04692, 0.02418),
 ('2017_pp_MagDown', 'Y',  0.000009, 0.54446, 0.16130, 0.01180, 0.04433, 0.02295),
 ('2017_pp_MagDown', 'Z', -0.000260, 0.03819, 0.68620, 0.58298, 0.06970, 0.16299),
 (  '2018_pp_MagUp', 'X', -0.000080, 0.54123, 0.16862, 0.01230, 0.04682, 0.02420),
 (  '2018_pp_MagUp', 'Y', -0.000033, 0.54574, 0.16732, 0.01187, 0.04410, 0.02308),
 (  '2018_pp_MagUp', 'Z', -0.000160, 0.03745, 0.70232, 0.42426, 0.07209, 0.17011),
 ('2018_pp_MagDown', 'X', -0.000079, 0.54080, 0.17710, 0.01249, 0.04630, 0.02393),
 ('2018_pp_MagDown', 'Y', -0.000059, 0.57869, 0.15315, 0.01223, 0.04540, 0.02381),
 ('2018_pp_MagDown', 'Z',  0.000150, 0.03805, 0.69137, 0.41754, 0.07009, 0.16409);

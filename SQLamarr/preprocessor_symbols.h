// (c) Copyright 2022 CERN for the benefit of the LHCb Collaboration.
//
// This software is distributed under the terms of the GNU General Public
// Licence version 3 (GPL Version 3), copied verbatim in the file "LICENCE".
//
// In applying this licence, CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.


// ERROR CODES 
#define LAMARR_BAD_INDEX -0xBADBAD

// VERTEX TYPE
#define LAMARR_VERTEX_PRIMARY     1
#define LAMARR_VERTEX_FROM_DECAY  2
#define LAMARR_VERTEX_UNKNOWN    -1



// LHCb-overridden GenParticle status
#define LAMARR_LHCB_UNKNWON                                         0     
#define LAMARR_LHCB_STABLE_IN_PRODGEN                               1
#define LAMARR_LHCB_DECAYED_BY_PRODGEN                              2
#define LAMARR_LHCB_DOCUMENTATION_PARTICLE                          3
#define LAMARR_LHCB_DECAYED_BY_DECAYGEN                             777
#define LAMARR_LHCB_DECAYED_BY_DECAYGEN_AND_PRODUCED_BY_PRODGEN     888
#define LAMARR_LHCB_SIGNAL_IN_LAB_FRAME                             889
#define LAMARR_LHCB_SIGNAL_AT_REST                                  998
#define LAMARR_LHCB_STABLE_IN_DECAYGEN                              999


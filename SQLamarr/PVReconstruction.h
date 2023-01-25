#pragma once

#include <array>

#include "SQLamarr/BaseSqlInterface.h"

namespace SQLamarr 
{
  /** Emulates the reconstruction of the primary vertex

    The position of each primary vertex is smeared according 
    to a parametrization loaded from a configuration file.
    The covariance matrix defining the uncertainty on the 
    position of the PV is also generated based on a 
    parametrization.

    The resolution model used to describe the difference 
    between the true and reconstructed positions of the PV
    is the sum of three Gaussian distributions sharing their 
    central values. 

    This simplified model includes two strong assumptions:
     * the resolution function does not depend on the number 
       of tracks from which the PV is obtained;
     * the covariance matrix is approximated with a diagonal 
       matrix neglecting correlations among axes
    */
  class PVReconstruction: public BaseSqlInterface
  {
    public:
      /// Set of parameters defining a 3-Gaussian resolution function in 1D
      struct SmearingParametrization_1D {
        float mu, f1, f2, sigma1, sigma2, sigma3;
      };

      /// Set of parmeters defining three 3-Gaussian functions for x, y and z
      struct SmearingParametrization {

        /// Access to raw data in array format 
        std::array<SmearingParametrization_1D, 3> data;

        /// Read/Write access to \f$x\f$ coordinate parametrization
        SmearingParametrization_1D& x() { return data[0]; }

        /// Read/Write access to \f$y\f$ coordinate parametrization
        SmearingParametrization_1D& y() { return data[1]; }

        /// Read/Write access to \f$z\f$ coordinate parametrization
        SmearingParametrization_1D& z() { return data[2]; }
      };

      PVReconstruction(
          SQLite3DB& db, 
          const SmearingParametrization& parametrization
          );

      static SmearingParametrization load_parametrization (
          const std::string file_path,
          const std::string table_name,
          unsigned int year,
          std::string polarity
          );
  };
}

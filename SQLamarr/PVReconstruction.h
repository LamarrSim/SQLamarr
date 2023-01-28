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
     - the resolution function does not depend on the number 
       of tracks from which the PV is obtained;
     - the covariance matrix is approximated with a diagonal 
       matrix neglecting correlations among axes


    Using PVReconstruction::load_parametrization(), 
    the parametrization can be loaded from an SQLite table 
    defining the following columns:
     - `condition`, a string representing the datataking conditions
     - `mu`, the mean of the Gaussians
     - `f1`, the core-fraction of the first Gaussian
     - `f2`, the core-fraction of the second Gaussian
     - `sigma1`, the standard deviation of the first Gaussian 
     - `sigma2`, the standard deviation of the second Gaussian 
     - `sigma3`, the standard deviation of the third Gaussian 
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

      /// Constructor 
      PVReconstruction(
          /// Reference to database (not owned)
          SQLite3DB& db,  

          /// 3-Gaussian parametrization of the resolution function
          const SmearingParametrization& parametrization 
          );

      /// Instanciate a SmearingParametrization object from an SQLite file
      static SmearingParametrization load_parametrization (
          const std::string file_path,  ///< SQLite database defining 
                                        ///  the parametrization
          const std::string table_name, ///< Name of the table defining 
                                        ///  the parametrization
          const std::string condition   ///< Datataking Condition
          );

      /// Execute the algorithm adding primary vertices to `Vertices` table
      void execute ();

    private: // members
      SmearingParametrization m_parametrization;

    private: // methods
      static void _sql_rnd_ggg (
          sqlite3_context *context,
          int argc,
          sqlite3_value **argv
          );

  };
}

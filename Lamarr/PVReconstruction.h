#pragma once

#include <array>

#include "Lamarr/BaseSqlInterface.h"

namespace Lamarr 
{
  class PVReconstruction: public BaseSqlInterface
  {
    public:
      struct SmearingParametrization_1D {
        float mu, f1, f2, sigma1, sigma2, sigma3;
      };

      struct SmearingParametrization {
        std::array<SmearingParametrization_1D, 3> data;
        SmearingParametrization_1D& x() { return data[0]; }
        SmearingParametrization_1D& y() { return data[1]; }
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

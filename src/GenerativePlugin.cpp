// STL
#include <random>

// SQLamarr
#include "SQLamarr/GenerativePlugin.h"
#include "SQLamarr/GlobalPRNG.h"

namespace SQLamarr 
{
  void GenerativePlugin::eval_parametrization (float* output, const float* input)
  { 
    std::vector<float> rnd(m_n_random);
    auto generator = GlobalPRNG::get_or_create(m_database.get());
    std::normal_distribution<float> gaussian;

    for (auto& r: rnd) 
      r = gaussian(*generator);

    (reinterpret_cast<ganfunc>(m_func))(output, input, rnd.data()); 
  }
}

#pragma once

namespace SQLamarr
{
  /// Interface to an executable class used for polymorphism
  class Transformer
  {
    public:
      /// Function to be overridden defining the execution step
      virtual void execute() = 0;
  };
}

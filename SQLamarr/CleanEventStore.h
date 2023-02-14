#pragma once

#include "SQLamarr/BaseSqlInterface.h"
#include "SQLamarr/Transformer.h"

namespace SQLamarr
{
  /** Removes all data from DB without affecting the schema
   *
   * When `execute`d, `CleanEventStore` gets a list of tables, 
   * including temporary instances, from SQLite3 schema, then loops over the 
   * existing tables and deletes all the rows.
   * This won't change the DB schema, nor change the DB connection data,
   * but clean the database to process another batch
   */
  class CleanEventStore: public BaseSqlInterface, Transformer
  {
    public:
      using BaseSqlInterface::BaseSqlInterface;

      /// Execute the algorithm, cleaning the database 
      void execute () override;
  };
}

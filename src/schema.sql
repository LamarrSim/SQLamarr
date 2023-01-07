constexpr char SQL_CREATE_SCHEMA[] = R"(

        CREATE TABLE IF NOT EXISTS DataSources (
          datasource_id INTEGER PRIMARY KEY AUTOINCREMENT,
          datasource TEXT, 
          run_number UNSIGNED BIG INT,
          evt_number UNSIGNED BIG INT
        );

        CREATE TABLE IF NOT EXISTS GenEvents (
          genevent_id INTEGER PRIMARY KEY AUTOINCREMENT, 
          collision INTEGER, 
          datasource_id INTEGER, 
          t REAL, 
          x REAL, 
          y REAL, 
          z REAL,
          FOREIGN KEY(datasource_id) REFERENCES DataSources(datasource_id)
        );

        CREATE TABLE IF NOT EXISTS GenVertices (
          genvertex_id INTEGER PRIMARY KEY AUTOINCREMENT,
          genevent_id INTEGER,
          hepmc_id INTEGER,
          status INTEGER,
          t REAL,
          x REAL,
          y REAL,
          z REAL,
          is_primary INTEGER,
          FOREIGN KEY(genevent_id) REFERENCES GenEvents(genevent_id)
        );

        CREATE INDEX IF NOT EXISTS GenVertices_genvertex_id 
          ON GenVertices (genvertex_id);
        CREATE INDEX IF NOT EXISTS GenVertices_genevent_id 
          ON GenVertices (genevent_id);

        CREATE TABLE IF NOT EXISTS GenParticles (
          genparticle_id INTEGER PRIMARY KEY AUTOINCREMENT,
          genevent_id INTEGER,
          hepmc_id INTEGER,
          production_vertex INTEGER,
          end_vertex INTEGER,
          pid INTEGER,
          status INTEGER,
          pe REAL,
          px REAL,
          py REAL,
          pz REAL,
          m REAL,
          FOREIGN KEY(genevent_id) REFERENCES GenEvents(genevent_id),
          FOREIGN KEY(production_vertex) REFERENCES GenVertices(genvertex_id),
          FOREIGN KEY(end_vertex) REFERENCES GenVertices(genvertex_id)
        );

        CREATE INDEX IF NOT EXISTS GenParticles_production_vertex 
          ON GenParticles (production_vertex);

        CREATE INDEX IF NOT EXISTS GenParticles_end_vertex 
          ON GenParticles (end_vertex);

        CREATE TABLE IF NOT EXISTS Vertices (
          vertex_id INTEGER PRIMARY KEY AUTOINCREMENT,
          genvertex_id INTEGER,
          genevent_id INTEGER,
          hepmc_id INTEGER,
          status INTEGER,
          t REAL,
          x REAL,
          y REAL,
          z REAL,
          FOREIGN KEY(genvertex_id) REFERENCES GenVertices(genvertex_id),
          FOREIGN KEY(genevent_id) REFERENCES GenEvents(genevent_id)
          );
)";

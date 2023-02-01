# SQLamarr extensions to SQLite

The [SQLite3](www.sqlite.org) library provides an extremely complete SQL dialect
to interact with tabular and relational data.

It also provides capabilities to extend the SQL language adding custom
functions.

In SQLamarr, several custom functions were defined, covering needs in two main 
areas:
 * Psudo-Random Number Generation;
 * Geometrical and Minkovsky metrics.

SQLamarr aims at providing tools to perform Monte~Carlo simulations of HEP
experiments. As for most Monte~Carlo applications, the generation of random numbers
is a critical aspect. Since SQLamarr might operate in multithreaded mode, random 
number generation must be handled carefully to preserve repeatability despite
the intrinsic randomicity of threading.
To this purpose, the `random()` function is not sufficient to the purpose of 
`SQLamarr` and was sided with `random_uniform()` and `random_category()`
providing an SQL interface to a set of Psudo-Random Number Generators (PRNGs) 
handled at application level with a Singleton design pattern (see
`SQLamarr::GlobalPRNG`).

Geometrical and Minkowski metrics are defined to improve readability 
of the SQL query, but in principle they could be expressed in query language 
as well.

## Geometrical functions
### `norm2 (x, y, z)`
Return the norm of a vector with coordinate $(x, y, z)$: 
$$\sqrt{x^2 + y^2 + z^2}.$$ 

### `pseudorapidity (x, y, z)`
Return the [pseudorapidity](https://en.wikipedia.org/wiki/Pseudorapidity) $\eta$ 
of a vector with coordinate $(x, y, z)$.
$$
\eta = -\log\left[\tan\left(\frac{\theta}{2}\right)\right] 
\qquad \mbox{where}\qquad
\theta = \arctan\left(\frac{\sqrt{x^2 + y^2}}{z^2}\right)
$$
### `polar (x, y, z)`
Return the [aziuthal angle](https://en.wikipedia.org/wiki/Spherical_coordinate_system) 
$\theta$ of a vector with coordinate $(x, y, z)$.
$$
\theta = \arctan\left(\frac{\sqrt{x^2 + y^2}}{z}\right)
$$
### `azimuthal (x, y, z)`
Return the [polar angle](https://en.wikipedia.org/wiki/Spherical_coordinate_system) 
$\varphi$ of a vector with coordinate $(x, y, z)$.
$$
\varphi = \arctan\left(\frac{y}{x}\right)
$$

## Random functions
### `random_uniform ()`
Generate a random number uniformly distributed between 0 and 1.

The pseudo-random sequence is stored as a function of the database instance
which should never be shared among multiple threads.

### `random_normal ()`
Generate a random number normally distributed as $\mathcal G(0,1)$.

The pseudo-random sequence is stored as a function of the database instance
which should never be shared among multiple threads.

### `random_category (p1 [, p2[, ... [, p10]]])`
Generate an integer random number according to the probabilities passed as 
arguments.
For example,
```sql
SELECT 
  random_category(0.2, 0.1)
```
will generate 0 with a probability of 20%, 1 with a 10% probability and 3 with
70% probability.

## Miscellaneous
### `propagation_charge (pdg_id)`
Define the charge based on the PDG ID code passed as an argument.
In particular:
 * 0: for photons and neutrons;
 * &pm;1 for pions, kaons and protons;
 * `NULL` for all other particles that Lamarr won't propagate through the
   detector;


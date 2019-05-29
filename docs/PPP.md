# Plans

##Improving how we determine physical particle properties

- Two levels of PPPs: low level has a separate HH group for each particle.
  Upper level has a collection of particles and their properties.

- Standard algorithm solutions
  Each algorithm has inputs, outputs, and a measure of function cost.


## Farther future:

- Inputs and outputs should be groupable into "families". For example,
  we want to be able to calculate volume fractions based on a bunch of different
  measures of a "length" scale. 

- Particle property / type map.
  - Types: (length\_type, volume\_type, ar\_type)

- Naming of the properties:
  - We can add aliases to these names to represent well-known properties.
  - HDF5 reserves "/" as a path separator.



Base
- fundamental types (length, volume, ar, ...)
- derived types (as a group)
 - group datasets / attributes include object name and the input variables.
 - add links to the input data. Hard links!
 - the dataset name is unique, but the group has a hardlink to the "data" dataset.
 - what should the group naming scheme be?
  - the basic derived type name is the parent group
  - then add pseudo-unique names depending on either the input dataset, or label 0, 1, 2, ...?

How do we request that particular data be generated?
- List the quantity that we want, with the fundamental types that we have available.
- This is a bit recursive. Naming can be determined using periods "."?


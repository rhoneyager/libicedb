# Discussion 2017-10-11

Only six people have requested access to the GitHub repository, so I am posting code and some discussion topics here.

Topics:

1. Building and compiling the library - [see HERE](https://icedb.met.honeyager.info/)
2. Attributes, tables, and netCDF - examples at [Related Pages/Attributes](https://icedb.met.honeyager.info/md_libicedb_docs_attributes.html) and [Modules/Attributes](https://icedb.met.honeyager.info/group__atts.html)
3. Example programs: on GitHub - [Reading and writing shapes](https://github.com/rhoneyager/libicedb/blob/master/apps/level_0/shapes-app.cpp), [Setting attributes](https://github.com/rhoneyager/libicedb/blob/master/apps/level_1/shapes-setattr-app.cpp), and [Creating tables](https://github.com/rhoneyager/libicedb/blob/master/apps/level_1/shapes-settbl-app.cpp). On this site - [Reading and writing shapes](https://icedb.met.honeyager.info/libicedb/apps/level_0/shapes-app.cpp), [Setting attributes](https://icedb.met.honeyager.info/libicedb/apps/level_1/shapes-setattr-app.cpp), [Creating tables](https://icedb.met.honeyager.info/libicedb/apps/level_1/shapes-settbl-app.cpp).
4. Level 1 Shape algorithms - [GitHub header file](https://github.com/rhoneyager/libicedb/blob/master/libicedb/icedb/level_1/candidate_shapeAlg.h) / [example algorithm implementation](https://github.com/rhoneyager/libicedb/blob/master/libicedb/src/candidate_shapeAlg.cpp). For those without GitHub: [the Header File](https://icedb.met.honeyager.info/candidate__shapeAlg_8h_source.html), [the header's documentation](https://icedb.met.honeyager.info/candidate__shapeAlg_8h.html), and [an example algorithm's implementation](https://icedb.met.honeyager.info/libicedb/libicedb/src/candidate_shapeAlg.cpp) --- Does anyone want to try to implement some simple algorithms? Like, finding the snowflake's mass, radius of gyration or minimally enclosing ellipsoid?

5. Some changes to the scattering data structure: integer variables and attributes need a well-defined size.

   

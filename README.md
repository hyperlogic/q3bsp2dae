q3bsp2dae
=============

Quake3Arena bsp file to collada converter.

TODO:
-------
   * Tessilate patches & output as triangles. (seems safer then outputing bezier patchs, cause the basis funcs are different i think)
   * Dump lightmaps as png files.
   * add lightmaps to dae

DONE:
--------
   * Load in BSP file
   * Extract visual mesh data into usable format.
   * Integrate COLLADAStreamWriter
   * Output library_geometries
   * Output visual_scene nodes
   * Output library_materials and library_effects


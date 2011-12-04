bsp2dae
=============

Quake3Arena bsp file to collada converter.

TODO:
* Integrate COLLADAStreamWriter
* Output mesh
* Tessilate patches & output as triangles. (seems safer then outputing bezier patchs, cause the basis funcs are different i think)
* Dump lightmaps as png files.

DONE:
* Load in BSP file
* Extract visual mesh data into usable format.

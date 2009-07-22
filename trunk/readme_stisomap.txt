
ST-Isomap mouse trajectory example
==================================

Chad Jenkins (cjenkins@cs.brown.edu, http://www.cs.brown.edu/~cjenkins/)
Date: 02/11/2005

----------------------------------

An example applying continuous Spatio-temporal Isomap for a 2D mouse trajectory.  This code is a modification of the original Isomap Matlab source (available at http://isomap.stanford.edu) for spatio-temporal data.  By spatio-temporal data, I mean data with nonlinear spatial structure and a sequential ordering.  ST-Isomap code was originally written for Matlab 5.3 (R11).  The current version of the code has been tested only on Matlab 6.5.

The original Isomap source has conditions about its usage.  Please refer to "isomap/Readme_from_isomap_distribution" for details about these conditions.  I would further the disclaimer from the Isomap authors to state that the current state of this code is hacky at best.  

For the algorithmic details about the ST-Isomap, please refer the following paper:

O.C. Jenkins and M.J Mataric, "A Spatio-temporal Extension to Isomap Nonlinear Dimension Reduction", The International Conference on Machine Learning (ICML 2004), pp. 441-448, 2004

If you find this code or the paper useful, please a citation would be appreciated.

----------------------------------

This archive (stisomap_example.zip) contains the following modifications to the original Isomap distribution:

runme.m - *try this first* a script that performs Isomap, temporally-windowed MDS, and ST-Isomap on a mouse-click trajectory

DistMat.m - computes a Euclidean distance matrix          
DistMatWin.m - computes a Euclidean distance matrix between windows of points
emds.m - Multidimensional Scaling (extracted from the original Isomap code)              
gc_var2.mat - a 2D mouse-click trajectory
ginput_c2.mat - another 2D mouse-click trajectory   
readme_stisomap.txt - this file 
record_mouse.m - a function to record a 2D trajectory as a series of mouse clicks      
isomap/IsomapIIst.m - ST-Isomap as a modified version of IsomapII.m                     

These following files have been included from the original Isomap distribution:

isomap/Readme_from_isomap_distribution  
isomap/IsomapII.m                       
isomap/L2_distance.m                    
isomap/dfun.m                           
isomap/dijk.m                           
isomap/dijkstra.cpp                     
isomap/dijkstra.dll                     
isomap/dijkstra.m                       
isomap/fibheap.h                        

----------------------------------

Please direct any question, comments, or concerns to Chad Jenkins (cjenkins@cs.brown.edu).  I get avalanches of email on a typical day.  Do not get upset if it takes me some time to respond, it is not personal.

Happy embedding.
-Chad
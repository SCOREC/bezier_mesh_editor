# Bezier Mesh Editor
__Bezier Mesh Editor__ is Qt appliction to visualize and edit Bezier meshes. It provides the ability to modify vertices and control points of Bezier enities to make changes in the mesh.

It is well-know in FEM community that the exponential rate of convergence for `p`-version finite element can only be achieved if the order of geometric approximation for the the underlying mesh is high enough to accurately capture the curved domain boundaries. This requires higher-order representations for the meshes and the ability to modfiy curved mesh entities during mesh adaptation. Implementation and Development of mesh modification operators in the case of curved meshes require a lot more care compared with linear meshes. One of the difficulties in developing such operators is the fact that visualizing highly complex curved cavities is often very difficult on paper. Hence the need for a 3D visualiztion tool. The hope here is that having such a visualization tool would help the process of designing new cavity operators for curved meshes.  

# Build Instructions and Dependencies
This tool requires an installation of [SCOREC/Core](https://github.com/SCOREC/core). A basic set of instruction for doing so using `CMake` is provided below (you will need to update `path_to_install_directory` to the location on your system where you want the install files to be written):

```
git clone --recursive https://github.com/SCOREC/core
cd core
mkdir build
cd build
cmake .. \
  -DCMAKE_BUILD_TYPE="Release" \
  -DCMAKE_INSTALL_PREFIX:PATH="path_to_install_directory" \
  -DCMAKE_C_COMPILER="/usr/bin/mpicc.mpich" \
  -DCMAKE_CXX_COMPILER="/usr/bin/mpicxx.mpich" \
  -DCMAKE_EXE_LINKER_FLAGS="-lpthread" \
  -DSCOREC_CXX_OPTIMIZE=ON \
  -DBUILD_EXES=ON \
  -DMESHES="../pumi-meshes" \
  -DIS_TESTING=ON

make -j 8
make install
```

The appication itself can be build inside Qt Creator. __(This has been tested on version 6.0.1 of Qt Creator)__

First clone the repository. Then you will need to edit the following lines in `CMakeLists.txt`

```
set(CMAKE_C_COMPILER "/usr/bin/mpicc.mpich")      #this must point to the local implementation of mpicc in your system
set(CMAKE_CXX_COMPILER "/usr/bin/mpicxx.mpich")   #this must point to the local implementation of mpicxx in your system
set(SCOREC_DIR "/home/morteza/Sandbox/install/core")   # this must point the root directory where SCOREC/Core installation is located
```

The open Qt Creator. From the `File` menu select `Open File or Project` and open the `CMakeFileLists.txt`. Follow the on screen instructions to configure the project (using all the defaults should work). Once that is done you can run the application by clicking the run button. 

# Some Notes

* Uses SCOREC Core as the mesh data structure
* Can load meshes and shows the control net
* Control points can be moved around to modify the mesh
* The final mesh can be saved

The following pictures show how this works in actions


![bmesh_editor_01](https://user-images.githubusercontent.com/1325140/185806726-bae95185-c287-4cef-9527-deba2505dfa3.png)

![bmesh_editor_02](https://user-images.githubusercontent.com/1325140/185806730-71de4b24-19ec-4ede-a947-65a4cc16b9bf.png)

![bmesh_editor_03](https://user-images.githubusercontent.com/1325140/185806736-c399360c-affd-4519-86b5-9e451b8b9d19.png)


# Future Developments

* Generalization to 3D
* More functionality (e.g. show invalid elements)
* ...

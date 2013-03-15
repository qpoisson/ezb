EZ-Mesh binary reader provided by John W. Ratcliff mailto:jratcliffscarab@gmail.com

This project provides a copy of the MeshConvert tool which can convert a varity of 3D graphics
file formats into the EZ-Mesh format.

The primary value of this tool is so that you can take an .FBX file exported from 3D Studio Max or Maya
and then convert it into a format that you can easily process by your own program.

The MeshConvert tool can process meshes, materials, skeletons, and animation data from FBX files.

It also provides a very small single source file which can read a binary EZ-Mesh files into a flat data structure in memory.

The provided MeshConvert.exe tool can handle the following input graphics file formats.

-------------------------------------------------------------------------------------------
SUPPORTED INPUT FORMATS
-------------------------------------------------------------------------------------------
APX  : APEX render mesh XML format for the NVIDIA APEX SDK; mesh data only, no skeleton or animation supoprt.
EZM  : The XML version of the EZ-Mesh file format; mesh, skeleton, and animation support
EZB  : The binary version of of the EZ-Mesh file format, mesh, skeleton and animation support.
FBX  : The AutoDesk FBX file format that can be exported by both Max and Maya.  Supports mesh, skeleton, and animation data.
OBJ  : The Waveront OBJ text file format.  Only supports mesh data.
XML  : The Ogre3D file format; supports mesh, skeleton, and animation data.
PSK  : The legacy UE3 .PSK format for skeletal characters; supports mesh and skeleton.


-------------------------------------------------------------------------------------------
SUPPORTED OUTPUT FORMATS
-------------------------------------------------------------------------------------------
APX  : APEX render mesh XML format for the NVIDIA APEX SDK; mesh data only, no skeleton or animation supoprt.
EZM  : The XML version of the EZ-Mesh file format; mesh, skeleton, and animation support
EZB  : The binary version of of the EZ-Mesh file format, mesh, skeleton and animation support.
OBJ  : The Waveront OBJ text file format.  Only supports mesh data.
XML  : The Ogre3D file format; supports mesh, skeleton, and animation data.
PSK  : The legacy UE3 .PSK format for skeletal characters; supports mesh and skeleton.


-------------------------------------------------------------------------------------------
Documentation on how to use the MeshConvert.exe tool to convert file formats.
-------------------------------------------------------------------------------------------
Usage: MeshConvert <name> (options)

-r x y z   : Rotates the mesh by this euler rotation. 90 0 0 rotates a Y-up axis mesh to be Z-up.
-s scale   : Scales the input mesh by this amount.  50 to go from metric to UE3 and 0.02 to go from UE3 to metric.
-f format  : Output formats are OBJ, EZM, EZB, XML (Ogre3d), PSK, APX
-h height  : Resizes the mesh so it is centered and scaled to this height.  Bottom of the mesh at Y=
-n         : Recompute face normals
-combine   : Looks for a series of meshes numbered <name>01 <name>02, etc. and combines them into a single skeletal mesh.
-flipV		: Flip's the V texture co-ordinate.
-x			: Extracts the mesh based on bone weighting into a series of output files.

Here are some examples:

"MeshConvert CrashTestDummy.fbx -f ezb"

This will load the test file 'CrashTestDummy.fbx" and save it out in binary EZ-Mesh format as 'CrashTestDummy.ezb'


-------------------------------------------------------------------------------------------
The CodeSuppository.exe Testing Tool
-------------------------------------------------------------------------------------------
Provided is a simple application that will let you load one of the supported file formats and view it
in a very basic way.  It will visualize the mesh, the skeleton, and can playback animation data.

Just run 'CodeSuppository.exe', then select 'File' and 'Import Mesh Data'.  Select any file in one of the
supported formats.  Example, load 'deer_bound.ezb'.  Select 'Play Animation' and use the Animation Speed slider.

Use the left mouse and WASD keys to nagvigate around the object.



-------------------------------------------------------------------------------------------
How to use the EZ-Mesh source code in your own project.
-------------------------------------------------------------------------------------------
The EZ-Mesh source code is dependent on the NVIDIA PhysX SDK foundation header files for very
basic data types.

However, I do not have legal permission to redistribute these header files so, instead, I have created
placeholder stub versions which compile fine.

If you intend to use this code in your project it is recommended that you either (A) grab the foundation
header files from the PhysX SDK or (B) refactor these placeholder versions to match the coding
style of your project.

The placeholder foundation header files are located in the sub-directory 'foundation'.

Not counting the foundation header files, you only need three additional source files to load the EZ-Mesh data
in binary format.

They are:

./MeshImport/EZMesh.h  : This header file defines the EZ-Mesh data structures in a flat layout.
./MeshImport/MeshImportEZB.h : This header file defines the API to load a binary image of an EZB file into memory.
./MeshImport/MeshImportEZB.cpp : This is the source code implementation to parse a binary .EZB file into an EZ-MeshSystem structure

Here is the way to use this code:

Given a block of memory which points to a flat .EZB file call the following method:

ezmesh::MeshImportEZB *ezb = ezmesh::createMeshImportEZB(mem);

Once you have this interface you will want to get access to the flat EZ-MeshSystem data to copy into your own
tool chain or graphics formats.

Call this method:  ezmesh::MeshSystem *ms = ezb->getMeshSystem();

Once you have finished processing the data, you need to release this class, which will also free up any associated
memory allocations which were created to hold the EZ-Mesh data.

Simply call the 'release' method ont he ezmesh::MeshImportEZB

For testing purposes, and to give you an example of how to walk the EZ-MeshSystem data structures, source code
has been provided to two exporters.  One will just save the data right back out to a binary EZ-Mesh file (.ezb) and
the other will save the mesh data into into an ASCII .XML file version of EZ-Mesh.



-------------------------------------------------------------------------------------------
Here is a Walk-Thru example of how to use this tool chain provided
-------------------------------------------------------------------------------------------

First, let's convert an FBX file with a skeleton and animation data into binary EZ-Mesh

Run: 'MeshConvert CrashTestDummy.fbx -f ezb'

This will produce the file 'CreateTestDummy.ezb'

Now, let's use this provided source code tool to conver the binary EZB into a human readable ASCII XML representation.

Run: 'ezb CreateTestDummy.ezb'

This will produce two new files; one 'CreateTestDummy.xml' and another 'CrashTestDummy_EXPORT.ezb' which is a freshly
saved binary copy of the data.

You can do the same thing with a much smaller sample file called 'leaf.fbx'

Now, run 'CodeSuppository.exe' and use 'File' 'Import Mesh' to test that all of these worked.

-------------------------------------------------------------------------------------------
HOW TO BUILD
-------------------------------------------------------------------------------------------
To build and run the source code, load the Visual Studio Solution file 'ezb.sln'

After building it, set the current working directory and pass in a valid command line.

This code has been written to be able work on multiple platforms and takes 'endian' specific
memory layout into account.

I have not yet built it on anything other than with Windows; so there may be some very minor
compile issues to resolve, but those should be very simple.

To recap, the only source you need to load binary EZ-meshes (with meshes, skeletons, and key framed animation data)
into your project are:

./MeshImport/EZMesh.h
./MeshImport/MeshImportEZB.h
./MeshImport/MeshImportEZB.cpp

You will also need either the foundation header files provided in:

./foundation

Or, use the PhysX SDK foundation header files.


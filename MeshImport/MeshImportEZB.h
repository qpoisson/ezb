#ifndef MESH_IMPORT_EZB_H

#define MESH_IMPORT_EZB_H

namespace mimp
{

class MeshSystem;

class MeshImportEZB
{
public:
	virtual MeshSystem *getMeshSystem(void) = 0;
	virtual void release(void) = 0;
protected:
	virtual ~MeshImportEZB(void)
	{
	}
};


MeshImportEZB *createMeshImportEZB(const void *data);

}; // end of mimp namespace

#endif

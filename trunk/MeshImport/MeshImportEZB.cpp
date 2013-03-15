/*!
**
** Written by John W. Ratcliff : March 15, 2013 
**
** Copyright (c) 2013 by John W. Ratcliff mailto:jratcliffscarab@gmail.com
** If you wish to contact me you can use the following methods:
** Personal website: http://jratcliffscarab.blogspot.com
** Coding Website:   http://codesuppository.blogspot.com
**
**
** The MIT license:
**
** Permission is hereby granted, MEMALLOC_FREE of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is furnished
** to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.

** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
** WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
** CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#include "MeshImportEZB.h"
#include "EZMesh.h"
#include <new.h>


namespace ezmesh
{

#define MI_ALLOC(x) ::malloc(x)
#define MI_FREE(x) ::free(x)

/**
\brief A wrapper class for MiFileBuf that provides both binary and ASCII streaming capabilities
*/
class InputStream
{
	static const physx::PxU32 MAX_STREAM_STRING = 1024;
public:
	class TempAlloc
	{
	public:
		TempAlloc(void)
		{
			mData = (char *)this+sizeof(TempAlloc);
			mNext = NULL;
		}
		~TempAlloc(void)
		{
		}
		char		*mData;
		TempAlloc	*mNext;
	};

	static bool isBigEndian()
	{
		physx::PxI32 i = 1;
		return *((char*)&i)==0;
	}


	InputStream(void)
	{
		mInputStream = NULL;
		mEndianSwap = !isBigEndian(); // by default streams are read/written in big endian format so that they are higher performance on consoles.  
		mTempAllocs = NULL;
	}

	~InputStream(void) 
	{ 
		TempAlloc *f = mTempAllocs;
		while ( f )
		{
			TempAlloc *next = f->mNext;
			MI_FREE(f);
			f = next;
		}
	};

	void init(const void *inputStream) 
	{ 
		mInputStream = (const physx::PxU8 *)inputStream;
	};

	PX_INLINE void swap2Bytes(void* _data) const
	{
		if ( mEndianSwap )
		{
			char *data = (char *)_data;
			char one_byte;
			one_byte = data[0]; data[0] = data[1]; data[1] = one_byte;
		}
	}

	PX_INLINE void swap4Bytes(void* _data) const
	{
		if ( mEndianSwap )
		{
			char *data = (char *)_data;
			char one_byte;
			one_byte = data[0]; data[0] = data[3]; data[3] = one_byte;
			one_byte = data[1]; data[1] = data[2]; data[2] = one_byte;
		}
	}

	PX_INLINE void swap8Bytes(void *_data) const
	{
		if ( mEndianSwap )
		{
			char *data = (char *)_data;
			char one_byte;
			one_byte = data[0]; data[0] = data[7]; data[7] = one_byte;
			one_byte = data[1]; data[1] = data[6]; data[6] = one_byte;
			one_byte = data[2]; data[2] = data[5]; data[5] = one_byte;
			one_byte = data[3]; data[3] = data[4]; data[4] = one_byte;
		}
	}



	PX_INLINE InputStream& operator>>(const char *&str)
	{
		str = ""; // by default no string streamed...
		physx::PxU32 len=0;
		*this >> len;
		PX_ASSERT( len < (MAX_STREAM_STRING-1) );
		if ( len < (MAX_STREAM_STRING-1) )
		{
			TempAlloc *ta = (TempAlloc *)MI_ALLOC(len+1+sizeof(TempAlloc));
			new ( ta ) TempAlloc;
			memcpy(ta->mData,mInputStream,len);
			mInputStream+=len;
			ta->mData[len] = 0;
			str = ta->mData;
			ta->mNext = mTempAllocs;
			mTempAllocs = ta;
		}
		return *this;
	}

	PX_INLINE InputStream& operator>>(bool &bv)
	{
		physx::PxU32 v = (physx::PxU32)bv;
		v = *(const bool *)mInputStream;
		mInputStream+=sizeof(v);
		swap4Bytes(&v);
		return *this;
	}

	PX_INLINE InputStream& operator>>(char &v)
	{
		v = *(const char *)mInputStream;
		mInputStream+=sizeof(v);
		return *this;
	}

	PX_INLINE InputStream& operator>>(physx::PxU8 &v)
	{
		v = *(const physx::PxU8 *)mInputStream;
		mInputStream+=sizeof(v);
		return *this;
	}

	PX_INLINE InputStream& operator>>(physx::PxI8 &v)
	{
		v = *(const physx::PxI8 *)mInputStream;
		mInputStream+=sizeof(v);
		return *this;
	}

	PX_INLINE InputStream& operator>>(physx::PxI64 &v)
	{
		v = *(const physx::PxI64 *)mInputStream;
		swap8Bytes(&v);
		mInputStream+=sizeof(v);
		return *this;
	}

	PX_INLINE InputStream& operator>>(physx::PxU64 &v)
	{
		v = *(const physx::PxU64 *)mInputStream;
		mInputStream+=sizeof(v);
		swap8Bytes(&v);
		return *this;
	}

	PX_INLINE InputStream& operator>>(physx::PxF64 &v)
	{
		v = *(const physx::PxF64 *)mInputStream;
		mInputStream+=sizeof(v);
		swap8Bytes(&v);
		return *this;
	}

	PX_INLINE InputStream& operator>>(physx::PxF32 &v)
	{
		v = *(const physx::PxF32 *)mInputStream;
		mInputStream+=sizeof(v);
		swap4Bytes(&v);
		return *this;
	}

	PX_INLINE InputStream& operator>>(physx::PxU32 &v)
	{
		v = *(const physx::PxU32 *)mInputStream;
		mInputStream+=sizeof(v);
		swap4Bytes(&v);
		return *this;
	}

	PX_INLINE InputStream& operator>>(physx::PxI32 &v)
	{
		v = *(const physx::PxI32 *)mInputStream;
		mInputStream+=sizeof(v);
		swap4Bytes(&v);
		return *this;
	}
	PX_INLINE InputStream& operator>>(physx::PxU16 &v)
	{
		v = *(const physx::PxU16 *)mInputStream;
		mInputStream+=sizeof(v);
		swap2Bytes(&v);
		return *this;
	}
	PX_INLINE InputStream& operator>>(physx::PxI16 &v)
	{
		v = *(const physx::PxI16 *)mInputStream;
		mInputStream+=sizeof(v);
		swap2Bytes(&v);
		return *this;
	}

	void read(void *dest,physx::PxU32 len)
	{
		memcpy(dest,mInputStream,len);
		mInputStream+=len;
	}

private:
	bool					mEndianSwap;
	const physx::PxU8		*mInputStream;
	TempAlloc				*mTempAllocs;
};

void deserialize(InputStream &mStream,physx::PxBounds3 &m)
{
	mStream >> m.minimum[0];
	mStream >> m.minimum[1];
	mStream >> m.minimum[2];
	mStream >> m.maximum[0];
	mStream >> m.maximum[1];
	mStream >> m.maximum[2];
}
void deserialize(InputStream &mStream,MeshRawTexture &m)
{
	mStream >> m.mName;
	mStream >> m.mWidth;
	mStream >> m.mHeight;
	mStream >> m.mBPP;
	physx::PxU32 dataLen = 0;
	mStream >> dataLen;
	if ( dataLen )
	{
		m.mData = (physx::PxU8 *)MI_ALLOC(dataLen);
		mStream.read(m.mData,dataLen);
	}
}

void deserialize(InputStream & /*mStream*/,MeshTetra & /*mrt*/)
{
	PX_ALWAYS_ASSERT(); // Not yet implemented
}

void deserialize(InputStream &mStream,MeshBone &m)
{
	mStream >> m.mName;
	mStream >> m.mParentIndex;
	mStream >> m.mPosition[0];
	mStream >> m.mPosition[1];
	mStream >> m.mPosition[2];
	mStream >> m.mOrientation.x;
	mStream >> m.mOrientation.y;
	mStream >> m.mOrientation.z;
	mStream >> m.mOrientation.w;
	mStream >> m.mScale[0];
	mStream >> m.mScale[1];
	mStream >> m.mScale[2];
}

void deserialize(InputStream &mStream,MeshSkeleton &m)
{
	mStream >> m.mName;
	mStream >> m.mBoneCount;
	if ( m.mBoneCount )
	{
		m.mBones = (MeshBone *)MI_ALLOC(sizeof(MeshBone)*m.mBoneCount);
		for (physx::PxI32 i=0; i<m.mBoneCount; i++)
		{
			new ( &m.mBones[i] ) MeshBone;
		}
	}
	for (physx::PxI32 i=0; i<m.mBoneCount; i++)
	{
		MeshBone &b = m.mBones[i];
		deserialize(mStream,b);
	}
}

void deserialize(InputStream &mStream,MeshAnimPose &m)
{
	mStream >> m.mPos[0];
	mStream >> m.mPos[1];
	mStream >> m.mPos[2];
	mStream >> m.mQuat.x;
	mStream >> m.mQuat.y;
	mStream >> m.mQuat.z;
	mStream >> m.mQuat.w;
	mStream >> m.mScale[0];
	mStream >> m.mScale[1];
	mStream >> m.mScale[2];
}

void deserialize(InputStream &mStream,MeshAnimTrack &m)
{
	mStream >> m.mName;
	mStream >> m.mFrameCount;
	mStream >> m.mDuration;
	mStream >> m.mDtime;
	if ( m.mFrameCount )
	{
		m.mPose = (MeshAnimPose *)MI_ALLOC(sizeof(MeshAnimPose)*m.mFrameCount);
		for (physx::PxI32 i=0; i<m.mFrameCount; i++)
		{
			new ( &m.mPose[i] ) MeshAnimPose;
		}
	}
	for (physx::PxI32 i=0; i<m.mFrameCount; i++)
	{
		MeshAnimPose &p = m.mPose[i];
		deserialize(mStream,p);
	}
}

void deserialize(InputStream &mStream,MeshAnimation &m)
{
	mStream >> m.mName;
	mStream >> m.mTrackCount;
	mStream >> m.mFrameCount;
	mStream >> m.mDuration;
	mStream >> m.mDtime;
	if ( m.mTrackCount )
	{
		m.mTracks = (MeshAnimTrack **)MI_ALLOC(sizeof(MeshAnimTrack *)*m.mTrackCount);
	}
	for (physx::PxI32 i=0; i<m.mTrackCount; i++)
	{
		MeshAnimTrack *t = (MeshAnimTrack *)MI_ALLOC(sizeof(MeshAnimTrack));
		new ( t ) MeshAnimTrack;
		deserialize(mStream,*t);
		m.mTracks[i] = t;
	}
}

void deserialize(InputStream &mStream,MeshMaterial &m)
{
	mStream >> m.mName;
	mStream >> m.mMetaData;
}

void deserialize(InputStream &mStream,MeshUserData &m)
{
	mStream >> m.mUserKey;
	mStream >> m.mUserValue;
}

void deserialize(InputStream &mStream,MeshUserBinaryData &m)
{
	mStream >> m.mName;
	mStream >> m.mUserLen;
	if ( m.mUserLen )
	{
		m.mUserData = (physx::PxU8 *)MI_ALLOC(m.mUserLen);
		mStream.read(m.mUserData,m.mUserLen);
	}
}

void deserialize(InputStream &mStream,SubMesh &m)
{
	mStream >> m.mMaterialName;
	deserialize(mStream,m.mAABB);
	mStream >> m.mVertexFlags;
	mStream >> m.mTriCount;
	if ( m.mTriCount )
	{
		m.mIndices = (physx::PxU32 *)MI_ALLOC(sizeof(physx::PxU32)*m.mTriCount*3);
	}
	for (physx::PxU32 i=0; i<m.mTriCount*3; i++)
	{
		mStream >> m.mIndices[i];
	}
}

void deserialize(InputStream &mStream,MeshVertex &m,physx::PxU32 vertexFlags)
{

	if ( vertexFlags & MIVF_POSITION )
	{
		mStream >> m.mPos[0];
		mStream >> m.mPos[1];
		mStream >> m.mPos[2];
	}
	if ( vertexFlags & MIVF_NORMAL )
	{
		mStream >> m.mNormal[0];
		mStream >> m.mNormal[1];
		mStream >> m.mNormal[2];
	}
	if ( vertexFlags & MIVF_COLOR )
	{
		mStream >> m.mColor;
	}
	if ( vertexFlags & MIVF_TEXEL1 )
	{
		mStream >> m.mTexel1[0];
		mStream >> m.mTexel1[1];
	}
	if ( vertexFlags & MIVF_TEXEL2 )
	{
		mStream >> m.mTexel2[0];
		mStream >> m.mTexel2[1];
	}
	if ( vertexFlags & MIVF_TEXEL3 )
	{
		mStream >> m.mTexel3[0];
		mStream >> m.mTexel3[1];
	}
	if ( vertexFlags & MIVF_TEXEL4 )
	{
		mStream >> m.mTexel4[0];
		mStream >> m.mTexel4[1];
	}
	if ( vertexFlags & MIVF_TANGENT )
	{
		mStream >> m.mTangent[0];
		mStream >> m.mTangent[1];
		mStream >> m.mTangent[2];
	}
	if ( vertexFlags & MIVF_BINORMAL )
	{
		mStream >> m.mBiNormal[0];
		mStream >> m.mBiNormal[1];
		mStream >> m.mBiNormal[2];
	}
	if ( vertexFlags & MIVF_BONE_WEIGHTING  )
	{
		mStream >> m.mBone[0];
		mStream >> m.mBone[1];
		mStream >> m.mBone[2];
		mStream >> m.mBone[3];
		mStream >> m.mWeight[0];
		mStream >> m.mWeight[1];
		mStream >> m.mWeight[2];
		mStream >> m.mWeight[3];
	}
	if ( vertexFlags & MIVF_RADIUS )
	{
		mStream >> m.mRadius;
	}
	if ( vertexFlags & MIVF_INTERP1 )
	{
		mStream >> m.mInterp1[0];
		mStream >> m.mInterp1[1];
		mStream >> m.mInterp1[2];
		mStream >> m.mInterp1[3];
	}
	if ( vertexFlags & MIVF_INTERP2 )
	{
		mStream >> m.mInterp2[0];
		mStream >> m.mInterp2[1];
		mStream >> m.mInterp2[2];
		mStream >> m.mInterp2[3];
	}
	if ( vertexFlags & MIVF_INTERP3 )
	{
		mStream >> m.mInterp3[0];
		mStream >> m.mInterp3[1];
		mStream >> m.mInterp3[2];
		mStream >> m.mInterp3[3];
	}
	if ( vertexFlags & MIVF_INTERP4 )
	{
		mStream >> m.mInterp4[0];
		mStream >> m.mInterp4[1];
		mStream >> m.mInterp4[2];
		mStream >> m.mInterp4[3];
	}
	if ( vertexFlags & MIVF_INTERP5 )
	{
		mStream >> m.mInterp5[0];
		mStream >> m.mInterp5[1];
		mStream >> m.mInterp5[2];
		mStream >> m.mInterp5[3];
	}
	if ( vertexFlags & MIVF_INTERP6 )
	{
		mStream >> m.mInterp6[0];
		mStream >> m.mInterp6[1];
		mStream >> m.mInterp6[2];
		mStream >> m.mInterp6[3];
	}
	if ( vertexFlags & MIVF_INTERP7 )
	{
		mStream >> m.mInterp7[0];
		mStream >> m.mInterp7[1];
		mStream >> m.mInterp7[2];
		mStream >> m.mInterp7[3];
	}
	if ( vertexFlags & MIVF_INTERP8 )
	{
		mStream >> m.mInterp8[0];
		mStream >> m.mInterp8[1];
		mStream >> m.mInterp8[2];
		mStream >> m.mInterp8[3];
	}


}


void deserialize(InputStream &mStream,Mesh &m)
{
	mStream >> m.mName;
	mStream >> m.mSkeletonName;
	deserialize(mStream,m.mAABB);
	mStream >> m.mSubMeshCount;
	if ( m.mSubMeshCount )
	{
		m.mSubMeshes = (SubMesh **)MI_ALLOC(sizeof(SubMesh *)*m.mSubMeshCount);
	}
	for (physx::PxU32 i=0; i<m.mSubMeshCount; i++)
	{
		SubMesh *s = (SubMesh *)MI_ALLOC(sizeof(SubMesh));
		new (s ) SubMesh;
		deserialize(mStream,*s);
		m.mSubMeshes[i] = s;
	}
	mStream >> m.mVertexFlags;
	mStream >> m.mVertexCount;
	if ( m.mVertexCount )
	{
		m.mVertices = (MeshVertex *)MI_ALLOC(sizeof(MeshVertex)*m.mVertexCount);
	}
	for (physx::PxU32 i=0; i<m.mVertexCount; i++)
	{
		MeshVertex &v = m.mVertices[i];
		new ( &v ) MeshVertex;
		deserialize(mStream,v,m.mVertexFlags);
	}
}

void deserialize(InputStream &mStream,MeshInstance &m)
{
	mStream >> m.mMeshName;
	mStream >> m.mPosition[0];
	mStream >> m.mPosition[1];
	mStream >> m.mPosition[2];
	mStream >> m.mRotation.x;
	mStream >> m.mRotation.y;
	mStream >> m.mRotation.z;
	mStream >> m.mRotation.w;
	mStream >> m.mScale[0];
	mStream >> m.mScale[1];
	mStream >> m.mScale[2];
}

void deserialize(InputStream & /*mStream*/,const MeshCollisionRepresentation & /*m*/)
{
	PX_ALWAYS_ASSERT(); // need to implement
}


class _MeshImportEZB : public MeshImportEZB, public MeshSystem
{
public:
	_MeshImportEZB(const void *data)
	{
		mStream.init(data);
		physx::PxU32 version = 0;
		mStream >> version;
		const char *tag;
		mStream >> tag;
		if ( version == MESH_BINARY_VERSION && tag && strcmp(tag,"EZMESH") == 0  )
		{
			mStream >> mAssetName;
			mStream >> mAssetInfo;
			mStream >> mMeshSystemVersion;
			mStream >> mAssetVersion;
			deserialize(mStream,mAABB);
			mStream >> mTextureCount;
			if ( mTextureCount )
			{
				mTextures = (MeshRawTexture **)MI_ALLOC(sizeof(MeshRawTexture *)*mTextureCount);
			}
			for (physx::PxU32 i=0; i<mTextureCount; i++)
			{
				MeshRawTexture *t = (MeshRawTexture *)MI_ALLOC(sizeof(MeshRawTexture));
				new ( t ) MeshRawTexture;
				deserialize(mStream,*t);
				mTextures[i] = t;
			}
			mStream >> mTetraMeshCount;
			if ( mTetraMeshCount )
			{
				mTetraMeshes = (MeshTetra **)MI_ALLOC(sizeof(MeshTetra *)*mTetraMeshCount);
			}
			for (physx::PxU32 i=0; i< mTetraMeshCount; i++)
			{
				MeshTetra *m = (MeshTetra *)MI_ALLOC(sizeof(MeshTetra));
				new ( m ) MeshTetra;
				deserialize(mStream,*m);
				mTetraMeshes[i] = m;
			}
			mStream >> mSkeletonCount;
			if ( mSkeletonCount )
			{
				mSkeletons = (MeshSkeleton **)MI_ALLOC(sizeof(MeshSkeleton *)*mSkeletonCount);
			}
			for (physx::PxU32 i=0; i< mSkeletonCount; i++)
			{
				MeshSkeleton *m =  (MeshSkeleton *)MI_ALLOC(sizeof(MeshSkeleton));
				new ( m ) MeshSkeleton;
				deserialize(mStream,*m);
				mSkeletons[i] = m;
			}
			mStream >> mAnimationCount;
			if ( mAnimationCount )
			{
				mAnimations = (MeshAnimation **)MI_ALLOC(sizeof(MeshAnimation *)*mAnimationCount);
			}
			for (physx::PxU32 i=0; i<mAnimationCount; i++)
			{
				MeshAnimation *m = (MeshAnimation *)MI_ALLOC(sizeof(MeshAnimation));
				new ( m ) MeshAnimation;
				deserialize(mStream,*m);
				mAnimations[i] = m;
			}
			mStream >> mMaterialCount; 
			if ( mMaterialCount )
			{
				mMaterials =  (MeshMaterial *)MI_ALLOC(sizeof(MeshMaterial)*mMaterialCount);
			}
			for (physx::PxU32 i=0; i<mMaterialCount; i++)
			{
				MeshMaterial &m = mMaterials[i];
				new ( &m ) MeshMaterial;
				deserialize(mStream,m);
			}
			mStream >> mUserDataCount;
			if ( mUserDataCount )
			{
				mUserData = (MeshUserData **)MI_ALLOC(sizeof(MeshUserData *)*mUserDataCount);
			}
			for (physx::PxU32 i=0; i<mUserDataCount; i++)
			{
				MeshUserData *m = (MeshUserData *)MI_ALLOC(sizeof(MeshUserData));
				new ( m ) MeshUserData;
				deserialize(mStream,*m);
				mUserData[i] = m;
			}
			mStream >> mUserBinaryDataCount;
			if ( mUserBinaryDataCount )
			{
				mUserBinaryData = (MeshUserBinaryData **)MI_ALLOC(sizeof(MeshUserBinaryData **)*mUserBinaryDataCount);
			}
			for (physx::PxU32 i=0; i<mUserBinaryDataCount; i++)
			{
				MeshUserBinaryData *m = (MeshUserBinaryData *)MI_ALLOC(sizeof(MeshUserBinaryData));
				new ( m ) MeshUserBinaryData;
				deserialize(mStream,*m);
				mUserBinaryData[i] = m;
			}
			mStream >> mMeshCount;
			if ( mMeshCount )
			{
				mMeshes = (Mesh **)MI_ALLOC(sizeof(Mesh)*mMeshCount);
			}
			for (physx::PxU32 i=0; i<mMeshCount; i++)
			{
				Mesh *m = (Mesh *)MI_ALLOC(sizeof(Mesh));
				new ( m ) Mesh;
				deserialize(mStream,*m);
				mMeshes[i] = m;
			}
			mStream >> mMeshInstanceCount;
			if ( mMeshInstanceCount )
			{
				mMeshInstances = (MeshInstance *)MI_ALLOC(sizeof(MeshInstance)*mMeshInstanceCount);
			}
			if ( mMeshInstanceCount )
			{
				mMeshInstances = (MeshInstance *)MI_ALLOC(sizeof(MeshInstance)*mMeshInstanceCount);
			}
			for (physx::PxU32 i=0; i<mMeshInstanceCount; i++)
			{
				MeshInstance &m = mMeshInstances[i];
				new ( &m ) MeshInstance;
				deserialize(mStream,m);
			}
			mStream >> mMeshCollisionCount;
			if ( mMeshCollisionCount )
			{
				mMeshCollisionRepresentations = (MeshCollisionRepresentation **)MI_ALLOC(sizeof(MeshCollisionRepresentation *)*mMeshCollisionCount);
			}
			for (physx::PxU32 i=0; i<mMeshCollisionCount; i++)
			{
				MeshCollisionRepresentation *m = (MeshCollisionRepresentation *)MI_ALLOC(sizeof(MeshCollisionRepresentation));
				new ( m ) MeshCollisionRepresentation;
				deserialize(mStream,*m);
				mMeshCollisionRepresentations[i] = m;
			}
			mStream >> mPlane.n.x;
			mStream >> mPlane.n.y;
			mStream >> mPlane.n.z;
			mStream >> mPlane.d;
		}
	}

	virtual ~_MeshImportEZB(void)
	{
		// Free up textures
		for (physx::PxU32 i=0; i<mTextureCount; i++)
		{
			MI_FREE(mTextures[i]->mData);
			MI_FREE(mTextures[i]);
		}
		MI_FREE(mTextures);

		// Free up tetrahedral meshes
		for (physx::PxU32 i=0; i<mTetraMeshCount; i++)
		{
			MI_FREE(mTetraMeshes[i]);
		}
		MI_FREE(mTetraMeshes);

		// Free up skeletons
		for (physx::PxU32 i=0; i<mSkeletonCount; i++)
		{
			MI_FREE(mSkeletons[i]->mBones);
			MI_FREE(mSkeletons[i]);
		}
		MI_FREE(mSkeletons);

		// Free up animations
		for (physx::PxU32 i=0; i<mAnimationCount; i++)
		{
			MeshAnimation *ma = mAnimations[i];
			for (physx::PxI32 j=0; j<ma->mTrackCount; j++)
			{
				MeshAnimTrack *track = ma->mTracks[j];
				MI_FREE(track->mPose);
				MI_FREE(track);
			}
			MI_FREE(ma->mTracks);
			MI_FREE(ma);
		}
		MI_FREE(mAnimations);

		// Free up materials
		MI_FREE(mMaterials);

		// Free up user data
		for (physx::PxU32 i=0; i<mUserDataCount; i++)
		{
			MI_FREE(mUserData[i]);
		}
		MI_FREE(mUserData);

		// Free up user binary data
		for (physx::PxU32 i=0; i<mUserBinaryDataCount; i++)
		{
			MI_FREE(mUserBinaryData[i]->mUserData);
			MI_FREE(mUserBinaryData[i]);
		}
		MI_FREE(mUserBinaryData);

		// Free up meshes
		for (physx::PxU32 i=0; i<mMeshCount; i++)
		{
			Mesh *m = mMeshes[i];
			for (physx::PxU32 j=0; j<m->mSubMeshCount; j++)
			{
				SubMesh *sm = m->mSubMeshes[j];
				MI_FREE(sm->mIndices);
				MI_FREE(sm);
			}
			MI_FREE(m->mVertices);
			MI_FREE(m->mSubMeshes);
			MI_FREE(m);
		}
		MI_FREE(mMeshes);

		// Free up mesh instances
		MI_FREE(mMeshInstances);

		// Free up collision representations
		for (physx::PxU32 i=0; i<mMeshCollisionCount; i++)
		{
			MI_FREE(mMeshCollisionRepresentations[i]);
		}
		MI_FREE(mMeshCollisionRepresentations);

	}

	virtual MeshSystem *getMeshSystem(void) 
	{
		return this;
	}

	virtual void release(void)
	{
		delete this;
	}

	InputStream	mStream;
};

MeshImportEZB *createMeshImportEZB(const void *data)
{
	_MeshImportEZB *ret = new _MeshImportEZB(data);
	return static_cast< _MeshImportEZB *>(ret);
}

}; // end of namespace

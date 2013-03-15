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
#include "MeshExportEZB.h"
#include "EZMesh.h"
#include <stdio.h>

#pragma warning(disable:4996)

namespace ezmesh
{

	/**
	\brief A wrapper class for MiFileBuf that provides both binary and ASCII streaming capabilities
	*/
	class OutputStream
	{
		static const physx::PxU32 MAX_STREAM_STRING = 1024;
	public:
		static bool isBigEndian()
		{
			physx::PxI32 i = 1;
			return *((char*)&i)==0;
		}


		OutputStream(FILE *fph)
		{
			mFph = fph;
			mEndianSwap = !isBigEndian(); // by default streams are read/written in big endian format so that they are higher performance on consoles.  
		}

		~OutputStream(void) 
		{ 
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


		PX_INLINE OutputStream& operator<<(const char *str)
		{
			physx::PxU32 len = str ? (physx::PxU32)strlen(str) : 0;
			*this<<len;
			if ( len )
			{
				fwrite(str,len,1,mFph);
			}
			return *this;
		}

		PX_INLINE OutputStream& operator<<(bool bv)
		{
			physx::PxU32 v = (physx::PxU32)bv;
			swap4Bytes(&v);
			fwrite(&v,sizeof(v),1,mFph);
			return *this;
		}

		PX_INLINE OutputStream& operator<<(char v)
		{
			fwrite(&v,sizeof(v),1,mFph);
			return *this;
		}

		PX_INLINE OutputStream& operator>>(physx::PxU8 v)
		{
			fwrite(&v,sizeof(v),1,mFph);
			return *this;
		}

		PX_INLINE OutputStream& operator<<(physx::PxI8 v)
		{
			fwrite(&v,sizeof(v),1,mFph);
			return *this;
		}

		PX_INLINE OutputStream& operator<<(physx::PxI64 v)
		{
			swap8Bytes(&v);
			fwrite(&v,sizeof(v),1,mFph);
			return *this;
		}

		PX_INLINE OutputStream& operator<<(physx::PxU64 v)
		{
			swap8Bytes(&v);
			fwrite(&v,sizeof(v),1,mFph);
			return *this;
		}

		PX_INLINE OutputStream& operator<<(physx::PxF64 v)
		{
			swap8Bytes(&v);
			fwrite(&v,sizeof(v),1,mFph);
			return *this;
		}

		PX_INLINE OutputStream& operator<<(physx::PxF32 v)
		{
			swap4Bytes(&v);
			fwrite(&v,sizeof(v),1,mFph);
			return *this;
		}

		PX_INLINE OutputStream& operator<<(physx::PxU32 v)
		{
			swap4Bytes(&v);
			fwrite(&v,sizeof(v),1,mFph);
			return *this;
		}

		PX_INLINE OutputStream& operator<<(physx::PxI32 v)
		{
			swap4Bytes(&v);
			fwrite(&v,sizeof(v),1,mFph);
			return *this;
		}
		PX_INLINE OutputStream& operator<<(physx::PxU16 v)
		{
			swap2Bytes(&v);
			fwrite(&v,sizeof(v),1,mFph);
			return *this;
		}
		PX_INLINE OutputStream& operator<<(physx::PxI16 v)
		{
			swap2Bytes(&v);
			fwrite(&v,sizeof(v),1,mFph);
			return *this;
		}



		PX_INLINE void write(const void *data,physx::PxU32 len)
		{
			fwrite(data,len,1,mFph);
		}

	private:
		FILE					*mFph;
		bool					mEndianSwap;
	};


	static void serialize(OutputStream &stream,const physx::PxBounds3 &aabb)
{
	stream << aabb.minimum[0];
	stream << aabb.minimum[1];
	stream << aabb.minimum[2];
	stream << aabb.maximum[0];
	stream << aabb.maximum[1];
	stream << aabb.maximum[2];
}

static void serialize(OutputStream &stream,const MeshRawTexture &m)
{
	stream << m.mName;
	stream << m.mWidth;
	stream << m.mHeight;
	stream << m.mBPP;

	physx::PxU32 dataLen = m.mData ? m.mWidth*m.mHeight*m.mBPP : 0;
	stream << dataLen;
	if ( dataLen )
	{
		stream.write(m.mData,dataLen);
	}
}

static void serialize(OutputStream & /*stream*/,const MeshTetra & /*mrt*/)
{
	PX_ALWAYS_ASSERT(); // Not yet implemented
}

static void serialize(OutputStream &stream,const MeshBone &m)
{
	stream << m.mName;
	stream << m.mParentIndex;
	stream << m.mPosition[0];
	stream << m.mPosition[1];
	stream << m.mPosition[2];
	stream << m.mOrientation.x;
	stream << m.mOrientation.y;
	stream << m.mOrientation.z;
	stream << m.mOrientation.w;
	stream << m.mScale[0];
	stream << m.mScale[1];
	stream << m.mScale[2];
}

static void serialize(OutputStream &stream,const MeshSkeleton &m)
{
	stream << m.mName;
	stream << m.mBoneCount;
	for (physx::PxI32 i=0; i<m.mBoneCount; i++)
	{
		const MeshBone &b = m.mBones[i];
		serialize(stream,b);
	}
}

static void serialize(OutputStream &stream,const MeshAnimPose &m)
{
	stream << m.mPos[0];
	stream << m.mPos[1];
	stream << m.mPos[2];
	stream << m.mQuat.x;
	stream << m.mQuat.y;
	stream << m.mQuat.z;
	stream << m.mQuat.w;
	stream << m.mScale[0];
	stream << m.mScale[1];
	stream << m.mScale[2];
}

static void serialize(OutputStream &stream,const MeshAnimTrack &m)
{
	stream << m.mName;
	stream << m.mFrameCount;
	stream << m.mDuration;
	stream << m.mDtime;
	for (physx::PxI32 i=0; i<m.mFrameCount; i++)
	{
		const MeshAnimPose &p = m.mPose[i];
		serialize(stream,p);
	}
}

static void serialize(OutputStream &stream,const MeshAnimation &m)
{
	stream << m.mName;
	stream << m.mTrackCount;
	stream << m.mFrameCount;
	stream << m.mDuration;
	stream << m.mDtime;
	for (physx::PxI32 i=0; i<m.mTrackCount; i++)
	{
		const MeshAnimTrack &t = *m.mTracks[i];
		serialize(stream,t);
	}
}

static void serialize(OutputStream &stream,const MeshMaterial &m)
{
	stream << m.mName;
	stream << m.mMetaData;
}

static void serialize(OutputStream &stream,const MeshUserData &m)
{
	stream << m.mUserKey;
	stream << m.mUserValue;
}

static void serialize(OutputStream &stream,const MeshUserBinaryData &m)
{
	stream << m.mName;
	stream << m.mUserLen;
	if ( m.mUserLen )
	{
		stream.write(m.mUserData,m.mUserLen);
	}
}

static void serialize(OutputStream &stream,const SubMesh &m)
{
	stream << m.mMaterialName;
	serialize(stream,m.mAABB);
	stream << m.mVertexFlags;
	stream << m.mTriCount;
	for (physx::PxU32 i=0; i<m.mTriCount*3; i++)
	{
		stream << m.mIndices[i];
	}
}

static void serialize(OutputStream &stream,const MeshVertex &m,physx::PxU32 vertexFlags)
{

	if ( vertexFlags & MIVF_POSITION )
	{
		stream << m.mPos[0];
		stream << m.mPos[1];
		stream << m.mPos[2];
	}
	if ( vertexFlags & MIVF_NORMAL )
	{
		stream << m.mNormal[0];
		stream << m.mNormal[1];
		stream << m.mNormal[2];
	}
	if ( vertexFlags & MIVF_COLOR )
	{
		stream << m.mColor;
	}
	if ( vertexFlags & MIVF_TEXEL1 )
	{
		stream << m.mTexel1[0];
		stream << m.mTexel1[1];
	}
	if ( vertexFlags & MIVF_TEXEL2 )
	{
		stream << m.mTexel2[0];
		stream << m.mTexel2[1];
	}
	if ( vertexFlags & MIVF_TEXEL3 )
	{
		stream << m.mTexel3[0];
		stream << m.mTexel3[1];
	}
	if ( vertexFlags & MIVF_TEXEL4 )
	{
		stream << m.mTexel4[0];
		stream << m.mTexel4[1];
	}
	if ( vertexFlags & MIVF_TANGENT )
	{
		stream << m.mTangent[0];
		stream << m.mTangent[1];
		stream << m.mTangent[2];
	}
	if ( vertexFlags & MIVF_BINORMAL )
	{
		stream << m.mBiNormal[0];
		stream << m.mBiNormal[1];
		stream << m.mBiNormal[2];
	}
	if ( vertexFlags & MIVF_BONE_WEIGHTING  )
	{
		stream << m.mBone[0];
		stream << m.mBone[1];
		stream << m.mBone[2];
		stream << m.mBone[3];
		stream << m.mWeight[0];
		stream << m.mWeight[1];
		stream << m.mWeight[2];
		stream << m.mWeight[3];
	}
	if ( vertexFlags & MIVF_RADIUS )
	{
		stream << m.mRadius;
	}
	if ( vertexFlags & MIVF_INTERP1 )
	{
		stream << m.mInterp1[0];
		stream << m.mInterp1[1];
		stream << m.mInterp1[2];
		stream << m.mInterp1[3];
	}
	if ( vertexFlags & MIVF_INTERP2 )
	{
		stream << m.mInterp2[0];
		stream << m.mInterp2[1];
		stream << m.mInterp2[2];
		stream << m.mInterp2[3];
	}
	if ( vertexFlags & MIVF_INTERP3 )
	{
		stream << m.mInterp3[0];
		stream << m.mInterp3[1];
		stream << m.mInterp3[2];
		stream << m.mInterp3[3];
	}
	if ( vertexFlags & MIVF_INTERP4 )
	{
		stream << m.mInterp4[0];
		stream << m.mInterp4[1];
		stream << m.mInterp4[2];
		stream << m.mInterp4[3];
	}
	if ( vertexFlags & MIVF_INTERP5 )
	{
		stream << m.mInterp5[0];
		stream << m.mInterp5[1];
		stream << m.mInterp5[2];
		stream << m.mInterp5[3];
	}
	if ( vertexFlags & MIVF_INTERP6 )
	{
		stream << m.mInterp6[0];
		stream << m.mInterp6[1];
		stream << m.mInterp6[2];
		stream << m.mInterp6[3];
	}
	if ( vertexFlags & MIVF_INTERP7 )
	{
		stream << m.mInterp7[0];
		stream << m.mInterp7[1];
		stream << m.mInterp7[2];
		stream << m.mInterp7[3];
	}
	if ( vertexFlags & MIVF_INTERP8 )
	{
		stream << m.mInterp8[0];
		stream << m.mInterp8[1];
		stream << m.mInterp8[2];
		stream << m.mInterp8[3];
	}


}


static void serialize(OutputStream &stream,const Mesh &m)
{
	stream << m.mName;
	stream << m.mSkeletonName;
	serialize(stream,m.mAABB);
	stream << m.mSubMeshCount;
	for (physx::PxU32 i=0; i<m.mSubMeshCount; i++)
	{
		const SubMesh &s = *m.mSubMeshes[i];
		serialize(stream,s);
	}
	stream << m.mVertexFlags;
	stream << m.mVertexCount;
	for (physx::PxU32 i=0; i<m.mVertexCount; i++)
	{
		const MeshVertex &v = m.mVertices[i];
		serialize(stream,v,m.mVertexFlags);
	}
}

static void serialize(OutputStream &stream,const MeshInstance &m)
{
	stream << m.mMeshName;
	stream << m.mPosition[0];
	stream << m.mPosition[1];
	stream << m.mPosition[2];
	stream << m.mRotation.x;
	stream << m.mRotation.y;
	stream << m.mRotation.z;
	stream << m.mRotation.w;
	stream << m.mScale[0];
	stream << m.mScale[1];
	stream << m.mScale[2];
}

static void serialize(OutputStream & /*stream*/,const MeshCollisionRepresentation & /*m*/)
{
	PX_ALWAYS_ASSERT(); // need to implement
}


static void serializeEZB(FILE *fph,const MeshSystem *ms)
{
	if ( ms && fph)
	{
		OutputStream stream(fph);
		physx::PxU32 version = MESH_BINARY_VERSION;
		stream << version;
		stream << "EZMESH";
		stream << ms->mAssetName;
		stream << ms->mAssetInfo;
		stream << ms->mMeshSystemVersion;
		stream << ms->mAssetVersion;
		serialize(stream,ms->mAABB);
		stream << ms->mTextureCount;
		for (physx::PxU32 i=0; i<ms->mTextureCount; i++)
		{
			const MeshRawTexture &t = *ms->mTextures[i];
			serialize(stream,t);
		}
		stream << ms->mTetraMeshCount;
		for (physx::PxU32 i=0; i< ms->mTetraMeshCount; i++)
		{
			const MeshTetra &mt = *ms->mTetraMeshes[i];
			serialize(stream,mt);
		}
		stream << ms->mSkeletonCount;
		for (physx::PxU32 i=0; i< ms->mSkeletonCount; i++)
		{
			const MeshSkeleton &m = *ms->mSkeletons[i];
			serialize(stream,m);
		}
		stream << ms->mAnimationCount;
		for (physx::PxU32 i=0; i<ms->mAnimationCount; i++)
		{
			const MeshAnimation &a = *ms->mAnimations[i];
			serialize(stream,a);
		}
		stream << ms->mMaterialCount; 
		for (physx::PxU32 i=0; i<ms->mMaterialCount; i++)
		{
			const MeshMaterial &m = ms->mMaterials[i];
			serialize(stream,m);
		}
		stream << ms->mUserDataCount;
		for (physx::PxU32 i=0; i<ms->mUserDataCount; i++)
		{
			const MeshUserData &m = *ms->mUserData[i];
			serialize(stream,m);
		}
		stream << ms->mUserBinaryDataCount;
		for (physx::PxU32 i=0; i<ms->mUserBinaryDataCount; i++)
		{
			const MeshUserBinaryData &m = *ms->mUserBinaryData[i];
			serialize(stream,m);
		}
		stream << ms->mMeshCount;
		for (physx::PxU32 i=0; i<ms->mMeshCount; i++)
		{
			const Mesh &m = *ms->mMeshes[i];
			serialize(stream,m);
		}
		stream << ms->mMeshInstanceCount;
		for (physx::PxU32 i=0; i<ms->mMeshInstanceCount; i++)
		{
			const MeshInstance &m = ms->mMeshInstances[i];
			serialize(stream,m);
		}
		stream << ms->mMeshCollisionCount;
		for (physx::PxU32 i=0; i<ms->mMeshCollisionCount; i++)
		{
			const MeshCollisionRepresentation &m = *ms->mMeshCollisionRepresentations[i];
			serialize(stream,m);
		}
		stream << ms->mPlane.n.x;
		stream << ms->mPlane.n.y;
		stream << ms->mPlane.n.z;
		stream << ms->mPlane.d;

	}
}


bool exportEZB(const char *fname,const MeshSystem *ms)
{
	bool ret = false;

	FILE *fph = fopen(fname,"wb");
	if ( fph )
	{
		ret = true;
		serializeEZB(fph,ms);
		fclose(fph);
	}
	return ret;
};


};

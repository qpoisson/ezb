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

#ifndef EZMESH_H
#define EZMESH_H

#include "foundation/PxVec3.h"

namespace mimp
{

	enum MeshVertexFlag
	{
		MIVF_POSITION       = (1<<0),
		MIVF_NORMAL         = (1<<1),
		MIVF_COLOR          = (1<<2),
		MIVF_TEXEL1         = (1<<3),
		MIVF_TEXEL2         = (1<<4),
		MIVF_TEXEL3         = (1<<5),
		MIVF_TEXEL4         = (1<<6),
		MIVF_TANGENT        = (1<<7),
		MIVF_BINORMAL       = (1<<8),
		MIVF_BONE_WEIGHTING = (1<<9),
		MIVF_RADIUS         = (1<<10),
		MIVF_INTERP1        = (1<<11),
		MIVF_INTERP2        = (1<<12),
		MIVF_INTERP3        = (1<<13),
		MIVF_INTERP4        = (1<<14),
		MIVF_INTERP5        = (1<<15),
		MIVF_INTERP6        = (1<<16),
		MIVF_INTERP7        = (1<<17),
		MIVF_INTERP8        = (1<<18),
		MIVF_ALL = (MIVF_POSITION | MIVF_NORMAL | MIVF_COLOR | MIVF_TEXEL1 | MIVF_TEXEL2 | MIVF_TEXEL3 | MIVF_TEXEL4 | MIVF_TANGENT | MIVF_BINORMAL | MIVF_BONE_WEIGHTING )
	};

	class MeshVertex 
	{
	public:
		MeshVertex(void)
		{
			mPos[0] = mPos[1] = mPos[2] = 0;
			mNormal[0] = 0; mNormal[1] = 1; mNormal[2] = 0;
			mColor = 0xFFFFFFFF;
			mTexel1[0] = mTexel1[1] = 0;
			mTexel2[0] = mTexel2[1] = 0;
			mTexel3[0] = mTexel3[1] = 0;
			mTexel4[0] = mTexel4[1] = 0;

			mInterp1[0] = mInterp1[1] = mInterp1[2] = mInterp1[3] =0;
			mInterp2[0] = mInterp2[1] = mInterp2[2] = mInterp2[3] =0;
			mInterp3[0] = mInterp3[1] = mInterp3[2] = mInterp3[3] =0;
			mInterp4[0] = mInterp4[1] = mInterp4[2] = mInterp4[3] =0;
			mInterp5[0] = mInterp5[1] = mInterp5[2] = mInterp5[3] =0;
			mInterp6[0] = mInterp6[1] = mInterp6[2] = mInterp6[3] =0;
			mInterp7[0] = mInterp7[1] = mInterp7[2] = mInterp7[3] =0;
			mInterp8[0] = mInterp8[1] = mInterp8[2] = mInterp8[3] =0;

			mTangent[0] = mTangent[1] = mTangent[2] = 0;
			mBiNormal[0] = mBiNormal[1] = mBiNormal[2] = 0;
			mWeight[0] = 1; mWeight[1] = 0; mWeight[2] = 0; mWeight[3] = 0;
			mBone[0] = mBone[1] = mBone[2] = mBone[3] = 0;
			mRadius = 0; // use for cloth simulations
		}

		bool  operator==(const MeshVertex &v) const
		{
			bool ret = false;

			if ( memcmp(this,&v,sizeof(MeshVertex)) == 0 ) ret = true;

			return ret;
		}

		physx::PxF32		mPos[3];
		physx::PxF32		mNormal[3];
		physx::PxU32		mColor;
		physx::PxF32		mTexel1[2];
		physx::PxF32		mTexel2[2];
		physx::PxF32		mTexel3[2];
		physx::PxF32		mTexel4[2];
		physx::PxF32		mInterp1[4];
		physx::PxF32		mInterp2[4];
		physx::PxF32		mInterp3[4];
		physx::PxF32		mInterp4[4];
		physx::PxF32		mInterp5[4];
		physx::PxF32		mInterp6[4];
		physx::PxF32		mInterp7[4];
		physx::PxF32		mInterp8[4];
		physx::PxF32		mTangent[3];
		physx::PxF32		mBiNormal[3];
		physx::PxF32		mWeight[4];
		unsigned short		mBone[4];
		physx::PxF32		mRadius;
	};

	class MeshBone
	{
	public:
		MeshBone(void)
		{
			mParentIndex = -1;
			mName = "";
			Identity();
		}

		void Set(const char *name,physx::PxI32 parentIndex,const physx::PxF32 pos[3],const physx::PxF32 rot[4],const physx::PxF32 scale[3])
		{
			mName = name;
			mParentIndex = parentIndex;
			mPosition[0] = pos[0];
			mPosition[1] = pos[1];
			mPosition[2] = pos[2];
			mOrientation[0] = rot[0];
			mOrientation[1] = rot[1];
			mOrientation[2] = rot[2];
			mOrientation[3] = rot[3];
			mScale[0] = scale[0];
			mScale[1] = scale[1];
			mScale[2] = scale[2];
		}

		void Identity(void)
		{
			mPosition[0] = 0;
			mPosition[1] = 0;
			mPosition[2] = 0;

			mOrientation[0] = 0;
			mOrientation[1] = 0;
			mOrientation[2] = 0;
			mOrientation[3] = 1;

			mScale[0] = 1;
			mScale[1] = 1;
			mScale[2] = 1;

		}

		void SetName(const char *name)
		{
			mName = name;
		}

		const char * GetName(void) const { return mName; };

		physx::PxI32 GetParentIndex(void) const { return mParentIndex; };

		const physx::PxF32 * GetPosition(void) const { return mPosition; };
		const physx::PxF32 * GetOrientation(void) const { return mOrientation; };
		const physx::PxF32 * GetScale(void) const { return mScale; };

		const char			*mName;
		physx::PxI32		mParentIndex;          // array index of parent bone
		physx::PxF32		mPosition[3];
		physx::PxF32		mOrientation[4];
		physx::PxF32		mScale[3];
	};

	class MeshEntry
	{
	public:
		MeshEntry(void)
		{
			mName = "";
			mBone = 0;
		}
		const char *mName;
		physx::PxI32         mBone;         // bone this mesh is associcated
	};

	class MeshSkeleton
	{
	public:
		MeshSkeleton(void)
		{
			mName = "";
			mBoneCount = 0;
			mBones = 0;
		}

		void SetName(const char *name)
		{
			mName = name;
		}

		void SetBones(physx::PxI32 bcount,MeshBone *bones) // memory ownership changes hands here!!!!!!!!!!
		{
			mBoneCount = bcount;
			mBones     = bones;
		}

		physx::PxI32 GetBoneCount(void) const { return mBoneCount; };

		const MeshBone& GetBone(physx::PxI32 index) const { return mBones[index]; };

		MeshBone * GetBonePtr(physx::PxI32 index) const { return &mBones[index]; };

		void SetBone(physx::PxI32 index,const MeshBone &b) { mBones[index] = b; };

		const char * GetName(void) const { return mName; };

		const char     *mName;
		physx::PxI32             mBoneCount;
		MeshBone       *mBones;
	};


	class MeshAnimPose
	{
	public:
		MeshAnimPose(void)
		{
			mPos[0] = 0;
			mPos[1] = 0;
			mPos[2] = 0;
			mQuat[0] = 0;
			mQuat[1] = 0;
			mQuat[2] = 0;
			mQuat[3] = 0;
			mScale[0] = 1;
			mScale[1] = 1;
			mScale[2] = 1;
		}

		void SetPose(const physx::PxF32 *pos,const physx::PxF32 *quat,const physx::PxF32 *scale)
		{
			mPos[0] = pos[0];
			mPos[1] = pos[1];
			mPos[2] = pos[2];
			mQuat[0] = quat[0];
			mQuat[1] = quat[1];
			mQuat[2] = quat[2];
			mQuat[3] = quat[3];
			mScale[0] = scale[0];
			mScale[1] = scale[1];
			mScale[2] = scale[2];
		};

		void Sample(physx::PxF32 *pos,physx::PxF32 *quat,physx::PxF32 *scale) const
		{
			pos[0] = mPos[0];
			pos[1] = mPos[1];
			pos[2] = mPos[2];
			quat[0] = mQuat[0];
			quat[1] = mQuat[1];
			quat[2] = mQuat[2];
			quat[3] = mQuat[3];
			scale[0] = mScale[0];
			scale[1] = mScale[1];
			scale[2] = mScale[2];
		}

		physx::PxF32 mPos[3];
		physx::PxF32 mQuat[4];
		physx::PxF32 mScale[3];
	};

	class MeshAnimTrack
	{
	public:
		MeshAnimTrack(void)
		{
			mName = "";
			mFrameCount = 0;
			mDuration = 0;
			mDtime = 0;
			mPose = 0;
		}

		void SetName(const char *name)
		{
			mName = name;
		}

		void SetPose(physx::PxI32 frame,const physx::PxF32 *pos,const physx::PxF32 *quat,const physx::PxF32 *scale)
		{
			if ( frame >= 0 && frame < mFrameCount )
				mPose[frame].SetPose(pos,quat,scale);
		}

		const char * GetName(void) const { return mName; };

		void SampleAnimation(physx::PxI32 frame,physx::PxF32 *pos,physx::PxF32 *quat,physx::PxF32 *scale) const
		{
			mPose[frame].Sample(pos,quat,scale);
		}

		physx::PxI32 GetFrameCount(void) const { return mFrameCount; };

		MeshAnimPose * GetPose(physx::PxI32 index) { return &mPose[index]; };

		const char *mName;
		physx::PxI32       mFrameCount;
		physx::PxF32     mDuration;
		physx::PxF32     mDtime;
		MeshAnimPose *mPose;
	};

	class MeshAnimation
	{
	public:
		MeshAnimation(void)
		{
			mName = "";
			mTrackCount = 0;
			mFrameCount = 0;
			mDuration = 0;
			mDtime = 0;
			mTracks = 0;
		}


		void SetName(const char *name)
		{
			mName = name;
		}

		void SetTrackName(physx::PxI32 track,const char *name)
		{
			mTracks[track]->SetName(name);
		}

		void SetTrackPose(physx::PxI32 track,physx::PxI32 frame,const physx::PxF32 *pos,const physx::PxF32 *quat,const physx::PxF32 *scale)
		{
			mTracks[track]->SetPose(frame,pos,quat,scale);
		}

		const char * GetName(void) const { return mName; };

		const MeshAnimTrack * LocateTrack(const char *name) const
		{
			const MeshAnimTrack *ret = 0;
			for (physx::PxI32 i=0; i<mTrackCount; i++)
			{
				const MeshAnimTrack *t = mTracks[i];
				if ( strcmp(t->GetName(),name) == 0 )
				{
					ret = t;
					break;
				}
			}
			return ret;
		}

		physx::PxI32 GetFrameIndex(physx::PxF32 t) const
		{
			t = fmodf( t, mDuration );
			physx::PxI32 index = physx::PxI32(t / mDtime);
			return index;
		}

		physx::PxI32 GetTrackCount(void) const { return mTrackCount; };
		physx::PxF32 GetDuration(void) const { return mDuration; };

		MeshAnimTrack * GetTrack(physx::PxI32 index)
		{
			MeshAnimTrack *ret = 0;
			if ( index >= 0 && index < mTrackCount )
			{
				ret = mTracks[index];
			}
			return ret;
		};

		physx::PxI32 GetFrameCount(void) const { return mFrameCount; };
		physx::PxF32 GetDtime(void) const { return mDtime; };

		const char *mName;
		physx::PxI32         mTrackCount;
		physx::PxI32         mFrameCount;
		physx::PxF32       mDuration;
		physx::PxF32       mDtime;
		MeshAnimTrack **mTracks;
	};



	class MeshMaterial
	{
	public:
		MeshMaterial(void)
		{
			mName = 0;
			mMetaData = 0;
		}
		const char *mName;
		const char *mMetaData;
	};

	class MeshAABB
	{
	public:
		MeshAABB(void)
		{
			mMin[0] = FLT_MAX;
			mMin[1] = FLT_MAX;
			mMin[2] = FLT_MAX;
			mMax[0] = FLT_MIN;
			mMax[1] = FLT_MIN;
			mMax[2] = FLT_MIN;
		}

		void include(const physx::PxF32 pos[3])
		{
			if ( pos[0] < mMin[0] ) mMin[0] = pos[0];
			if ( pos[1] < mMin[1] ) mMin[1] = pos[1];
			if ( pos[2] < mMin[2] ) mMin[2] = pos[2];
			if ( pos[0] > mMax[0] ) mMax[0] = pos[0];
			if ( pos[1] > mMax[1] ) mMax[1] = pos[1];
			if ( pos[2] > mMax[2] ) mMax[2] = pos[2];
		}
		physx::PxF32 mMin[3];
		physx::PxF32 mMax[3];
	};

	class SubMesh
	{
	public:
		SubMesh(void)
		{
			mMaterialName = 0;
			mMaterial     = 0;
			mVertexFlags  = 0;
			mTriCount     = 0;
			mIndices      = 0;
		}

		const char          *mMaterialName;
		MeshMaterial        *mMaterial;
		MeshAABB             mAABB;
		physx::PxU32         mVertexFlags; // defines which vertex components are active.
		physx::PxU32         mTriCount;    // number of triangles.
		physx::PxU32        *mIndices;     // indexed triange list
	};

	class Mesh
	{
	public:
		Mesh(void)
		{
			mName         = 0;
			mSkeletonName = 0;
			mSkeleton     = 0;
			mSubMeshCount = 0;
			mSubMeshes    = 0;
			mVertexFlags  = 0;
			mVertexCount  = 0;
			mVertices     = 0;
		}
		const char         *mName;
		const char         *mSkeletonName;
		MeshSkeleton       *mSkeleton; // the skeleton used by this mesh system.
		MeshAABB            mAABB;
		physx::PxU32        mSubMeshCount;
		SubMesh           **mSubMeshes;

		physx::PxU32       mVertexFlags;  // combined vertex usage flags for all sub-meshes
		physx::PxU32       mVertexCount;
		MeshVertex        *mVertices;

	};

	class MeshRawTexture
	{
	public:
		MeshRawTexture(void)
		{
			mName = 0;
			mData = 0;
			mWidth = 0;
			mHeight = 0;
			mBPP = 0;
		}
		const char    *mName;
		physx::PxU8 *mData;
		physx::PxU32   mWidth;
		physx::PxU32   mHeight;
		physx::PxU32        mBPP;
	};

	class MeshInstance
	{
	public:
		MeshInstance(void)
		{
			mMeshName = 0;
			mMesh     = 0;
			mPosition[0] = mPosition[1] = mPosition[2] = 0;
			mRotation[0] = mRotation[1] = mRotation[2] = mRotation[3] = 0;
			mScale[0] = mScale[1] = mScale[2] = 0;
		}
		const char  *mMeshName;
		Mesh        *mMesh;
		physx::PxF32        mPosition[3];
		physx::PxF32        mRotation[4]; //quaternion XYZW
		physx::PxF32        mScale[3];
	};

	class MeshUserData
	{
	public:
		MeshUserData(void)
		{
			mUserKey = 0;
			mUserValue = 0;
		}
		const char *mUserKey;
		const char *mUserValue;
	};

	class MeshUserBinaryData
	{
	public:
		MeshUserBinaryData(void)
		{
			mName     = 0;
			mUserData = 0;
			mUserLen  = 0;
		}
		const char    *mName;
		physx::PxU32   mUserLen;
		physx::PxU8 *mUserData;
	};

	class MeshTetra
	{
	public:
		MeshTetra(void)
		{
			mTetraName  = 0;
			mMeshName   = 0;  // mesh the tetraheadral mesh is associated with.
			mMesh       = 0;
			mTetraCount = 0;
			mTetraData  = 0;
		}

		const char  *mTetraName;
		const char  *mMeshName;
		MeshAABB     mAABB;
		Mesh        *mMesh;
		physx::PxU32 mTetraCount; // number of tetrahedrons
		physx::PxF32       *mTetraData;
	};

#define MESH_SYSTEM_VERSION 1 // version number of this data structure, used for binary serialization

	enum MeshCollisionType
	{
		MCT_BOX,
		MCT_SPHERE,
		MCT_CAPSULE,
		MCT_CONVEX,
		MCT_LAST
	};

	class MeshCollision
	{
	public:
		MeshCollision(void)
		{
			mType = MCT_LAST;
			mName = NULL;
			mLocalPosition[0] = 0;
			mLocalPosition[1] = 0;
			mLocalPosition[2] = 0;
			mLocalOrientation[0] = 0;
			mLocalOrientation[1] = 0;
			mLocalOrientation[2] = 0;
			mLocalOrientation[3] = 1;
		}

		MeshCollisionType getType(void) const { return mType; };

		MeshCollisionType    mType;
		const char            *mName;  // the bone this collision geometry is associated with.
		physx::PxF32                mLocalPosition[3];
		physx::PxF32                mLocalOrientation[4];
	};

	class MeshCollisionBox : public MeshCollision 
	{
	public:
		MeshCollisionBox(void)
		{
			mType = MCT_BOX;
			mSides[0] = mSides[1] = mSides[2] = 1;
		}
		physx::PxF32 mSides[3];
	};

	class MeshCollisionSphere : public MeshCollision
	{
	public:
		MeshCollisionSphere(void)
		{
			mType = MCT_SPHERE;
			mRadius = 1;
		}
		physx::PxF32 mRadius;
	};

	class MeshCollisionCapsule : public MeshCollision
	{
	public:
		MeshCollisionCapsule(void)
		{
			mType = MCT_CAPSULE;
			mRadius = 1;
			mHeight = 1;
		}
		physx::PxF32  mRadius;
		physx::PxF32  mHeight;
	};

	class MeshConvex
	{
	public:
		MeshConvex(void)
		{
			mVertexCount = 0;
			mVertices = 0;
			mTriCount = 0;
			mIndices = 0;
		}
		physx::PxU32    mVertexCount;
		physx::PxF32    *mVertices;
		physx::PxU32    mTriCount;
		physx::PxU32    *mIndices;
	};

	class MeshCollisionConvex : public MeshCollision, public MeshConvex
	{
	public:
		MeshCollisionConvex(void)
		{
			mType = MCT_CONVEX;
		}


	};

	class MeshCollisionRepresentation
	{
	public:
		MeshCollisionRepresentation(void)
		{
			mName = NULL;
			mInfo = NULL;
			mCollisionCount = 0;
			mCollisionGeometry = NULL;
			mPosition[0] = 0;
			mPosition[1] = 0;
			mPosition[2] = 0;
			mOrientation[0] = 0;
			mOrientation[1] = 0;
			mOrientation[2] = 0;
			mOrientation[3] = 1;
			mSolverCount = 4;
			mAwake = true;
		}
		const char        *mName;
		const char        *mInfo;
		physx::PxU32            mCollisionCount;
		MeshCollision    **mCollisionGeometry;
		physx::PxF32            mPosition[3];
		physx::PxF32            mOrientation[4];
		physx::PxU32            mSolverCount;
		bool            mAwake;
	};

	class MeshSimpleJoint
	{
	public:
		MeshSimpleJoint(void)
		{
			mName = "";
			mParent = "";
			mParent = "";
			mTwistLow = 0;
			mTwistHigh = 0;
			mSwing1 = 0;
			mSwing2 = 0;
			mOffset[0] = 0;
			mOffset[1] = 0;
			mOffset[2] = 0;
			mXaxis[0] = 1;
			mXaxis[1] = 0;
			mXaxis[2] = 0;
			mZaxis[0] = 0;
			mZaxis[1] = 0;
			mZaxis[2] = 1;
		}
		const char *mName;            // name of joint; defines which mesh collision representation it is connected to.
		const char *mParent;        // name of parent, defines which mesh collision representation this joint connects to.
		physx::PxF32        mOffset[3];        // The offset of the joint from the root bone.
		physx::PxF32        mXaxis[3];        // The axis of the joint from the root bone
		physx::PxF32        mZaxis[3];        // The normal of the joint relative to the root bone.
		physx::PxF32        mTwistLow;        // The low twist limit in radians
		physx::PxF32        mTwistHigh;        // The high twist limit in radians
		physx::PxF32        mSwing1;        // The swing1 limit
		physx::PxF32        mSwing2;        // The swing2 limit
	};

	class MeshPairCollision
	{
	public:
		const char *mBody1;
		const char *mBody2;
	};

	class MeshSystem
	{
	public:
		MeshSystem(void)
		{
			mAssetName           = 0;
			mAssetInfo           = 0;
			mTextureCount        = 0;
			mTextures            = 0;
			mSkeletonCount       = 0;
			mSkeletons           = 0;
			mAnimationCount      = 0;
			mAnimations          = 0;
			mMaterialCount       = 0;
			mMaterials           = 0;
			mMeshCount           = 0;
			mMeshes              = 0;
			mMeshInstanceCount   = 0;
			mMeshInstances       = 0;
			mUserDataCount       = 0;
			mUserData            = 0;
			mUserBinaryDataCount = 0;
			mUserBinaryData      = 0;
			mTetraMeshCount      = 0;
			mTetraMeshes         = 0;
			mMeshSystemVersion   = MESH_SYSTEM_VERSION;
			mAssetVersion        = 0;
			mMeshCollisionCount  = 0;
			mMeshCollisionRepresentations = 0;
			mMeshJointCount        = 0;
			mMeshJoints            = NULL;
			mMeshPairCollisionCount = 0;
			mMeshPairCollisions = NULL;

			mPlane[0] = 1;
			mPlane[1] = 0;
			mPlane[2] = 0;
			mPlane[3] = 0;
		}


		const char			*mAssetName;
		const char			*mAssetInfo;
		physx::PxI32		mMeshSystemVersion;
		physx::PxI32		mAssetVersion;
		MeshAABB			mAABB;
		physx::PxU32		mTextureCount;          // Are textures necessary? [rgd].
		MeshRawTexture		**mTextures;              // Texture storage in mesh data is rare, and the name is simply an attribute of the material

		physx::PxU32		mTetraMeshCount;        // number of tetrahedral meshes
		MeshTetra			**mTetraMeshes;           // tetraheadral meshes

		physx::PxU32		mSkeletonCount;         // number of skeletons
		MeshSkeleton		**mSkeletons;             // the skeletons.

		physx::PxU32		mAnimationCount;
		MeshAnimation		**mAnimations;

		physx::PxU32		mMaterialCount;         // Materials are owned by this list, merely referenced later.
		MeshMaterial		*mMaterials;

		physx::PxU32		mUserDataCount;
		MeshUserData		**mUserData;

		physx::PxU32		mUserBinaryDataCount;
		MeshUserBinaryData	**mUserBinaryData;

		physx::PxU32		mMeshCount;
		Mesh				**mMeshes;

		physx::PxU32		mMeshInstanceCount;
		MeshInstance		*mMeshInstances;

		physx::PxU32		mMeshCollisionCount;
		MeshCollisionRepresentation **mMeshCollisionRepresentations;

		physx::PxU32		mMeshJointCount;
		MeshSimpleJoint		*mMeshJoints;

		physx::PxU32		mMeshPairCollisionCount;
		MeshPairCollision	*mMeshPairCollisions;

		physx::PxF32		mPlane[4];

	};




}; // End of namespace for physx

#endif

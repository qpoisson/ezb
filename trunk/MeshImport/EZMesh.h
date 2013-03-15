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

#include "foundation/PxVec2.h"
#include "foundation/PxVec3.h"
#include "foundation/PxVec4.h"
#include "foundation/PxQuat.h"
#include "foundation/PxPlane.h"
#include "foundation/PxBounds3.h"

#define MESH_BINARY_VERSION 101	// version number of the binary file format

namespace ezmesh
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

	// A very fat vertex.  Used only for transferring data into an application.
	// Ideally you should translate the enabled vertex interpolants into your own internal stream format.
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

		physx::PxVec3		mPos;			// The vertex position
		physx::PxVec3		mNormal;		// The vetex normal
		physx::PxU32		mColor;			// The vertex color as ARGB
		physx::PxVec2		mTexel1;		// First texture co-ordinate
		physx::PxVec2		mTexel2;		// Second texture co-ordinate
		physx::PxVec2		mTexel3;		// Third texture co-ordinate
		physx::PxVec2		mTexel4;		// Fourth texture co-ordinate
		physx::PxVec4		mInterp1;		// Up to 8 arbitrary float4 interpolants
		physx::PxVec4		mInterp2;
		physx::PxVec4		mInterp3;
		physx::PxVec4		mInterp4;
		physx::PxVec4		mInterp5;
		physx::PxVec4		mInterp6;
		physx::PxVec4		mInterp7;
		physx::PxVec4		mInterp8;
		physx::PxVec3		mTangent;		// The tangent vector
		physx::PxVec3		mBiNormal;		// The binaormal
		physx::PxVec4		mWeight;		// Up to 4 floating point weight values; must add up to exactly 1.0
		unsigned short		mBone[4];		// Up to 4 bone indices into a skeleton
		physx::PxF32		mRadius;		// An optional radius of influence for this vertex
	};

	// A single bone in a skeleton
	class MeshBone
	{
	public:
		MeshBone(void)
		{
			mParentIndex = -1;
			mName = "";
			Identity();
		}

		void Set(const char *name,physx::PxI32 parentIndex,const physx::PxVec3 &pos,const physx::PxQuat &rot,const physx::PxVec3 &scale)
		{
			mName = name;
			mParentIndex = parentIndex;
			mPosition = pos;
			mOrientation = rot;
			mScale = scale;
		}

		void Identity(void)
		{
			mPosition[0] = 0;
			mPosition[1] = 0;
			mPosition[2] = 0;

			mOrientation = physx::PxQuat::createIdentity();

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

		const physx::PxVec3& GetPosition(void) const { return mPosition; };
		const physx::PxQuat& GetOrientation(void) const { return mOrientation; };
		const physx::PxVec3&  GetScale(void) const { return mScale; };

		const char			*mName;					// The name of the bone
		physx::PxI32		mParentIndex;			// array index of parent bone; -1 means there is no parent
		physx::PxVec3		mPosition;				// The bone position in skeleton space
		physx::PxQuat		mOrientation;			// The orientation in parent relative skeleton space
		physx::PxVec3		mScale;					// The uniform scale in parant relative skeleton space
	};

	// An association between a mesh and a particular bone in a skeleton.
	class MeshEntry
	{
	public:
		MeshEntry(void)
		{
			mName = "";
			mBone = 0;
		}
		const char		*mName;
		physx::PxI32	mBone;         // bone this mesh is associated
	};

	// A completely specified skeleton with child/parent relationships.
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

		const char		*mName;		// The name of the skeleton
		physx::PxI32	mBoneCount; // The number of bones in the skeleton
		MeshBone		*mBones;	// The individual bones
	};

	// A single animation sample in skeleton space
	class MeshAnimPose
	{
	public:
		MeshAnimPose(void)
		{
			mPos = physx::PxVec3(0,0,0);
			mQuat = physx::PxQuat::createIdentity();
			mScale = physx::PxVec3(1,1,1);
		}

		void SetPose(const physx::PxVec3 &pos,const physx::PxQuat &quat,const physx::PxVec3 &scale)
		{
			mPos = pos;
			mQuat = quat;
			mScale = scale;
		};

		void Sample(physx::PxVec3 &pos,physx::PxQuat &quat,physx::PxVec3 &scale) const
		{
			pos = mPos;
			quat = mQuat;
			scale = mScale;
		}

		physx::PxVec3 mPos;
		physx::PxQuat mQuat;
		physx::PxVec3 mScale;
	};

	// An array of animation samples for a particular track (bone)
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

		void SetPose(physx::PxI32 frame,const physx::PxVec3 &pos,const physx::PxQuat &quat,const physx::PxVec3 &scale)
		{
			if ( frame >= 0 && frame < mFrameCount )
				mPose[frame].SetPose(pos,quat,scale);
		}

		const char * GetName(void) const { return mName; };

		void SampleAnimation(physx::PxI32 frame,physx::PxVec3 &pos,physx::PxQuat &quat,physx::PxVec3 &scale) const
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

	// A full set of sampled animation data
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

		void SetTrackPose(physx::PxI32 track,physx::PxI32 frame,const physx::PxVec3 &pos,const physx::PxQuat &quat,const physx::PxVec3 &scale)
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

		const char		*mName;
		physx::PxI32	mTrackCount;
		physx::PxI32	mFrameCount;
		physx::PxF32	mDuration;
		physx::PxF32	mDtime;
		MeshAnimTrack **mTracks;
	};


	// A material associated with a sub-mesh
	class MeshMaterial
	{
	public:
		MeshMaterial(void)
		{
			mName = 0;
			mMetaData = 0;
		}
		const char *mName;				// The name of the material
		const char *mMetaData;			// Optional application associated material meta-data
	};

	// A single sub-mesh
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

		const char			*mMaterialName;	// The name of the material used by this submesh
		MeshMaterial		*mMaterial;		// The pointer to the mesh material; presuming the named lookup has been performed.
		physx::PxBounds3	mAABB;			// The bounding volume for this submesh
		physx::PxU32		mVertexFlags;	// defines which vertex components are active.
		physx::PxU32		mTriCount;		// number of triangles.
		physx::PxU32		*mIndices;		// indexed triangle list
	};

	// A single mesh
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
		const char         *mName;				// The name of the mesh
		const char         *mSkeletonName;		// The name of the skeleton used by this mesh
		MeshSkeleton       *mSkeleton;			// pointer to skeleton if the named lookup has been performed.
		physx::PxBounds3	mAABB;				// Bounding volume for this mesh
		physx::PxU32        mSubMeshCount;		// The number of submeshes in this mesh
		SubMesh           **mSubMeshes;			// Pointer to the submeshes
		physx::PxU32       mVertexFlags;		// combined vertex usage flags for all sub-meshes
		physx::PxU32       mVertexCount;		// The number of unique vertices in the vetex buffer
		MeshVertex        *mVertices;			// The full fat vetices

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
		const char		*mName;
		physx::PxU8		*mData;
		physx::PxU32	mWidth;
		physx::PxU32	mHeight;
		physx::PxU32	mBPP;
	};

	class MeshInstance
	{
	public:
		MeshInstance(void)
		{
			mMeshName = 0;
			mMesh     = 0;
			mPosition[0] = mPosition[1] = mPosition[2] = 0;
			mRotation = physx::PxQuat::createIdentity();
			mScale[0] = mScale[1] = mScale[2] = 1;
		}
		const char  *mMeshName;
		Mesh        *mMesh;
		physx::PxVec3	mPosition;
		physx::PxQuat	mRotation; //quaternion XYZW
		physx::PxVec3	mScale;
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

		const char			*mTetraName;
		const char			*mMeshName;
		physx::PxBounds3	mAABB;
		Mesh				*mMesh;
		physx::PxU32		mTetraCount; // number of tetrahedrons
		physx::PxF32		*mTetraData;
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
			mLocalOrientation = physx::PxQuat::createIdentity();
		}

		MeshCollisionType getType(void) const { return mType; };

		MeshCollisionType	mType;
		const char			*mName;  // the bone this collision geometry is associated with.
		physx::PxVec3		mLocalPosition;
		physx::PxQuat		mLocalOrientation;
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
			mOrientation = physx::PxQuat::createIdentity();
			mSolverCount = 4;
			mAwake = true;
		}
		const char		*mName;
		const char		*mInfo;
		physx::PxU32	mCollisionCount;
		MeshCollision	**mCollisionGeometry;
		physx::PxVec3	mPosition;
		physx::PxQuat	mOrientation;
		physx::PxU32	mSolverCount;
		bool			mAwake;
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
		const char			*mName;            // name of joint; defines which mesh collision representation it is connected to.
		const char			*mParent;        // name of parent, defines which mesh collision representation this joint connects to.
		physx::PxVec3		mOffset;        // The offset of the joint from the root bone.
		physx::PxVec3		mXaxis;        // The axis of the joint from the root bone
		physx::PxVec3		mZaxis;        // The normal of the joint relative to the root bone.
		physx::PxF32		mTwistLow;        // The low twist limit in radians
		physx::PxF32		mTwistHigh;        // The high twist limit in radians
		physx::PxF32		mSwing1;        // The swing1 limit
		physx::PxF32		mSwing2;        // The swing2 limit
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

			mPlane.n = physx::PxVec3(1,0,0);
			mPlane.d = 0;
		}


		const char			*mAssetName;			// The name of this MeshSystem asset
		const char			*mAssetInfo;			// Optional additional user tagged information associated with this asset.
		physx::PxI32		mMeshSystemVersion;		// The version number of this mesh system.
		physx::PxI32		mAssetVersion;			// The asset version number.
		physx::PxBounds3	mAABB;					// The axis aligned bounding volume of this asset.
		physx::PxU32		mTextureCount;			// The number of texture ins this asset.
		MeshRawTexture		**mTextures;			// Texture storage in mesh data is rare, and the name is simply an attribute of the material

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

		physx::PxPlane		mPlane;

	};




}; // End of namespace for physx

#endif

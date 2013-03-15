#include "MeshExportEZM.h"
#include "EZMesh.h"
#include "foundation/PxIntrinsics.h"
#include "foundation/PxString.h"

#include <stdio.h>

#pragma warning(disable:4996)

namespace mimp
{

static const char *cleanAttribute(const char *attribute)
{
	static char scratch[1024];
	char *end = &scratch[1020];
	char *dest = scratch;
	while ( attribute && *attribute && dest < end )
	{
		if ( *attribute == 34 )
		{
			dest[0]= '%';
			dest[1] = '2';
			dest[2] = '0';
			dest+=3;
			attribute++;
		}
		else
		{
			*dest++ = *attribute++;
		}
	}
	*dest = 0;
	return scratch;
}



static const char * FloatString(physx::PxF32 v,bool binary=false)
{
	static char data[64*16];
	static physx::PxI32  index=0;

	char *ret = &data[index*64];
	index++;
	if (index == 16 ) index = 0;

	if ( !physx::intrinsics::isFinite(v) )
  {
	PX_ALWAYS_ASSERT();
	strcpy(ret,"0"); // not a valid number!
  }
	else if ( v == 1 )
	{
		strcpy(ret,"1");
	}
	else if ( v == 0 )
	{
		strcpy(ret,"0");
	}
	else if ( v == - 1 )
	{
		strcpy(ret,"-1");
	}
	else
	{
		if ( binary )
		{
			physx::PxU32 *iv = (physx::PxU32 *) &v;
			sprintf_s(ret,64,"%.4f$%x", v, *iv );
		}
		else
		{
			sprintf_s(ret,64,"%.9f", v );
			const char *dot = strstr(ret,".");
			if ( dot )
			{
				physx::PxI32 len = (physx::PxI32)strlen(ret);
				char *foo = &ret[len-1];
				while ( *foo == '0' ) foo--;
				if ( *foo == '.' )
					*foo = 0;
				else
					foo[1] = 0;
			}
		}
	}

	return ret;
}

static const char *getStr(const char *str)
{
	if ( str == 0 ) str = "";
	return str;
}

static void printAABB(FILE * /*fph*/,const MeshAABB & /*a*/)
{
	// fprintf(fph," <MeshAABB min=\"%s,%s,%s\" max=\"%s,%s,%s\"/>\r\n", FloatString(a.mMin[0]), FloatString(a.mMin[1]), FloatString(a.mMin[2]), FloatString(a.mMax[0]), FloatString(a.mMax[1]), FloatString(a.mMax[2]) );
}

static void print(FILE * /*fph*/,MeshRawTexture * /*t*/)
{
	PX_ASSERT(0); // not yet implemented
}

static void print(FILE * /*fph*/,MeshTetra * /*t*/)
{
	PX_ASSERT(0); // not yet implemented
}

static void print(FILE *fph,MeshBone &b,MeshSkeleton *s)
{
	const char *parent = 0;

	if ( b.mParentIndex >= 0 )
	{
		PX_ASSERT( b.mParentIndex >= 0 && b.mParentIndex < s->mBoneCount );
		if ( b.mParentIndex >= 0 && b.mParentIndex < s->mBoneCount )
		{
			parent = s->mBones[b.mParentIndex].mName;
		}
	}
	if ( parent )
	{
		fprintf(fph,"        <Bone name=\"%s\" parent=\"%s\" orientation=\"%s %s %s %s\" position=\"%s %s %s\" scale=\"%s %s %s\"/>\r\n",
			b.mName,
			parent,
			FloatString(b.mOrientation[0]),
			FloatString(b.mOrientation[1]),
			FloatString(b.mOrientation[2]),
			FloatString(b.mOrientation[3]),
			FloatString(b.mPosition[0]),
			FloatString(b.mPosition[1]),
			FloatString(b.mPosition[2]),
			FloatString(b.mScale[0]),
			FloatString(b.mScale[1]),
			FloatString(b.mScale[2]) );
	}
	else
	{
		fprintf(fph,"        <Bone name=\"%s\" orientation=\"%s %s %s %s\" position=\"%s %s %s\" scale=\"%s %s %s\"/>\r\n",
			b.mName,
			FloatString(b.mOrientation[0]),
			FloatString(b.mOrientation[1]),
			FloatString(b.mOrientation[2]),
			FloatString(b.mOrientation[3]),
			FloatString(b.mPosition[0]),
			FloatString(b.mPosition[1]),
			FloatString(b.mPosition[2]),
			FloatString(b.mScale[0]),
			FloatString(b.mScale[1]),
			FloatString(b.mScale[2]) );
	}
}

static void print(FILE *fph,MeshSkeleton *s)
{
	fprintf(fph,"      <Skeleton name=\"%s\" count=\"%d\">\r\n", s->mName, s->mBoneCount);
	for (physx::PxU32 i=0; i<(physx::PxU32)s->mBoneCount; i++)
	{
		print(fph,s->mBones[i],s);
	}
	fprintf(fph,"      </Skeleton>\r\n");
}

static void print(FILE *fph,const MeshAnimPose &p)
{
	fprintf(fph,"      %s %s %s   %s %s %s %s   %s %s %s,\r\n", 
		FloatString(p.mPos[0]),
		FloatString(p.mPos[1]),
		FloatString(p.mPos[2]),
		FloatString(p.mQuat[0]),
		FloatString(p.mQuat[1]),
		FloatString(p.mQuat[2]),
		FloatString(p.mQuat[3]),
		FloatString(p.mScale[0]),
		FloatString(p.mScale[1]),
		FloatString(p.mScale[2]) );
}

static void print(FILE *fph,MeshAnimTrack *track)
{
	fprintf(fph,"        <AnimTrack name=\"%s\" count=\"%d\" has_scale=\"true\">\r\n", track->mName, track->mFrameCount);
	for (physx::PxI32 i=0; i<track->mFrameCount; i++)
	{
		print(fph,track->mPose[i]);
	}
	fprintf(fph,"        </AnimTrack>\r\n");

}

static void print(FILE *fph,MeshAnimation *a)
{
	fprintf(fph,"      <Animation name=\"%s\" trackcount=\"%d\" framecount=\"%d\" duration=\"%s\" dtime=\"%s\">\r\n", a->mName, a->mTrackCount, a->mFrameCount, FloatString( a->mDuration ), FloatString( a->mDtime) );

	for (physx::PxI32 i=0; i<a->mTrackCount; i++)
	{
		print(fph,a->mTracks[i]);
	}

	fprintf(fph,"      </Animation>\r\n");

}

static void print(FILE *fph,const MeshMaterial &m)
{
	fprintf(fph,"      <Material name=\"%s\" meta_data=\"%s\"/>\r\n", m.mName, cleanAttribute(m.mMetaData) );
}

static void print(FILE * /*fph*/,MeshUserData * /*d*/)
{
}

static void print(FILE * /*fph*/,MeshUserBinaryData * /*d*/)
{
}

static const char * getCtype(physx::PxU32 flags)
{
	static char ctype[512];

	ctype[0] = 0;

	if ( flags & MIVF_POSITION ) { strcat(ctype,"fff "); };
	if ( flags & MIVF_NORMAL ) { strcat(ctype,"fff "); };
	if ( flags & MIVF_COLOR ) { strcat(ctype,"x4 "); };
	if ( flags & MIVF_TEXEL1 ) { strcat(ctype,"ff "); };
	if ( flags & MIVF_TEXEL2 ) { strcat(ctype,"ff "); };
	if ( flags & MIVF_TEXEL3 ) { strcat(ctype,"ff "); };
	if ( flags & MIVF_TEXEL4 ) { strcat(ctype,"ff "); };
	if ( flags & MIVF_TANGENT ) { strcat(ctype,"fff "); };
	if ( flags & MIVF_BINORMAL ) { strcat(ctype,"fff "); };
	if ( flags & MIVF_BONE_WEIGHTING ) { strcat(ctype,"ffff hhhh "); };
	if ( flags & MIVF_INTERP1 ) { strcat(ctype,"ffff "); };
	if ( flags & MIVF_INTERP2 ) { strcat(ctype,"ffff "); };
	if ( flags & MIVF_INTERP3 ) { strcat(ctype,"ffff "); };
	if ( flags & MIVF_INTERP4 ) { strcat(ctype,"ffff "); };
	if ( flags & MIVF_INTERP5 ) { strcat(ctype,"ffff "); };
	if ( flags & MIVF_INTERP6 ) { strcat(ctype,"ffff "); };
	if ( flags & MIVF_INTERP7 ) { strcat(ctype,"ffff "); };
	if ( flags & MIVF_INTERP8 ) { strcat(ctype,"ffff "); };
	if ( flags & MIVF_RADIUS ) strcat(ctype,"f ");

	if (  ctype[0] )
	{
		char *foo = (char *)ctype;
		physx::PxI32 len = (physx::PxI32)strlen(foo);
		if ( foo[len-1] == ' ' )
		{
			foo[len-1] = 0;
		}
	}

	return ctype;
}

static const char * getSemantics(physx::PxU32 flags)
{
	static char semantic[512];
	semantic[0] = 0;

	if ( flags & MIVF_POSITION ) { strcat(semantic,"position "); };
	if ( flags & MIVF_NORMAL ) { strcat(semantic,"normal "); };
	if ( flags & MIVF_COLOR ) { strcat(semantic,"color "); };
	if ( flags & MIVF_TEXEL1 ) { strcat(semantic,"texcoord1 "); };
	if ( flags & MIVF_TEXEL2 ) { strcat(semantic,"texcoord2 "); };
	if ( flags & MIVF_TEXEL3 ) { strcat(semantic,"texcoord3 "); };
	if ( flags & MIVF_TEXEL4 ) { strcat(semantic,"texcoord4 "); };
	if ( flags & MIVF_TANGENT ) { strcat(semantic,"tangent "); };
	if ( flags & MIVF_BINORMAL ) { strcat(semantic,"binormal "); };
	if ( flags & MIVF_BONE_WEIGHTING ) { strcat(semantic,"blendweights blendindices "); };
	if ( flags & MIVF_INTERP1 ) { strcat(semantic,"interp1 "); };
	if ( flags & MIVF_INTERP2 ) { strcat(semantic,"interp2 "); };
	if ( flags & MIVF_INTERP3 ) { strcat(semantic,"interp3 "); };
	if ( flags & MIVF_INTERP4 ) { strcat(semantic,"interp4 "); };
	if ( flags & MIVF_INTERP5 ) { strcat(semantic,"interp5 "); };
	if ( flags & MIVF_INTERP6 ) { strcat(semantic,"interp6 "); };
	if ( flags & MIVF_INTERP7 ) { strcat(semantic,"interp7 "); };
	if ( flags & MIVF_INTERP8 ) { strcat(semantic,"interp8 "); };
	if ( flags & MIVF_RADIUS ) strcat(semantic,"radius ");

	if ( semantic[0] )
	{
		char *foo = (char *)semantic;
		physx::PxI32 len = (physx::PxI32)strlen(foo);
		if ( foo[len-1] == ' ' )
		{
			foo[len-1] = 0;
		}
	}


	return semantic;
}

static void printVertex(FILE *fph,physx::PxU32 flags,const MeshVertex &v,physx::PxU32 &column,bool &newRow)
{
	if ( newRow )
	{
		if ( column != 0 )
		{
			fprintf(fph,"\r\n");
		}
		newRow = false;
		column = 0;
		fprintf(fph,"          ");
	}
	char scratch[1024] = { 0 };
	char temp[1024];

	if ( flags & MIVF_POSITION )
	{
		sprintf_s(temp,1024,"%s %s %s ", FloatString(v.mPos[0]), FloatString(v.mPos[1]), FloatString(v.mPos[2]) );
		strcat_s(scratch,1024,temp);
	}

	if ( flags & MIVF_NORMAL )
	{
		sprintf_s(temp,1024,"%s %s %s ", FloatString(v.mNormal[0]), FloatString(v.mNormal[1]), FloatString(v.mNormal[2]) );
		strcat_s(scratch,1024,temp);
	}

	if ( flags & MIVF_COLOR )
	{
		sprintf_s(temp,1024,"%08X ", v.mColor );
		strcat_s(scratch,1024,temp);
	}
	if ( flags & MIVF_TEXEL1 )
	{
		sprintf_s(temp,1024,"%s %s ", FloatString(v.mTexel1[0]), FloatString(v.mTexel1[1]) );
		strcat_s(scratch,1024,temp);
	}
	if ( flags & MIVF_TEXEL2 )
	{
		sprintf_s(temp,1024,"%s %s ", FloatString(v.mTexel2[0]), FloatString(v.mTexel2[1]) );
		strcat_s(scratch,1024,temp);
	}
	if ( flags & MIVF_TEXEL3 )
	{
		sprintf_s(temp,1024,"%s %s ", FloatString(v.mTexel3[0]), FloatString(v.mTexel3[1]) );
		strcat_s(scratch,1024,temp);
	}
	if ( flags & MIVF_TEXEL4 )
	{
		sprintf_s(temp,1024,"%s %s ", FloatString(v.mTexel4[0]), FloatString(v.mTexel4[1]) );
		strcat_s(scratch,1024,temp);
	}
	if ( flags & MIVF_TANGENT )
	{
		sprintf_s(temp,1024,"%s %s %s ", FloatString(v.mTangent[0]), FloatString(v.mTangent[1]), FloatString(v.mTangent[2]) );
		strcat_s(scratch,1024,temp);
	}
	if ( flags & MIVF_BINORMAL )
	{
		sprintf_s(temp,1024,"%s %s %s ", FloatString(v.mBiNormal[0]), FloatString(v.mBiNormal[1]), FloatString(v.mBiNormal[2]) );
		strcat_s(scratch,1024,temp);
	}
	if ( flags & MIVF_BONE_WEIGHTING )
	{
		sprintf_s(temp,1024,"%s %s %s %s ", FloatString(v.mWeight[0]), FloatString(v.mWeight[1]), FloatString(v.mWeight[2]), FloatString(v.mWeight[3]) );
		strcat_s(scratch,1024,temp);
		sprintf_s(temp,1024,"%d %d %d %d ", v.mBone[0], v.mBone[1], v.mBone[2], v.mBone[3] );
		strcat_s(scratch,1024,temp);
	}
	if ( flags & MIVF_INTERP1 )
	{
		sprintf_s(temp,1024,"%s %s %s ", FloatString(v.mInterp1[0]), FloatString(v.mInterp1[1]), FloatString(v.mInterp1[2]) , FloatString(v.mInterp1[3]));
		strcat_s(scratch,1024,temp);
	}
	if ( flags & MIVF_INTERP2 )
	{
		sprintf_s(temp,1024,"%s %s %s ", FloatString(v.mInterp2[0]), FloatString(v.mInterp2[1]), FloatString(v.mInterp2[2]) , FloatString(v.mInterp2[3]));
		strcat_s(scratch,1024,temp);
	}
	if ( flags & MIVF_INTERP3 )
	{
		sprintf_s(temp,1024,"%s %s %s ", FloatString(v.mInterp3[0]), FloatString(v.mInterp3[1]), FloatString(v.mInterp3[2]) , FloatString(v.mInterp3[3]));
		strcat_s(scratch,1024,temp);
	}
	if ( flags & MIVF_INTERP4 )
	{
		sprintf_s(temp,1024,"%s %s %s ", FloatString(v.mInterp4[0]), FloatString(v.mInterp4[1]), FloatString(v.mInterp4[2]) , FloatString(v.mInterp4[3]));
		strcat_s(scratch,1024,temp);
	}
	if ( flags & MIVF_INTERP5 )
	{
		sprintf_s(temp,1024,"%s %s %s ", FloatString(v.mInterp5[0]), FloatString(v.mInterp5[1]), FloatString(v.mInterp5[2]) , FloatString(v.mInterp5[3]));
		strcat_s(scratch,1024,temp);
	}
	if ( flags & MIVF_INTERP6 )
	{
		sprintf_s(temp,1024,"%s %s %s ", FloatString(v.mInterp6[0]), FloatString(v.mInterp6[1]), FloatString(v.mInterp6[2]) , FloatString(v.mInterp6[3]));
		strcat_s(scratch,1024,temp);
	}
	if ( flags & MIVF_INTERP7 )
	{
		sprintf_s(temp,1024,"%s %s %s ", FloatString(v.mInterp7[0]), FloatString(v.mInterp7[1]), FloatString(v.mInterp7[2]) , FloatString(v.mInterp7[3]));
		strcat_s(scratch,1024,temp);
	}
	if ( flags & MIVF_INTERP8 )
	{
		sprintf_s(temp,1024,"%s %s %s ", FloatString(v.mInterp8[0]), FloatString(v.mInterp8[1]), FloatString(v.mInterp8[2]) , FloatString(v.mInterp8[3]));
		strcat_s(scratch,1024,temp);
	}
	if ( flags & MIVF_RADIUS )
	{
		sprintf_s(temp,1024,"%s ", FloatString(v.mRadius) );
		strcat_s(scratch,1024,temp);
	}
	strcat_s(scratch,1024,",    ");
	physx::PxU32 slen = (physx::PxI32)strlen(scratch);
	fprintf(fph,"%s", scratch );
	column+=slen;
	if ( column >= 160 )
		newRow = true;
}

static void printIndex(FILE *fph,const physx::PxU32 *idx,physx::PxU32 &column,bool &newRow)
{
	if ( newRow )
	{
		if ( column != 0 )
		{
			fprintf(fph,"\r\n");
		}
		newRow = false;
		column = 0;
		fprintf(fph,"          ");
	}
	char temp[1024];
	sprintf_s(temp,1024,"%d %d %d,  ", idx[0], idx[1], idx[2] );
	fprintf(fph,"%s",temp);
	physx::PxU32 slen = (physx::PxU32)strlen(temp);
	column+=slen;
	if ( column >= 160 )
		newRow = true;
}


static void print(FILE *fph,SubMesh *m)
{
	fprintf(fph,"      <MeshSection material=\"%s\" ctype=\"%s\" semantic=\"%s\">\r\n", m->mMaterialName, getCtype(m->mVertexFlags), getSemantics(m->mVertexFlags) );
	printAABB(fph,m->mAABB);

	fprintf(fph,"        <indexbuffer triangle_count=\"%d\">\r\n", m->mTriCount );
	const physx::PxU32 *scan = m->mIndices;
	bool newRow = true;
	physx::PxU32 column = 0;
	for (physx::PxU32 i=0; i<m->mTriCount; i++)
	{
		printIndex(fph,scan,column,newRow);
		scan+=3;
	}
	fprintf(fph,"\r\n");
	fprintf(fph,"        </indexbuffer>\r\n");

	fprintf(fph,"      </MeshSection>\r\n");
}

static void print(FILE *fph,Mesh *m)
{
	fprintf(fph,"      <Mesh name=\"%s\" skeleton=\"%s\" submesh_count=\"%d\">\r\n", m->mName, m->mSkeletonName, m->mSubMeshCount );
	printAABB(fph,m->mAABB);

	fprintf(fph,"        <vertexbuffer count=\"%d\" ctype=\"%s\" semantic=\"%s\">\r\n", m->mVertexCount, getCtype(m->mVertexFlags), getSemantics(m->mVertexFlags) );

	bool newRow=true;
	physx::PxU32 column=0;

	for (physx::PxU32 i=0; i<m->mVertexCount; i++)
	{
		printVertex(fph, m->mVertexFlags, m->mVertices[i], column, newRow );

	}
	fprintf(fph,"\r\n");
	fprintf(fph,"        </vertexbuffer>\r\n");


	for (physx::PxU32 i=0; i<m->mSubMeshCount; i++)
	{
		print(fph,m->mSubMeshes[i]);
	}

	fprintf(fph,"      </Mesh>\r\n");
}

static void print(FILE *fph,MeshInstance &m)
{
	fprintf(fph,"        <MeshInstance mesh=\"%s\" position=\"%s,%s,%s\" rotation=\"%s,%s,%s,%s\" scale=\"%s,%s,%s\"/>\r\n",
		m.mMeshName,
		FloatString( m.mPosition[0] ),
		FloatString( m.mPosition[1] ),
		FloatString( m.mPosition[2] ),
		FloatString( m.mRotation[0] ),
		FloatString( m.mRotation[1] ),
		FloatString( m.mRotation[2] ),
		FloatString( m.mRotation[3] ),
		FloatString( m.mScale[0] ),
		FloatString( m.mScale[1] ),
		FloatString( m.mScale[2] ) );
}

#if 0 // export of collision types not yet fully implemented
static const char * getTypeString(MeshCollisionType t)
{
	const char *ret = "unknown";
	switch ( t )
	{
	case MCT_BOX: ret = "BOX"; break;
	case MCT_SPHERE: ret = "SPHERE"; break;
	case MCT_CAPSULE: ret = "CAPSULE"; break;
	case MCT_CONVEX: ret = "CONVEX"; break;
	case MCT_LAST: break; // Make compiler happy
	}
	return ret;
}

static void print(FILE *fph,MeshCollisionBox * /*b*/)
{
	fprintf(fph,"       <MeshCollisionBox >\r\n");
	fprintf(fph,"       </MeshCollisionBox>\r\n");
}

static void print(FILE *fph,MeshCollisionSphere * /*b*/)
{
	fprintf(fph,"       <MeshCollisionSphere >\r\n");
	fprintf(fph,"       </MeshCollisionSphere>\r\n");
}

static void print(FILE *fph,MeshCollisionCapsule * /*b*/)
{
	fprintf(fph,"       <MeshCollisionCapsule >\r\n");
	fprintf(fph,"       </MeshCollisionCapsule>\r\n");
}

static void print(FILE *fph,MeshCollisionConvex *m)
{
	fprintf(fph,"       <MeshCollisionConvex >\r\n");

	{
		bool newRow = true;
		fprintf(fph,"         <vertexbuffer count=\"%d\" ctype=\"fff\" semantic=\"position\">\r\n", m->mVertexCount );
		for (physx::PxU32 i=0; i<m->mVertexCount; i++)
		{
			const physx::PxF32 *p = &m->mVertices[i*3];
			if ( newRow )
			{
				fprintf(fph,"          ");
				newRow = false;
			}

			fprintf(fph,"%s %s %s, ",FloatString(p[0]), FloatString(p[1]), FloatString(p[2]) );
			if ( (i&7) == 0 )
			{
				fprintf(fph,"\r\n");
				newRow = true;
			}
		}
		if ( !newRow )
			fprintf(fph,"\r\n");
		fprintf(fph,"        </vertexbuffer>\r\n");
	}

	{
		fprintf(fph,"         <indexbuffer triangle_count=\"%d\">\r\n", m->mTriCount );
		const physx::PxU32 *scan = m->mIndices;
		bool newRow = true;
		physx::PxU32 column = 0;
		for (physx::PxU32 i=0; i<m->mTriCount; i++)
		{
			printIndex(fph,scan,column,newRow);
			scan+=3;
		}
		fprintf(fph,"\r\n");
		fprintf(fph,"        </indexbuffer>\r\n");
	}

	fprintf(fph,"       </MeshCollisionConvex>\r\n");
}
#endif


static void print(FILE *fph,MeshCollision *m)
{
	PX_ASSERT(0); // TODO TODO
	PX_FORCE_PARAMETER_REFERENCE(fph);
	PX_FORCE_PARAMETER_REFERENCE(m);
#if 0
	fprintf(fph,"        <MeshCollision name=\"%s\" type=\"%s\" transform=\"%s %s %s %s   %s %s %s %s   %s %s %s %s   %s %s %s %s\">\r\n",
		m->mName,getTypeString(m->mType),
		FloatString( m->mTransform[0] ),
		FloatString( m->mTransform[1] ),
		FloatString( m->mTransform[2] ),
		FloatString( m->mTransform[3] ),
		FloatString( m->mTransform[4] ),
		FloatString( m->mTransform[5] ),
		FloatString( m->mTransform[6] ),
		FloatString( m->mTransform[7] ),
		FloatString( m->mTransform[8] ),
		FloatString( m->mTransform[9] ),
		FloatString( m->mTransform[10] ),
		FloatString( m->mTransform[11] ),
		FloatString( m->mTransform[12] ),
		FloatString( m->mTransform[13] ),
		FloatString( m->mTransform[14] ),
		FloatString( m->mTransform[15] ) );

	switch ( m->mType )
	{
	case MCT_BOX:
		{
			MeshCollisionBox *b = static_cast< MeshCollisionBox *>(m);
			print(fph,b);
		}
		break;
	case MCT_SPHERE:
		{
			MeshCollisionSphere *b = static_cast< MeshCollisionSphere *>(m);
			print(fph,b);
		}
		break;
	case MCT_CAPSULE:
		{
			MeshCollisionCapsule *b = static_cast< MeshCollisionCapsule *>(m);
			print(fph,b);
		}
		break;
	case MCT_CONVEX:
		{
			MeshCollisionConvex *b = static_cast< MeshCollisionConvex *>(m);
			print(fph,b);
		}
		break;
	case MCT_LAST: // Make compiler happy
		break;
	}

	fprintf(fph,"        </MeshCollision>\r\n");
#endif
}

static void print(FILE *fph,MeshCollisionRepresentation *m)
{
	fprintf(fph,"      <MeshCollisionRepresentation name=\"%s\" info=\"%s\" count=\"%d\">\r\n", m->mName, m->mInfo, m->mCollisionCount );
	for (physx::PxU32 i=0; i<m->mCollisionCount; i++)
	{
		print(fph,m->mCollisionGeometry[i]);
	}
	fprintf(fph,"      </MeshCollisionRepresentation>\r\n");

}

static void saveMeshSystem(FILE *fph,const MeshSystem *mesh)
{
		fprintf(fph,"<?xml version=\"1.0\"?>\r\n");
		fprintf(fph,"  <MeshSystem asset_name=\"%s\" asset_info=\"%s\" mesh_system_version=\"%d\" mesh_system_asset_version=\"%d\">\r\n", getStr(mesh->mAssetName), getStr(mesh->mAssetInfo), mesh->mMeshSystemVersion, mesh->mAssetVersion );
		printAABB(fph,mesh->mAABB);

		//*******************************************************************
		//***
		//*** Output Textures
		//***
		//*******************************************************************
		if ( mesh->mTextureCount )
		{
			fprintf(fph,"    <Textures count=\"%d\">\r\n", mesh->mTextureCount );
			for (physx::PxU32 i=0; i<mesh->mTextureCount; i++)
			{
				print(fph,mesh->mTextures[i]);
			}
			fprintf(fph,"    </Textures>\r\n");
		}


		//*******************************************************************
		//***
		//*** Tetraheadral meshes
		//***
		//*******************************************************************
		if ( mesh->mTetraMeshCount )
		{
			fprintf(fph,"    <TetraMeshes count=\"%d\">\r\n", mesh->mTetraMeshCount );
			for (physx::PxU32 i=0; i<mesh->mTetraMeshCount; i++)
			{
				print(fph,mesh->mTetraMeshes[i]);
			}
			fprintf(fph,"    </TetraMeshes>\r\n");
		}

		//*******************************************************************
		//***
		//*** Output skeletons
		//***
		//*******************************************************************
		if ( mesh->mSkeletonCount )
		{
			fprintf(fph,"    <Skeletons count=\"%d\">\r\n", mesh->mSkeletonCount);
			for (physx::PxU32 i=0; i<mesh->mSkeletonCount; i++)
			{
				print(fph,mesh->mSkeletons[i]);
			}
			fprintf(fph,"    </Skeletons>\r\n");
		}

		//*******************************************************************
		//***
		//*** Output Animations
		//***
		//*******************************************************************
		if ( mesh->mAnimationCount )
		{
			fprintf(fph,"    <Animations count=\"%d\">\r\n", mesh->mAnimationCount );
			for (physx::PxU32 i=0; i<mesh->mAnimationCount; i++)
			{
				print(fph,mesh->mAnimations[i]);
			}
			fprintf(fph,"    </Animations>\r\n");
		}

		//*******************************************************************
		//***
		//*** Output Materials
		//***
		//*******************************************************************
		if ( mesh->mMaterialCount )
		{
			fprintf(fph,"    <Materials count=\"%d\">\r\n", mesh->mMaterialCount );
			for (physx::PxU32 i=0; i<mesh->mMaterialCount; i++)
			{
				print(fph,mesh->mMaterials[i]);
			}
			fprintf(fph,"    </Materials>\r\n", mesh->mMaterialCount );
		}


		//*******************************************************************
		//***
		//*** Output UserData
		//***
		//*******************************************************************
		// user data
		if ( mesh->mUserDataCount )
		{
			fprintf(fph,"    <UserData count=\"%d\">\r\n", mesh->mUserDataCount );
			for (physx::PxU32 i=0; i<mesh->mUserDataCount; i++)
			{
				print(fph,mesh->mUserData[i]);
			}
			fprintf(fph,"    </UserData>\r\n");
		}

		//*******************************************************************
		//***
		//*** Output UserBinaryData
		//***
		//*******************************************************************
		// user data
		if ( mesh->mUserBinaryDataCount )
		{
			fprintf(fph,"    <UserBinaryData count=\"%d\">\r\n", mesh->mUserBinaryDataCount );
			for (physx::PxU32 i=0; i<mesh->mUserBinaryDataCount; i++)
			{
				print(fph,mesh->mUserBinaryData[i]);
			}
			fprintf(fph,"    </UserBinaryData>\r\n");
		}


		//*******************************************************************
		//***
		//*** Output Meshes
		//***
		//*******************************************************************
		if ( mesh->mMeshCount )
		{
			fprintf(fph,"    <Meshes count=\"%d\">\r\n", mesh->mMeshCount );
			for (physx::PxU32 i=0; i<mesh->mMeshCount; i++)
			{
				print(fph,mesh->mMeshes[i]);
			}
			fprintf(fph,"    </Meshes>\r\n");
		}

		//*******************************************************************
		//***
		//*** Output MeshInstances
		//***
		//*******************************************************************
		if ( mesh->mMeshInstanceCount )
		{
			fprintf(fph,"    <MeshInstances count=\"%d\">\r\n", mesh->mMeshInstanceCount );
			for (physx::PxU32 i=0; i<mesh->mMeshInstanceCount; i++)
			{
				print(fph,mesh->mMeshInstances[i]);
			}
			fprintf(fph,"    </MeshInstances>\r\n");
		}

		//*******************************************************************
		//***
		//*** Output MeshCollisionRepresentations
		//***
		//*******************************************************************
		if ( mesh->mMeshCollisionCount )
		{
			fprintf(fph,"    <MeshCollisionRepresentations count=\"%d\">\r\n", mesh->mMeshCollisionCount );
			for (physx::PxU32 i=0; i<mesh->mMeshCollisionCount; i++)
			{
				print(fph,mesh->mMeshCollisionRepresentations[i]);
			}
			fprintf(fph,"    </MeshCollisionRepresentations>\r\n");
		}

		fprintf(fph,"  </MeshSystem>\r\n");
}


bool exportEZM(const char *fname,const MeshSystem *ms)
{
	bool ret = false;

	FILE *fph = fopen(fname,"wb");
	if ( fph )
	{

		saveMeshSystem(fph,ms);

		fclose(fph);
		ret = true;
	}
	
	return ret;
}

};

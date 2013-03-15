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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "MeshImport/EZMesh.h"
#include "MeshImport/MeshImportEZB.h"
#include "MeshImport/MeshExportEZM.h"
#include "MeshImport/MeshExportEZB.h"

#pragma warning(disable:4996)

void main(int argc,const char **argv)
{
	if ( argc != 2 )
	{
		printf("Usage: ezb <fname.ezb>\r\n");
	}
	else
	{
		FILE *fph = fopen(argv[1],"rb");
		if ( fph )
		{
			fseek(fph,0L,SEEK_END);
			size_t len = ftell(fph);
			fseek(fph,0L,SEEK_SET);
			if ( len > 0 )
			{
				printf("Reading input file '%s'\r\n", argv[1] );
				void * mem = ::malloc(len);
				if ( mem )
				{
					size_t r = fread(mem,len,1,fph);
					if ( r == 1 )
					{
						printf("Successfully read input file.\r\n");
						ezmesh::MeshImportEZB *ezb = ezmesh::createMeshImportEZB(mem);
						if ( ezb )
						{
							ezmesh::MeshSystem *ms = ezb->getMeshSystem();
							if ( ms )
							{
								printf("Successfully loaded the MeshSystem.\r\n");
								char scratch[512];
								strncpy(scratch,argv[1],512);
								char *dot = strchr(scratch,'.');
								if ( dot )
								{
									*dot = 0;
									strcat(scratch,".ezm");
									printf("Exporting binary file as XML EZ-Mesh to '%s'\r\n", scratch );
									bool ok = exportEZM(scratch,ms);
									printf("Export file(%s) complete.  Success=%s\r\n", scratch, ok ? "true" : "false");
									*dot = 0;
									strcat(scratch,"_EXPORT.ezb");
									printf("Re-exporting it in binary again just for testing purposes.\r\n");
									ok = exportEZB(scratch,ms);
									printf("Export file(%s) complete.  Success=%s\r\n", scratch, ok ? "true" : "false");
								}
								else
								{
									printf("Input file name missing extension?? %s\r\n", argv[1] );
								}
							}
							ezb->release();
						}
					}
					else
					{
						printf("Failed to read entire input file.\r\n");
					}
					::free(mem);
				}
				else
				{
					printf("Failed to allocated %d bytes of memory for the input file.\r\n", len );
				}
			}
			else
			{
				printf("Input file '%s' is empty.\r\n", argv[1] );
			}
			fclose(fph);
		}
		else
		{
			printf("Failed to open file '%s' for read access.\r\n", argv[1] );
		}
	}
}

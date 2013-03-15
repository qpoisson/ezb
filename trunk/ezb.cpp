#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include "MeshImport/MeshImport.h"
#include "MeshImport/MeshImportEZB.h"
#include "MeshImport/MeshExportEZM.h"

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
						mimp::MeshImportEZB *ezb = mimp::createMeshImportEZB(mem);
						if ( ezb )
						{
							mimp::MeshSystem *ms = ezb->getMeshSystem();
							if ( ms )
							{
								printf("Successfully loaded the MeshSystem.\r\n");
								char scratch[512];
								strncpy(scratch,argv[1],512);
								char *dot = strchr(scratch,'.');
								if ( dot )
								{
									*dot = 0;
								}
								strcat(scratch,".ezm");
								printf("Exporting binary file as XML EZ-Mesh to '%s'\r\n", scratch );
								bool ok = exportEZM(scratch,ms);
								printf("Export complete.  Success=%s\r\n", ok ? "true" : "false");
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

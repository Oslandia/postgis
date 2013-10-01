/**********************************************************************
 *
 * PostGIS - Spatial Types for PostgreSQL
 * http://postgis.refractions.net
 *
 * Referenced object functions
 *
 * Copyright 2012-2013 Oslandia <infos@oslandia.com>
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU General Public Licence. See the COPYING file.
 *
 **********************************************************************/

#include "postgres.h"
#include "fmgr.h"
#include "liblwgeom.h"

#define REF_TYPE_SFCGALGEOMETRY  0
#define REF_TYPE_GEOSGEOMETRY    1
#define REF_TYPE_RASTER          2

#define NUM_REF_TYPES            1

/**
 * Referenced geometries
 *
 * This structure is a varlena.
 * It is based on GSERIALIZED with a special flag
 */
typedef struct
{
    uint32_t size;
    uint8_t  srid[3];  /* always 0 for a ref_object */
    uint8_t  flags;    /* always FLAG_GET_ISPOINTER == 1 */
    uint8_t  ref_type; /* type of the object */
    void *   ref_ptr;  /* the actual memory address of the underlying object */
} ref_object_t;

GSERIALIZED* serialize_ref_object( void * pgeom, bool nested, int type );
void* unserialize_ref_object( Datum rgeom, int requested_type );

/**
 * DETOAST macro. It will detoast and unserialize the geometry back to a GSERIALIZED
 */
#define POSTGIS_DETOAST_DATUM(x) unserialize_ref_object( PG_DETOAST_DATUM(x), -1 )

/**
 * Macro used to access the 'nested' member of fcinfo
 */
#if PG_KNOWS_NESTED_CALLS
#define PG_FUNCTION_NESTED (fcinfo->nested)
#else
#define PG_FUNCTION_NESTED false
#endif



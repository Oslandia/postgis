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

#define REF_TYPE_SFCGALGEOMETRY  0
#define REF_TYPE_GEOSGEOMETRY    1
#define REF_TYPE_RASTER          2

#define NUM_REF_TYPES            1

/**
 * Referenced geometries
 *
 * This structure is a varlena.
 * If size is greater than the minimum GSERIALIZED size (16), then it's a GSERIALIZED,
 * else it is a referenced geometry
 */
typedef struct
{
    uint32_t size;
    char ref_type;
    void* ref_ptr;
    /* we have 3 bytes left here for possible extra information */
} ref_object_t;

ref_object_t* serialize_ref_object( void * pgeom, bool nested, int type );
void* unserialize_ref_object( ref_object_t * rgeom, int requested_type );

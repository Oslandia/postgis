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

#include "lwgeom_ref.h"
#include "lwgeom_sfcgal.h"

static GSERIALIZED* sfcgal_serialize( void *ptr );

struct ref_type_definition
{
    const char* name;
    GSERIALIZED* (*serialize_fn)   ( void *ptr );
    void*        (*deserialize_fn) ( GSERIALIZED *ptr );
};

static GSERIALIZED* sfcgal_serialize( void *ptr )
{
    GSERIALIZED *ret;
    sfcgal_prepared_geometry_t* pgeom = (sfcgal_prepared_geometry_t*)ptr;
    ret = SFCGALPreparedGeometry2POSTGIS( pgeom, /* force3D?? */0 );
    sfcgal_prepared_geometry_delete( pgeom );
    return ret;
}

struct ref_type_definition ref_types[NUM_REF_TYPES] = {
    {
	.name = "sfcgal",
	.serialize_fn = sfcgal_serialize,
	.deserialize_fn = POSTGIS2SFCGALPreparedGeometry
    }
};

ref_object_t* serialize_ref_object( void *pgeom, bool nested, int type )
{
    ref_object_t* ret;

    if ( ! nested ) {
	/* serialize */
	lwnotice("[SERIALIZE] serialize to GSERIALIZED from '%s'", ref_types[type].name );
	ret = (*ref_types[type].serialize_fn)( pgeom );
    }
    else {
	lwnotice("[SERIALIZE] no need to serialize, pass pointer of type '%s'", ref_types[type].name );
	ret = (ref_object_t*)lwalloc( sizeof(ref_object_t) );
	SET_VARSIZE( ret, sizeof(ref_object_t) );
	ret->ref_ptr = pgeom;
	ret->ref_type = type;
    }

    return ret;
}

/*
 * ginput: Datum from PG_GETARG_DATUM(i)
 */
void* unserialize_ref_object( ref_object_t * ginput, int requested_type )
{
    void *ret;
    ref_object_t *rgeom;

    rgeom = PG_DETOAST_DATUM( ginput );

    uint32_t s = VARSIZE(rgeom);
    if ( s == sizeof(ref_object_t) ) {
	if ( requested_type == -1 ) {
	    lwnotice("[REF] forcing serialization");
	    ret = (*ref_types[rgeom->ref_type].serialize_fn) (rgeom->ref_ptr);
	}
	else if ( rgeom->ref_type != requested_type ) {
	    lwnotice("[REF] type conversion from '%s' to '%s'", ref_types[rgeom->ref_type].name, ref_types[requested_type].name);
	    /* serialize the current pointer */
	    GSERIALIZED* sobj = (*ref_types[rgeom->ref_type].serialize_fn) ( rgeom->ref_ptr );
	    /* unserialize to the requested type */
	    ret = (*ref_types[requested_type].deserialize_fn) ( sobj );
	}
	else {
	    lwnotice("[REF] deserialize a pointer of type %s", ref_types[rgeom->ref_type].name );
	    ret = rgeom->ref_ptr;
	}
    }
    else {
	if ( requested_type == -1 ) {
	    ret = rgeom;
	}
	else {
	    lwnotice("[REF] deserialize to pointer of type %s", ref_types[requested_type].name );
	    ret = (*ref_types[requested_type].deserialize_fn) (rgeom);
	    /* PG_FREE_IF_COPY equivalent */
	    if ( ginput != rgeom ) {
		pfree( rgeom );
	    }
	}
    }

    return ret;
}

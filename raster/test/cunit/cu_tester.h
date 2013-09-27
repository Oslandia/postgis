/**********************************************************************
 * $Id$
 *
 * PostGIS - Spatial Types for PostgreSQL
 * http://postgis.refractions.net
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU General Public Licence. See the COPYING file.
 *
 **********************************************************************/

#include "librtcore.h"

#include "../../../postgis_config.h"
#include "../../raster_config.h"

#define PG_TEST(test_func) { #test_func, test_func }
#define MAX_CUNIT_MSG_LENGTH 512

/* Contains the most recent error message generated by rterror. */
char cu_error_msg[MAX_CUNIT_MSG_LENGTH+1];

/* Resets cu_error_msg back to blank. */
void cu_error_msg_reset(void);

/* free raster object */
void cu_free_raster(rt_raster raster);

/* helper to add bands to raster */
rt_band cu_add_band(
	rt_raster raster,
	rt_pixtype pixtype,
	int hasnodata, double nodataval
);

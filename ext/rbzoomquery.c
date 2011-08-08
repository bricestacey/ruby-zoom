/*
 * Copyright (C) 2005 Laurent Sansonetti <lrz@chopine.be>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "rbzoom.h"

#ifdef MAKING_RDOC_HAPPY
mZoom = rb_define_module("ZOOM");
#endif

/* Class: ZOOM::Query
 * Search queries.
 */
static VALUE cZoomQuery;

static VALUE
rbz_query_make (ZOOM_query query)
{
    return query != NULL
        ? Data_Wrap_Struct (cZoomQuery,
                            NULL,
                            ZOOM_query_destroy,
                            query)
        : Qnil;
}

ZOOM_query
rbz_query_get (VALUE obj)
{
    ZOOM_query query;
        
    Data_Get_Struct (obj, struct ZOOM_query_p, query);
    assert (query != NULL);

    return query;
}

/*
 * call-seq: new_prefix(prefix)
 *
 * prefix: PQF notation.
 *
 * Creates a RPN query using the given PQF notation.
 *
 * Returns: a newly created ZOOM::Query object.
 */
static VALUE
rbz_query_new_prefix (VALUE self, VALUE prefix)
{
    ZOOM_query query;

    query = ZOOM_query_create ();
    ZOOM_query_prefix (query, RVAL2CSTR (prefix)); 
    
    return rbz_query_make (query);
}

/* call-seq:
 * 	 new_cql(prefix)
 *
 * prefix: CQL notation.
 *
 * Creates a CQL query using the given CQL notation.
 *
 * Returns: a newly created ZOOM::Query object.
 */
static VALUE
rbz_query_new_cql (VALUE self, VALUE cql)
{
    ZOOM_query query;

    query = ZOOM_query_create ();
    ZOOM_query_cql (query, RVAL2CSTR (cql)); 
    
    return rbz_query_make (query);
}

/*
 * call-seq: new_sort_by(criteria)
 *
 * criteria: a sort criteria.
 *
 * Creates a sort query from the YAZ sorting notation.
 *
 * Returns: a newly created ZOOM::Query object.
 */
static VALUE
rbz_query_new_sort_by (VALUE self, VALUE criteria)
{
    ZOOM_query query;

    query = ZOOM_query_create ();
    ZOOM_query_sortby (rbz_query_get (self), RVAL2CSTR (criteria)); 
    
    return rbz_query_make (query);
}

void
Init_zoom_query (VALUE mZoom)
{
    VALUE c;

    c = rb_define_class_under (mZoom, "Query", rb_cObject); 
    rb_define_singleton_method (c, "new_prefix", rbz_query_new_prefix, 1);
    rb_define_singleton_method (c, "new_cql", rbz_query_new_cql, 1);
    rb_define_singleton_method (c, "new_sort_by", rbz_query_new_sort_by, 1);
            
    cZoomQuery = c;
}

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

#ifndef __RBZOOM_H_
#define __RBZOOM_H_

#include <yaz/zoom.h>
#include <ruby.h>
#include <assert.h>

/* initialization */
void Init_zoom (void);
void Init_zoom_connection (VALUE mZoom);
void Init_zoom_query (VALUE mZoom);
void Init_zoom_resultset (VALUE mZoom);
void Init_zoom_record (VALUE mZoom);
void Init_zoom_package (VALUE mZoom);

/* rbzoomoptions.c */
ZOOM_options ruby_hash_to_zoom_options (VALUE hash);
VALUE zoom_option_value_to_ruby_value (const char *value);
void define_zoom_option (VALUE klass, const char *option);

/* rbzoomparse.c */
ZOOM_query rbz_query_get (VALUE obj);

/* rbzoomresultset.c */
VALUE rbz_resultset_make (ZOOM_resultset resultset);

/* rbzoomrecord.c */
VALUE rbz_record_make (ZOOM_record record);

/* rbzoompackage.c */
VALUE rbz_package_make (ZOOM_connection connection, ZOOM_options options);

/* rbconnection.c */
void rbz_connection_check(VALUE obj); 
        
/* useful macros */
#if !defined (RVAL2CSTR)
# define RVAL2CSTR(x)       (NIL_P (x) ? NULL : RSTRING_PTR(x))
#endif
#if !defined (CSTR2RVAL)
# define CSTR2RVAL(x)       (x == NULL ? Qnil : rb_str_new2(x))
#endif
#if !defined (RVAL2CBOOL)
# define RVAL2CBOOL(x)      (RTEST (x))
#endif
#if !defined (CBOOL2RVAL)
# define CBOOL2RVAL(x)      (x ? Qtrue : Qfalse)
#endif

#endif /* __RBZOOM_H_ */

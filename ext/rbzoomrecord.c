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


/* 
 * A record object is a retrieval record on the client side - created from 
 * result sets.
 */
static VALUE cZoomRecord;

VALUE
rbz_record_make (ZOOM_record record)
{
    return record != NULL
        ? Data_Wrap_Struct (cZoomRecord,
                            NULL,
                            ZOOM_record_destroy,
                            record)
        : Qnil;
}

static ZOOM_record
rbz_record_get (VALUE obj)
{
    ZOOM_record record;
        
    Data_Get_Struct (obj, struct ZOOM_record_p, record);
    assert (record != NULL);

    return record;
}

static char _type [128];

static const char *
rbz_record_type (const char *form, int argc, VALUE *argv)
{
    VALUE charset_from;
    VALUE charset_to;

    if (argc == 0)
        return form;

    rb_scan_args (argc, argv, "11", &charset_from, &charset_to);
   
    memset (_type, 0, sizeof _type);
    
    if (NIL_P (charset_to))
        snprintf (_type, sizeof _type, "%s; charset=%s", form, 
                  RVAL2CSTR (charset_from));
    else
        snprintf (_type, sizeof _type, "%s; charset=%s,%s", form, 
                  RVAL2CSTR (charset_from), RVAL2CSTR (charset_to));

    return _type;
}

/*
 * call-seq: 
 * 	database(charset_from=nil, charset_to=nil)
 *
 * charset_from: the name of the charset to convert from (optional).
 *
 * charset_to: the name of the charset to convert to (optional).
 *
 * Returns: the database name of the record.
 */
static VALUE
rbz_record_database (int argc, VALUE *argv, VALUE self)
{
    return CSTR2RVAL (ZOOM_record_get (rbz_record_get (self),
                                       rbz_record_type ("database", argc, argv),
                                       NULL));    
}

/*
 * call-seq: 
 * 	syntax(charset_from=nil, charset_to=nil)
 *
 * charset_from: the name of the charset to convert from (optional).
 *
 * charset_to: the name of the charset to convert to (optional).
 *
 * Returns: the symbolic transfer syntax name of the record.
 */
static VALUE
rbz_record_syntax (int argc, VALUE *argv, VALUE self)
{
    return CSTR2RVAL (ZOOM_record_get (rbz_record_get (self),
                                       rbz_record_type ("syntax", argc, argv),
                                       NULL));    
}

/*
 * call-seq: 
 * 	render(charset_from=nil, charset_to=nil)
 *
 * charset_from: the name of the charset to convert from (optional).
 *
 * charset_to: the name of the charset to convert to (optional).
 *
 * Returns: a display friendly description of the record.
 */
static VALUE
rbz_record_render (int argc, VALUE *argv, VALUE self)
{
    return CSTR2RVAL (ZOOM_record_get (rbz_record_get (self),
                                       rbz_record_type ("render", argc, argv),
                                       NULL));    
}

/*
 * call-seq: 
 * 	xml(charset_from=nil, charset_to=nil)
 *
 * charset_from: the name of the charset to convert from (optional).
 *
 * charset_to: the name of the charset to convert to (optional).
 *
 * Returns an XML description of the record.  SRW/SRU and Z39.50 records with 
 * transfer syntax XML are returned verbatim.  MARC records are returned in 
 * MARCXML (converted from ISO2709 to MARCXML by YAZ). GRS-1 and OPAC records are 
 * not supported for this form. 
 * 
 * Returns: an XML description of the record.
 */
static VALUE
rbz_record_xml (int argc, VALUE *argv, VALUE self)
{
    return CSTR2RVAL (ZOOM_record_get (rbz_record_get (self),
                                       rbz_record_type ("xml", argc, argv),
                                       NULL));    
}

/*
 * call-seq: 
 * 	raw
 *
 * MARC records are returned in ISO2709.
 * GRS-1 and OPAC records are not supported for this form. 
 * 
 * Returns: an ISO2709 record.
 */

static VALUE
rbz_record_raw (int argc, VALUE *argv, VALUE self)
{
    return CSTR2RVAL (ZOOM_record_get (rbz_record_get (self),
                                       rbz_record_type ("raw", argc, argv),
                                       NULL));    
}

void
Init_zoom_record (VALUE mZoom)
{
    VALUE c;
    
    c = rb_define_class_under (mZoom, "Record", rb_cObject); 
    rb_undef_method (CLASS_OF (c), "new");

    rb_define_method (c, "database", rbz_record_database, -1);
    rb_define_method (c, "syntax", rbz_record_syntax, -1);
    rb_define_method (c, "render", rbz_record_render, -1);
    rb_define_alias (c, "to_s", "render");
    rb_define_method (c, "xml", rbz_record_xml, -1);
    rb_define_method (c, "raw", rbz_record_raw, -1);
    
    cZoomRecord = c;
}

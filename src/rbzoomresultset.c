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

/* Class: ZOOM::ResultSet
 * The result set object is a container for records returned from a target.
 */
static VALUE cZoomResultSet;

VALUE
rbz_resultset_make (ZOOM_resultset resultset)
{
    return resultset != NULL
        ? Data_Wrap_Struct (cZoomResultSet,
                            NULL,
                            ZOOM_resultset_destroy,
                            resultset)
        : Qnil;
}

static ZOOM_resultset
rbz_resultset_get (VALUE obj)
{
    ZOOM_resultset resultset;
        
    Data_Get_Struct (obj, struct ZOOM_resultset_p, resultset);
    assert (resultset != NULL);

    return resultset;
}

/*
 * call-seq: 
 * 	set_option(key, value)
 *
 * key: the name of the option, as a string.
 *
 * value: the value of this option (as a string, integer or boolean).
 *
 * Sets an option on the result set.
 * 
 * Returns: self.
 */
static VALUE
rbz_resultset_set_option (VALUE self, VALUE key, VALUE val)
{
    ZOOM_resultset_option_set (rbz_resultset_get (self),
                               RVAL2CSTR (key),
                               RVAL2CSTR (rb_obj_as_string (val)));
    
    return self;
}

/*
 * call-seq: 
 * 	get_option(key)
 *
 * key: the name of the option, as a string.
 *
 * Gets the value of a result set's option.
 * 
 * Returns: the value of the given option, as a string, integer or boolean.
 */
static VALUE
rbz_resultset_get_option (VALUE self, VALUE key)
{
    const char *value;
 
    value = ZOOM_resultset_option_get (rbz_resultset_get (self),
                                       RVAL2CSTR (key));

    return zoom_option_value_to_ruby_value (value);
}

/* 
 * Returns: the number of hits.
 */
static VALUE
rbz_resultset_size (VALUE self)
{
    return INT2NUM (ZOOM_resultset_size (rbz_resultset_get (self)));
}

/*
 * call-seq:
 * 	[](key)
 *
 * key: either an integer, a range or an interval of 2 integers.
 *
 * Retrieves one or many records from the result set, according to the given
 * key.
 *
 * 	# Gets the first record.
 * 	rset[0]
 * 	# Gets the first, second and third records.
 * 	rset[1..3]
 * 	# Gets three records starting from the second one.
 * 	rset[2, 3]
 *
 * Returns: one or many references to ZOOM::Record objects.
 */
static VALUE
rbz_resultset_index (int argc, VALUE *argv, VALUE self)
{
    ZOOM_record *records;
    ZOOM_record record;
    VALUE ary;
    size_t begin;
    size_t count;
    size_t i;
    
    if (argc == 1) {
        VALUE arg = argv [0];

        if (TYPE (arg) == T_FIXNUM || TYPE (arg) == T_BIGNUM) {
            record = ZOOM_resultset_record (rbz_resultset_get (self),
                                            NUM2LONG (arg));
            return record != NULL
                ? rbz_record_make (ZOOM_record_clone (record))
                : Qnil;
        }
       
        if (CLASS_OF (arg) == rb_cRange) {
            begin = NUM2LONG (rb_funcall (arg, rb_intern ("begin"), 0));
            count = NUM2LONG (rb_funcall (arg, rb_intern ("end"), 0));
            count -= begin;
        }
        else
            rb_raise (rb_eArgError, 
                      "Invalid argument of type %s (not Numeric or Range)",
                      rb_class2name (CLASS_OF (arg)));
    }
    else {
        VALUE rb_begin;
        VALUE rb_count;
        
        rb_scan_args (argc, argv, "2", &rb_begin, &rb_count);
        
        begin = NUM2LONG (rb_begin);
        count = NUM2LONG (rb_count);
    }
        
    ary = rb_ary_new ();
    if (count == 0)
        return ary;

    /* Allocate array */
    records = ALLOC_N (ZOOM_record, count);

    /* Download records in batches */
    ZOOM_resultset_records (rbz_resultset_get (self), records, begin, count);

    /* Test the first record in the set.  If null, then fall back.  If valid, 
     * generate the ruby array.
     */

    if (records[0]!=NULL) {
       for (i = 0; i < count; i++)

         /* We don't want any null records -- if there is on in the resultset, 
          * ignore it.
          */

         if (records[i]!=NULL)
            rb_ary_push (ary, rbz_record_make (ZOOM_record_clone (records [i])));
    } else {
      /* This is our fallback function
       * It exists for those anomalies where the server 
       * will not respect the batch request and will return just 
       * a null array (per change request 36 where Laurent Sansonetti notes
       *    Retrieves the record one by one using ZOOM_resultset_record instead
       *    of getting them all in once with ZOOM_resultset_records (for a strange
       *    reason sometimes the resultset was not empty but ZOOM_resultset_records
       *    used to return empty records).
       */

      for (i = 0; i < count; i++) {
        record = ZOOM_resultset_record (rbz_resultset_get (self),
                                        begin + i);
        /* Ignore null records */
        if (record != NULL)
            rb_ary_push (ary, rbz_record_make (ZOOM_record_clone (record)));
      }
    }

    return ary;
}

/*
 * Lists the records inside the result set.
 *
 * Returns: an array of ZOOM::Record objects.
 */
static VALUE
rbz_resultset_records (VALUE self)
{
    VALUE argv [2];

    argv [0] = INT2FIX (0);
    argv [1] = rbz_resultset_size (self);
    
    return rbz_resultset_index (2, argv, self);
}

/*
 * call-seq: 
 * 	each_record { |record| ... }
 *
 * Parses the records inside the result set and call the given block for each
 * record, passing a reference to a ZOOM::Record object as parameter.
 *
 * Returns: self.
 */
static VALUE
rbz_resultset_each_record (VALUE self)
{
    rb_ary_each (rbz_resultset_records (self));
    return self;
}

void
Init_zoom_resultset (VALUE mZoom)
{
    VALUE c;
    
    c = rb_define_class_under (mZoom, "ResultSet", rb_cObject); 
    rb_undef_method (CLASS_OF (c), "new");
    rb_define_method (c, "set_option", rbz_resultset_set_option, 2);
    rb_define_method (c, "get_option", rbz_resultset_get_option, 1);
    
    define_zoom_option (c, "start");
    define_zoom_option (c, "count");
    define_zoom_option (c, "presentChunk");
    define_zoom_option (c, "elementSetName");
    define_zoom_option (c, "preferredRecordSyntax");
    define_zoom_option (c, "schema");
    define_zoom_option (c, "setname");
    
    rb_define_method (c, "size", rbz_resultset_size, 0);
    rb_define_alias (c, "length", "size");
    rb_define_method (c, "records", rbz_resultset_records, 0);
    rb_define_method (c, "each_record", rbz_resultset_each_record, 0);
    rb_define_method (c, "[]", rbz_resultset_index, -1);
    
    cZoomResultSet = c;
}

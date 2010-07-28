/*
 * Copyright (C) 2007 Katipo Communications, Ltd.
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
#include <stdio.h>

#ifdef MAKING_RDOC_HAPPY
mZoom = rb_define_module("ZOOM");
#endif

/* Document-class: ZOOM::Package
 *
 * This class represents an Extended Services Package: an instruction to the server 
 * to do something not covered by the core parts of the Z39.50 standard 
 */
static VALUE cZoomPackage;


static ZOOM_package
rbz_package_get (VALUE obj)
{
    ZOOM_package package;

	if (cZoomPackage == Qnil)
		rb_raise(rb_eRuntimeError, "cZoomPackage is nil: has destroy() already been called on this Package?");
        
    Data_Get_Struct (obj, struct ZOOM_package_p, package);
    assert (package != NULL);

    return package;
}


/*
 * call-seq: 
 * 	make(connection, options)
*
*  Creates a ZOOM::Package from the connection and options specified.
*
*  Returns: the created ZOOM::Package or Qnil.
*/
VALUE
rbz_package_make (ZOOM_connection connection, ZOOM_options options)
{

  ZOOM_package package;

  package =  ZOOM_connection_package(connection, options);

  if (cZoomPackage == Qnil)
	rb_raise(rb_eRuntimeError, "cZoomPackage is nil: has destroy() already been called on this Package?");

  return package != NULL
      ? Data_Wrap_Struct (cZoomPackage,
                          NULL,
                          ZOOM_package_destroy,
                          package)
      : Qnil;
}


/*
 * call-seq:
 * 	set_option(key, value)
 *
 * key: the name of the option, as a string.
 *
 * value: the value of this option (as a string, integer or boolean).
 *
 * Sets an option on the package.
 * 
 * Returns: self.
 */
static VALUE
rbz_package_set_option (VALUE self, VALUE key, VALUE val)
{   

	ZOOM_package package;
    
    package = rbz_package_get (self);
    ZOOM_package_option_set (package,
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
 * Gets the value of a package's option.
 * 
 * Returns: the value of the given option, as a string, integer or boolean.
 */
static VALUE
rbz_package_get_option (VALUE self, VALUE key)
{
	ZOOM_package package;
    const char *value;
 
    package = rbz_package_get (self);
 
    value = ZOOM_package_option_get (package,
                                        RVAL2CSTR (key));

    return zoom_option_value_to_ruby_value (value);
}

/*
 * call-seq:
 * 	send(type)
 *
 * type:  the actual extended service package type to be sent, as a string.
 *
 * Sends the package.
 * 
 * Returns: self.
 */
static VALUE
rbz_package_send(VALUE self, VALUE type)
{
    ZOOM_package package;
	const char *typeChar;

    package = rbz_package_get (self);

    typeChar = StringValuePtr(type);
    ZOOM_package_send(package, typeChar);
  
    return self;
}



/* Interface to a subset of the Z39.50 extended services.
*/
void
Init_zoom_package (VALUE mZoom)
{
    VALUE c;
    
    c = rb_define_class_under (mZoom, "Package", rb_cObject); 

	/* Remove the default constructor to force initialization through Connection#package. */
    rb_undef_method (CLASS_OF (c), "new");

	/* Instance methods */
    rb_define_method (c, "set_option", rbz_package_set_option, 2);
    rb_define_method (c, "get_option", rbz_package_get_option, 1);
    rb_define_method (c, "send", rbz_package_send, 1);

	// Common Options
    define_zoom_option (c, "package-name");
	define_zoom_option (c, "user-id");
	define_zoom_option (c, "function");
	define_zoom_option (c, "waitAction");
	define_zoom_option (c, "targetReference");
	
	// Item Order, type must be set to itemorder in ZOOM_package_send.
	define_zoom_option (c, "contact-name");
	define_zoom_option (c, "contact-phone");
	define_zoom_option (c, "contact-email");
	define_zoom_option (c, "itemorder-item");
	
	// Record Update, type must be set to update in ZOOM_package_send.
	define_zoom_option (c, "action");
	define_zoom_option (c, "recordIdOpaque");
	define_zoom_option (c, "recordIdNumber");
	define_zoom_option (c, "record");
	define_zoom_option (c, "syntax");
	define_zoom_option (c, "databaseName");
	define_zoom_option (c, "correlationInfo.note");
	define_zoom_option (c, "correlationInfo.id");
	define_zoom_option (c, "elementSetName");
	
	// Database Create, type must be set to create in ZOOM_package_send.
	// Database Drop, type must be set to drop in ZOOM_package_send.
	    
    cZoomPackage = c;
}

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


/* Document-class: ZOOM::Connection
 * The Connection object is a session with a target.
 */
static VALUE cZoomConnection;


static VALUE
rbz_connection_make (ZOOM_connection connection)
{
    return connection != NULL
        ? Data_Wrap_Struct (cZoomConnection,
                            NULL,
                            ZOOM_connection_destroy,
                            connection)
        : Qnil;
}

static ZOOM_connection
rbz_connection_get (VALUE obj)
{
    ZOOM_connection connection;
        
    Data_Get_Struct (obj, struct ZOOM_connection_p, connection);
    assert (connection != NULL);

    return connection;
}

#define RAISE_IF_FAILED(connection)                     \
    do {                                                \
        int error;                                      \
        const char *errmsg;                             \
        const char *addinfo;                            \
                                                        \
        error = ZOOM_connection_error (connection,      \
                                       &errmsg,         \
                                       &addinfo);       \
        if (error != 0)                                 \
            rb_raise (rb_eRuntimeError, "%s (%d) %s",   \
                      errmsg, error, addinfo);          \
    }                                                   \
    while (0)

void rbz_connection_check(VALUE obj)
{
	ZOOM_connection connection;
	
	connection = rbz_connection_get(obj);
  	RAISE_IF_FAILED (connection);
}


/*
 * call-seq: 
 * 	open(host, port=nil) { |conn| ... }
 *
 * host: hostname of the target to connect to.
 *
 * port: network port of the target to connect to.
 *
 * A convenience method that creates a new connection and attempts to 
 * establish a network connection to the given target, basically calling
 * ZOOM::Connection.new and ZOOM::Connection#connect.
 *  
 * If a block is given, then it will be called once the connection is 
 * established, passing a reference to the connection object as a parameter, 
 * and destroying the connection automatically at the end of the block.
 * With no block, this method just returns the connection object.
 *
 * Returns: a newly created ZOOM::Connection object.
 */
static VALUE
rbz_connection_open (int argc, VALUE *argv, VALUE self)
{
    VALUE host;
    VALUE port;
    ZOOM_connection connection;
    VALUE rb_connection;
    
    rb_scan_args (argc, argv, "11", &host, &port);

    connection = ZOOM_connection_new (RVAL2CSTR (host),
                                      NIL_P (port) ? 0 : FIX2INT (port));
    RAISE_IF_FAILED (connection);
    
    rb_connection = rbz_connection_make (connection);
    if (rb_block_given_p ()) {
        rb_yield(rb_connection);
        return Qnil;
    }
    return rb_connection;
}

/*
 * call-seq: new(options=nil)
 *
 * options: options for the connection, as a Hash object.
 *
 * Creates a new connection object, but does not establish a network connection
 * immediately, allowing you to specify options before (if given). You can 
 * thus establish the connection using ZOOM::Connection#connect.
 * 
 * Returns: a newly created ZOOM::Connection object.
 */
static VALUE
rbz_connection_new (int argc, VALUE *argv, VALUE self)
{
    ZOOM_options options;
    ZOOM_connection connection;
    VALUE rb_options;
    
    rb_scan_args (argc, argv, "01", &rb_options);

    if (NIL_P (rb_options))
        options = ZOOM_options_create ();
    else
        options = ruby_hash_to_zoom_options (rb_options);

    connection = ZOOM_connection_create (options);
    ZOOM_options_destroy (options);
    RAISE_IF_FAILED (connection);

    return rbz_connection_make (connection);
}

/*
 * call-seq:
 * 	connect(host, port=nil)
 *
 * host: hostname of the target to connect to.
 *
 * port: network port of the target to connect to.
 *
 * Establishes a network connection to the target specified by the given
 * arguments.  If no port is given, 210 will be used.  A colon in the host
 * string denotes the beginning of a port number.  If the host string includes
 * a slash, the following part specifies a database for the connection.
 *
 * You can also prefix the host string with a scheme followed by a colon.
 * The default scheme is tcp (Z39.50 protocol).  The scheme http selects SRW
 * over HTTP.
 *
 * This method raises an exception on error.
 *
 * Returns: self.
 */
static VALUE
rbz_connection_connect (int argc, VALUE *argv, VALUE self)
{
    ZOOM_connection connection;
    VALUE host;
    VALUE port;
    
    rb_scan_args (argc, argv, "11", &host, &port);
  
    connection = rbz_connection_get (self);
    ZOOM_connection_connect (connection, 
                             RVAL2CSTR (host), 
                             NIL_P (port) ? 0 : FIX2INT (port));
    RAISE_IF_FAILED (connection); 

    return self;
}

/*
 * call-seq:
 * 	set_option(key, value)
 *
 * key: the name of the option, as a string.
 *
 * value: the value of this option (as a string, integer or boolean).
 *
 * Sets an option on the connection.
 * 
 * Returns: self.
 */
static VALUE
rbz_connection_set_option (VALUE self, VALUE key, VALUE val)
{
    ZOOM_connection connection;
    
    connection = rbz_connection_get (self);
    ZOOM_connection_option_set (connection,
                                RVAL2CSTR (key),
                                RVAL2CSTR (rb_obj_as_string (val)));
    RAISE_IF_FAILED (connection); 
    
    return self;
}

/*
 * call-seq: 
 * 	get_option(key)
 *
 * key: the name of the option, as a string.
 *
 * Gets the value of a connection's option.
 * 
 * Returns: the value of the given option, as a string, integer or boolean.
 */
static VALUE
rbz_connection_get_option (VALUE self, VALUE key)
{
    ZOOM_connection connection;
    const char *value;
 
    connection = rbz_connection_get (self);
    value = ZOOM_connection_option_get (connection,
                                        RVAL2CSTR (key));

    return zoom_option_value_to_ruby_value (value);
}

/*
 * call-seq: 
 * 	search(criterion)
 *
 * criterion: the search criterion, either as a ZOOM::Query object or as a string,
 * representing a PQF query.
 *  
 * Searches the connection from the given criterion.  You can either create and
 * pass a reference to a ZOOM::Query object, or you can simply pass a string
 * that represents a PQF query.
 *
 * This method raises an exception on error.
 * 
 * Returns: a result set from the search, as a ZOOM::ResultSet object,
 * empty if no results were found.
 */
static VALUE
rbz_connection_search (VALUE self, VALUE criterion)
{
    ZOOM_connection connection;
    ZOOM_resultset resultset;

    connection = rbz_connection_get (self);
    if (TYPE (criterion) == T_STRING)
        resultset = ZOOM_connection_search_pqf (connection,
                                                RVAL2CSTR (criterion));
    else
        resultset = ZOOM_connection_search (connection,
                                            rbz_query_get (criterion));
    RAISE_IF_FAILED (connection); 
    assert (resultset != NULL);
  
    return rbz_resultset_make (resultset);
}


/*
 * Constructs a new extended services ZOOM::Package using this connections host information.
 *
 * Note: The Perl script passes this connections options if already set, otherwise constructs a new ZOOM::Option object. 
 * Currently this method always constructs a new ZOOM::Option object for each package.
 * 
 * Returns: a new ZOOM::Package object.
 */
static VALUE
rbz_connection_package(VALUE self)
{
  ZOOM_connection connection;
  ZOOM_options options;
  VALUE package;

  connection = rbz_connection_get (self);
  options = ZOOM_options_create ();
  package = rbz_package_make(connection, options);
  return package;
}


void
Init_zoom_connection (VALUE mZoom)
{
    VALUE c;

    c = rb_define_class_under (mZoom, "Connection", rb_cObject); 
    rb_define_singleton_method (c, "open", rbz_connection_open, -1);
    rb_define_singleton_method (c, "new", rbz_connection_new, -1);
    rb_define_method (c, "connect", rbz_connection_connect, -1);
    rb_define_method (c, "set_option", rbz_connection_set_option, 2);
    rb_define_method (c, "get_option", rbz_connection_get_option, 1);
    rb_define_method (c, "package", rbz_connection_package, 0);

    define_zoom_option (c, "implementationName");
    define_zoom_option (c, "user");
    define_zoom_option (c, "group");
    define_zoom_option (c, "password");
    define_zoom_option (c, "host");
    define_zoom_option (c, "proxy");
    define_zoom_option (c, "async");
    define_zoom_option (c, "maximumRecordSize");
    define_zoom_option (c, "preferredMessageSize");
    define_zoom_option (c, "lang");
    define_zoom_option (c, "charset");
    define_zoom_option (c, "serverImplementationId");
    define_zoom_option (c, "targetImplementationName");
    define_zoom_option (c, "serverImplementationVersion");
    define_zoom_option (c, "databaseName");
    define_zoom_option (c, "piggyback");
    define_zoom_option (c, "smallSetUpperBound");
    define_zoom_option (c, "largeSetLowerBound");
    define_zoom_option (c, "mediumSetPresentNumber");
    define_zoom_option (c, "smallSetElementSetName");
    define_zoom_option (c, "mediumSetElementSetName");

    /* herited from Zoom::ResultSet */
    define_zoom_option (c, "start");
    define_zoom_option (c, "count");
    define_zoom_option (c, "presentChunk");
    define_zoom_option (c, "elementSetName");
    define_zoom_option (c, "preferredRecordSyntax");
    define_zoom_option (c, "schema");
    define_zoom_option (c, "setname");
    
    rb_define_method (c, "search", rbz_connection_search, 1);
    
    cZoomConnection = c;
}

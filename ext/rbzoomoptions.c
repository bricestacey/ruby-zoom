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

#include <ctype.h>
#include "rbzoom.h"

ZOOM_options
ruby_hash_to_zoom_options (VALUE hash)
{
    ZOOM_options options;
    VALUE ary;
    VALUE pair;
    VALUE key;
    VALUE value;
    int i;

    options = ZOOM_options_create ();
    
    ary = rb_funcall (hash, rb_intern ("to_a"), 0);
    for (i = 0; i < RARRAY_LEN(ary); i++) {
        pair = RARRAY_PTR(ary)[i];
        key = RARRAY_PTR(pair)[0];
        value = RARRAY_PTR(pair)[1];
        
        switch (TYPE (value)) {
            case T_TRUE:
            case T_FALSE:
                ZOOM_options_set_int (options, 
                                      RVAL2CSTR (key),
                                      RVAL2CBOOL (value) ? 1 : 0);
                break;

            case T_FIXNUM:
                ZOOM_options_set_int (options, 
                                      RVAL2CSTR (key),
                                      FIX2INT (value));
                break;
                
            case T_STRING:
                ZOOM_options_set (options, 
                                  RVAL2CSTR (key), 
                                  RVAL2CSTR (value));
                break;

            default:
                rb_raise (rb_eArgError, "Unrecognized type");        
        }
    }        

    return options;
}

VALUE
zoom_option_value_to_ruby_value (const char *value)
{
    unsigned int i;

    if (value == NULL)
        return Qnil; 

    for (i = 0; i < strlen (value); i++)
        if (!isdigit (value [i]))
            return CSTR2RVAL (value);

    return INT2FIX (atoi (value));
}

void
define_zoom_option (VALUE klass, const char *option)
{
    char code [1024];
    char rubyname [128];
    char c;
    unsigned int i;
    unsigned int j;
   
    /* rubyfy the option name */
    for (i = 0, j = 0; i < strlen (option) && j < sizeof rubyname; i++, j++) {
        c = option [i];
        if (isupper (c)) {
            rubyname [j++] = '_';
            c = tolower (c);
        }
		else if (c == '-')
		{
			c = '_';
		}
		else if (c == '.')
		{
			c = '_';
		}
		
        rubyname [j] = c;
    }
    rubyname [j] = '\0';

    snprintf (code, sizeof code, 
              "def %s; get_option(\"%s\"); end\n"
              "def %s=(val); set_option(\"%s\", val); val; end\n"
              "def set_%s(val); set_option(\"%s\", val); end\n",
              rubyname, option,
              rubyname, option,
              rubyname, option);
             
    rb_funcall (klass, rb_intern ("module_eval"), 1, CSTR2RVAL (code));
}

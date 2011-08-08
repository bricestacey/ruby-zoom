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


void
Init_zoom (void)
{
    VALUE mZoom;

    mZoom = rb_define_module ("ZOOM");

    Init_zoom_connection (mZoom);
    Init_zoom_query (mZoom);
    Init_zoom_resultset (mZoom);
    Init_zoom_record (mZoom);
    Init_zoom_package (mZoom);
}

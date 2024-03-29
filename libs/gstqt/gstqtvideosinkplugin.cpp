/*
    Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies). <qt-info@nokia.com>
    Copyright (C) 2011 Collabora Ltd. <info@collabora.com>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 2.1
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gstqtvideosinkplugin.h"
#include "gstqtvideosink.h"
//djs #include "gstqtglvideosink.h"
#include "gstqwidgetvideosink.h"

//djs #if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
//djs # include "gstqtquick2videosink.h"
//djs #endif

GST_DEBUG_CATEGORY(gst_qt_video_sink_debug);

/* entry point to initialize the plug-in */
static gboolean plugin_init(GstPlugin *plugin)
{
    GST_DEBUG_CATEGORY_INIT(gst_qt_video_sink_debug,
                            G_STRINGIFY(QTVIDEOSINK_NAME), 0,
                            "Debug category for GstQtVideoSink");

    if(!gst_element_register(plugin, G_STRINGIFY(HABIT2_PLUGIN_NAME),
                GST_RANK_NONE, GST_TYPE_QT_VIDEO_SINK)) {
        GST_ERROR("Failed to register " G_STRINGIFY(HABIT2_PLUGIN_NAME));
        return FALSE;
    }
//#ifndef GST_QT_VIDEO_SINK_NO_OPENGL
//    if(!gst_element_register(plugin, G_STRINGIFY(QTGLVIDEOSINK_NAME),
//                GST_RANK_NONE, GST_TYPE_QT_GL_VIDEO_SINK)) {
//        GST_ERROR("Failed to register " G_STRINGIFY(QTGLVIDEOSINK_NAME));
//        return FALSE;
//    }
//#endif
    if(!gst_element_register(plugin, G_STRINGIFY(QWIDGETVIDEOSINK_NAME),
                GST_RANK_NONE, GST_TYPE_QWIDGET_VIDEO_SINK)) {
        GST_ERROR("Failed to register " G_STRINGIFY(QWIDGETVIDEOSINK_NAME));
        return FALSE;
    }

//#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
//    if (!gst_element_register(plugin, "qtquick2videosink",
//                GST_RANK_NONE, GST_TYPE_QT_QUICK2_VIDEO_SINK)) {
//        GST_ERROR("Failed to register qtquick2videosink");
//        return FALSE;
//    }
//#endif

    return TRUE;
}

GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    HABIT2_PLUGIN_NAME,
    "A video sink that can draw on any Qt surface",
    plugin_init,
    PACKAGE_VERSION,
    "LGPL",
    PACKAGE_NAME,
    PACKAGE_ORIGIN
)

/* app_search.c
 *
 * Copyright 2020 Yuri Edward
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "app_search.h"
#include "utils.h"

#define PROP_BASE_DIRECTORY 0

typedef struct {
    GString *base_directory;
    gboolean strict;
} AppSearchPrivate;

static GParamSpec *obj_properties[1] = { NULL };

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE(AppSearch, app_search, G_TYPE_OBJECT);

static gchar *find_impl(AppSearch *self, const gchar *appname, const gboolean strict)
{
    AppSearchPrivate *private = app_search_get_instance_private(self);
    const gchar *fname = NULL;
    GDir *dir = g_dir_open(private->base_directory->str, 0, NULL);
    gchar *res;

    if (dir == NULL)
        return NULL;
    while ((fname = g_dir_read_name(dir)) != NULL)
    {
        res = app_search_try_read_command(self, appname, fname, strict);
        if (res != NULL)
        {
            g_dir_close(dir);
            return res;
        }
    }
    g_dir_close(dir);
    return NULL;
}

static void app_search_set_property(GObject *obj, guint prop, const GValue *value, GParamSpec *pspec)
{
    AppSearchPrivate *self = app_search_get_instance_private(APP_SEARCH(obj));

    switch (prop)
    {
    case PROP_BASE_DIRECTORY:
        if (g_value_get_string(value) != NULL)
        {
            self->base_directory = g_string_new(g_value_get_string(value));
            g_set_string(&self->base_directory, g_string_replace(self->base_directory, "~", g_get_home_dir()));
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop, pspec);
        break;
    }
}

static void app_search_get_property(GObject *obj, guint prop, GValue *value, GParamSpec *pspec)
{
    AppSearchPrivate *self = app_search_get_instance_private(APP_SEARCH(obj));

    switch (prop)
    {
    case PROP_BASE_DIRECTORY:
        g_value_set_static_string(value, self->base_directory->str);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(obj, prop, pspec);
        break;
    }
}

static void app_search_class_init(AppSearchClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS(class);

    object_class->set_property = app_search_set_property;
    object_class->get_property = app_search_get_property;
    obj_properties[PROP_BASE_DIRECTORY] = g_param_spec_string(
        "base_directory",
        "BaseDirectory",
        "Base directory to search in.",
        NULL,
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE
    );
    g_object_class_install_properties(object_class, 1, obj_properties);
    class->try_read_command = NULL;
    class->find = find_impl;
}

static void app_search_init(AppSearch *self)
{
    AppSearchPrivate *private = app_search_get_instance_private(self);

    private->strict = FALSE;
    private->base_directory = NULL;
}

gboolean app_search_is_strict(AppSearch *self)
{
    AppSearchPrivate *private = app_search_get_instance_private(self);

    return private->strict;
}

gchar *app_search_find(AppSearch *self, const gchar *appname, const gboolean strict)
{
    return APP_SEARCH_CLASS(self)->find(self, appname, strict);
}

gchar *app_search_try_read_command(AppSearch *self, const gchar *appname, const gchar *entry_name, const gboolean strict)
{
    return APP_SEARCH_CLASS(self)->try_read_command(self, appname, entry_name, strict);
}


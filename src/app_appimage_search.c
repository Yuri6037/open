/* app_static_search.c
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

#include "app_appimage_search.h"
#include "utils.h"

struct _AppAppimageSearch
{
    AppSearch parent;
};

G_DEFINE_TYPE(AppAppimageSearch, app_appimage_search, APP_TYPE_SEARCH);

static gchar *app_appimage_search_extract_name(const gchar *fname)
{
    gchar **tab = g_strsplit(fname, "-", INT_MAX);
    gchar *res;

    if (tab == NULL)
        return g_strdup(fname);
    res = g_strdup(tab[0]);
    g_strfreev(tab);
    return res;
}

static gchar *app_appimage_search_try_read_command(AppSearch *self, const gchar *appname, const gchar *entry_name, G_GNUC_UNUSED const gboolean strict)
{
    g_autofree gchar *base_directory = NULL;
    g_autofree gchar *path = NULL;
    g_autofree gchar *test_app_name = NULL;

    if (!g_str_has_suffix(entry_name, ".AppImage") && !g_str_has_suffix(entry_name, ".appimage"))
        return NULL;
    test_app_name = app_appimage_search_extract_name(entry_name);
    if (g_fixed_strcasecmp(appname, test_app_name) == 0)
    {
        g_object_get(self, "base_directory", &base_directory, NULL);
        path = g_strjoin("/", base_directory, entry_name, NULL);
        return g_strdup(path);
    }
    return NULL;
}

static void app_appimage_search_class_init(AppAppimageSearchClass *class)
{
    APP_SEARCH_CLASS(class)->try_read_command = app_appimage_search_try_read_command;
}

static void app_appimage_search_init(G_GNUC_UNUSED AppAppimageSearch *self)
{
}

AppAppimageSearch *app_appimage_search_new(const gchar *base_directory)
{
    return g_object_new(APP_TYPE_APPIMAGE_SEARCH, "base_directory", base_directory, NULL);
}

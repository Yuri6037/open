/* app_desktop_search.c
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

#include "app_desktop_search.h"
#include "utils.h"

struct _AppDesktopSearch
{
};

G_DEFINE_TYPE(AppDesktopSearch, app_desktop_search, APP_TYPE_SEARCH);

static gchar *app_desktop_search_extract_name(const gchar *fname)
{
    gchar **tab = g_strsplit(fname, ".", INT_MAX);
    gchar *res;
    guint len;

    if (tab == NULL)
        return g_strdup(fname);
    len = g_strv_length(tab);
    res = g_strdup(tab[len - 2]);
    g_strfreev(tab);
    return res;
}

static gchar *app_desktop_search_gen_app_command(DesktopFile *desktop)
{
    gchar *exec = desktop_to_command(desktop);

    free_desktop_file(desktop);
    return exec;
}

static DesktopFile *app_desktop_search_check_app_name(const gchar *appname, const gchar *path, const gchar *test_app_name, const gboolean strict)
{
    DesktopFile *desktop;

    if (strict)
        return g_fixed_strcasecmp(appname, test_app_name) == 0 ? parse_desktop_file(path) : NULL;
    else
    {
        if (g_fixed_strcasecmp(appname, test_app_name) == 0)
            return parse_desktop_file(path);
        desktop = parse_desktop_file(path);
        if (desktop == NULL)
            return FALSE;
        if (g_fixed_strcasecmp(appname, desktop->name) == 0)
            return desktop;
        else
        {
            free_desktop_file(desktop);
            return NULL;
        }
    }
}

static gchar *app_desktop_search_try_read_command(AppSearch *self, const gchar *appname, const gchar *entry_name, const gboolean strict)
{
    g_autofree gchar *test_app_name;
    g_autofree gchar *path;
    const gchar *base_directory;
    DesktopFile *desktop;
    gchar *command;

    if (!g_str_has_suffix(entry_name, ".desktop"))
        return NULL;
    g_object_get(G_OBJECT(self), "base_directory", &base_directory, NULL);
    test_app_name = app_desktop_search_extract_name(entry_name);
    path = g_strjoin("/", base_directory, entry_name, NULL);
    desktop = app_desktop_search_check_app_name(appname, path, test_app_name, strict);
    if (desktop == NULL)
        return NULL;
    g_debug("May have found desktop application '%s'", entry_name);
    command = app_desktop_search_gen_app_command(desktop);
    return command;
}

static void app_desktop_search_class_init(AppDesktopSearchClass *class)
{
    APP_SEARCH_CLASS(class)->try_read_command = app_desktop_search_try_read_command;
}

static void app_desktop_search_init()
{
}

AppDesktopSearch *app_desktop_search_new(const gchar *base_directory)
{
    return g_object_new(APP_TYPE_DESKTOP_SEARCH, "base_directory", base_directory);
}


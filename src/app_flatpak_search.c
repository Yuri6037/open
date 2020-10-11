/* app_flatpak_search.c
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

#include "app_flatpak_search.h"
#include "utils.h"

struct _AppFlatpakSearch
{
    AppSearch parent;
};

G_DEFINE_TYPE(AppFlatpakSearch, app_flatpak_search, APP_TYPE_SEARCH);

/* Flatpak name hack begin */
static gboolean app_flatpak_search_check_app_name(const gchar *appname, const gchar *entry_name, const gboolean strict)
{
    gchar **tab = g_strsplit(entry_name, ".", INT_MAX);
    gboolean test = FALSE;
    guint len;

    if (tab == NULL)
        return FALSE;
    len = g_strv_length(tab);
    if (g_fixed_strcasecmp(appname, tab[len - 1]) == 0)
        test = TRUE;
    else if (!strict)
        test = g_fixed_strcasecmp(appname, tab[len - 2]) == 0;
    g_strfreev(tab);
    return test;
}
/* Flatpak name hack end */

static gchar *app_flatpak_search_try_read_command(AppSearch *self, const gchar *appname, const gchar *entry_name, const gboolean strict)
{
    g_autofree gchar *base_directory = NULL;
    gchar *command = NULL;

    g_object_get(G_OBJECT(self), "base_directory", &base_directory, NULL);
    if (app_flatpak_search_check_app_name(appname, entry_name, strict))
        command = g_strjoin("/", base_directory, entry_name, NULL);
    return command;
}

static void app_flatpak_search_class_init(AppFlatpakSearchClass *class)
{
    APP_SEARCH_CLASS(class)->try_read_command = app_flatpak_search_try_read_command;
}

static void app_flatpak_search_init(G_GNUC_UNUSED AppFlatpakSearch *self)
{
}

AppFlatpakSearch *app_flatpak_search_new(const gchar *base_directory)
{
    return g_object_new(APP_TYPE_FLATPAK_SEARCH, "base_directory", base_directory, NULL);
}


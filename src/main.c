/* main.c
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

#include "open-config.h"

#include "utils.h"
#include "app_search.h"
#include "app_desktop_search.h"
#include "app_flatpak_search.h"
#include "app_appimage_search.h"

#include <gio/gio.h>
#include <stdlib.h>

#define N_SEARCH 6

static void create_search_array(AppSearch *arr[N_SEARCH])
{
    arr[0] = APP_SEARCH(app_desktop_search_new("."));
    arr[1] = APP_SEARCH(app_appimage_search_new("."));
    arr[2] = APP_SEARCH(app_desktop_search_new("/usr/share/applications/"));
    arr[3] = APP_SEARCH(app_flatpak_search_new("/var/lib/flatpak/exports/bin/"));
    arr[4] = APP_SEARCH(app_desktop_search_new("~/.local/share/applications/"));
    arr[5] = APP_SEARCH(app_flatpak_search_new("~/.local/share/flatpak/exports/bin/"));
}

static void destroy_search_array(AppSearch *arr[N_SEARCH])
{
    for (gsize i = 0; i != N_SEARCH; ++i)
        g_object_unref(arr[i]);
}

static gchar *locate_app_bin(const gboolean strict, const gchar *appname)
{
    gchar *res;
    AppSearch *arr[N_SEARCH];

    create_search_array(arr);
    for (gsize i = 0; i != N_SEARCH; ++i)
    {
        res = app_search_find(arr[i], appname, strict);
        if (res != NULL)
            return res;
    }
    destroy_search_array(arr);
    return NULL;
}

static gboolean run_command(const gchar *cmd, gchar **data)
{
    g_autoptr(GString) str = g_string_new("");
    guint len = g_strv_length(data);
    gchar *argv[4] = { "bash", "-c", NULL, NULL };
    GPid pid;
    GError *err = NULL;

    str = g_string_append(str, cmd);
    for (guint i = 1; i != len; ++i)
    {
        str = g_string_append_c(str, ' ');
        str = g_string_append(str, data[i]);
    }
    g_debug("Running command '%s'...", str->str);
    argv[2] = str->str;
    g_spawn_async(NULL, argv, NULL, G_SPAWN_SEARCH_PATH | G_SPAWN_STDOUT_TO_DEV_NULL | G_SPAWN_STDERR_TO_DEV_NULL, NULL, NULL, &pid, &err);
    if (err != NULL)
    {
        g_printerr("%s\n", err->message);
        return FALSE;
    }
    return TRUE;
}

gint main (gint argc, gchar *argv[])
{
    g_autoptr(GOptionContext) context = NULL;
    g_autoptr(GError) error = NULL;
    g_autofree gchar *bin = NULL;
    gboolean version = FALSE;
    gboolean strict = FALSE;
    gchar **data = NULL;
    gint res = EXIT_SUCCESS;
    GOptionEntry main_entries[] = {
        { "version", 'v', 0, G_OPTION_ARG_NONE, &version, "Show program version", NULL },
        { "strict", 's', 0, G_OPTION_ARG_NONE, &strict, "Strict mode (skips checking of desktop Name entry)", NULL },
        { G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_STRING_ARRAY, &data, "Application name and arguments", "<application name> [arguments]" },
        { NULL, 0, 0, G_OPTION_ARG_NONE, NULL, NULL, NULL }
    };

    context = g_option_context_new("- Opens a desktop or flatpak application from command line");
    g_option_context_add_main_entries(context, main_entries, NULL);
    if (!g_option_context_parse(context, &argc, &argv, &error))
    {
        g_printerr("%s\n", error->message);
        return EXIT_FAILURE;
    }
    else if (version)
    {
        g_printerr("%s\n", PACKAGE_VERSION);
        return EXIT_SUCCESS;
    }
    else if (data == NULL)
    {
        g_printerr("Please specify application name, or use -h for help\n");
        return EXIT_FAILURE;
    }
    bin = locate_app_bin(strict, data[0]);
    if (bin == NULL)
    {
        g_printerr("Could not find matching application for name '%s'\n", data[0]);
        return EXIT_FAILURE;
    }
    g_debug("Found application command: '%s'", bin);
    res = run_command(bin, data) ? EXIT_SUCCESS : EXIT_FAILURE;
    g_strfreev(data);
    return res;
}

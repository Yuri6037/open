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

#include <gio/gio.h>
#include <stdlib.h>

/*static const gchar *SEARCH_PATHS[] = {
    "/usr/share/applications/",
    "~/.local/share/applications/",
    "~/.local/share/flatpak/exports/bin/",
    "/var/lib/flatpak/exports/bin/"
};

static gchar *extract_name(const gchar *fname)
{
    gchar **tab = g_strsplit(fname, ".", INT_MAX);
    gchar *res;
    guint len;

    if (tab == NULL)
        return g_strdup(fname);
    len = g_strv_length(tab);
    if (g_str_has_suffix(fname, ".desktop"))
        res = g_strdup(tab[len - 2]);
    else
        res = g_strdup(tab[len - 1]);
    g_strfreev(tab);
    return res;
}

static gchar *attempt_gen_app_bin(const gchar *folder_base, const gchar *fname, DesktopFile *desktop)
{
    g_autofree gchar *path = NULL;
    gchar *exec;

    if (desktop != NULL)
        exec = desktop_to_command(desktop);
    else
    {
        path = g_strjoin("/", folder_base, fname, NULL);
        if (g_str_has_suffix(path, ".desktop"))
        {
            desktop = parse_desktop_file(path);
            exec = desktop_to_command(desktop);
        }
        else
        {
            free_desktop_file(desktop);
            return g_strdup(path);
        }
    }
    free_desktop_file(desktop);
    return exec;
}

static gint g_fixed_strcasecmp(const gchar *s1, const gchar *s2)
{
    gchar *sf1 = g_utf8_casefold(s1, -1);
    gchar *sf2 = g_utf8_casefold(s2, -1);
    gint res;

    g_assert(sf1 != NULL);
    g_assert(sf2 != NULL);
    res = strcmp(sf1, sf2);
    g_free(sf1);
    g_free(sf2);
    return res;
}

static gboolean check_app_name(const gboolean strict, const gchar *base_folder, const gchar *fname, const gchar *test_app_name, const gchar *appname, DesktopFile **desktop)
{
    g_autofree gchar *path = NULL;

    if (strict)
        return g_fixed_strcasecmp(appname, test_app_name) == 0;
    else
    {
        if (g_fixed_strcasecmp(appname, test_app_name) == 0)
            return TRUE;
        if (g_str_has_suffix(fname, ".desktop"))
        {
            path = g_strjoin("/", base_folder, fname, NULL);
            *desktop = parse_desktop_file(path);
            if (*desktop == NULL)
                return FALSE;
            if (g_fixed_strcasecmp(appname, (*desktop)->name) == 0)
                return TRUE;
            else
            {
                free_desktop_file(*desktop);
                return FALSE;
            }
        }
        else
            return FALSE;
    }
}

static gchar *check_folder_entry(const gboolean strict, const gchar *fname, const gchar *base_folder, const gchar *appname)
{
    g_autofree gchar *test_app_name = extract_name(fname);
    DesktopFile *desktop = NULL;
    gchar *bin;
    gboolean test = check_app_name(strict, base_folder, fname, test_app_name, appname, &desktop);

    if (!test)
        return NULL;
    g_debug("May have found application '%s'", fname);
    bin = attempt_gen_app_bin(base_folder, fname, desktop);
    if (bin != NULL)
        return bin;
    return NULL;
}

static gchar *search_folder(const gboolean strict, const gchar *folder, const gchar *appname)
{
    const char *user_home = g_get_home_dir();
    g_autoptr(GString) base_folder = g_string_new(folder);
    g_autoptr(GString) actual_folder = g_string_replace(base_folder, "~", user_home);
    const gchar *fname = NULL;
    GDir *dir = g_dir_open(actual_folder->str, 0, NULL);
    gchar *res;

    if (dir == NULL)
        return NULL;
    while ((fname = g_dir_read_name(dir)) != NULL)
    {
        res = check_folder_entry(strict, fname, actual_folder->str, appname);
        if (res != NULL)
        {
            g_dir_close(dir);
            return res;
        }
    }
    g_dir_close(dir);
    return NULL;
}*/

/*
    "/usr/share/applications/",
    "~/.local/share/applications/",
    "~/.local/share/flatpak/exports/bin/",
    "/var/lib/flatpak/exports/bin/"
*/

static void init_search_array(AppSearch *arr[4])
{
    arr[0] = APP_SEARCH(app_desktop_search_new("/usr/share/applications/"));
    arr[1] = APP_SEARCH(app_flatpak_search_new("/var/lib/flatpak/exports/bin/"));
    arr[2] = APP_SEARCH(app_desktop_search_new("~/.local/share/applications/"));
    arr[3] = APP_SEARCH(app_flatpak_search_new("~/.local/share/flatpak/exports/bin/"));
}

static gchar *locate_app_bin(const gboolean strict, const gchar *appname)
{
    gchar *res;
    AppSearch *arr[4];

    init_search_array(arr);
    for (gsize i = 0; i != sizeof(arr) / sizeof(AppSearch *); ++i)
    {
        res = app_search_find(arr[i], appname, strict);
        if (res != NULL)
            return res;
    }
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

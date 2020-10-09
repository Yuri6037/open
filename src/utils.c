/* utils.c
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

#include <gio/gio.h>
#include "utils.h"

static DesktopFile *alloc_desktop()
{
    DesktopFile *res = g_malloc(sizeof(DesktopFile));

    res->exec = NULL;
    res->name = NULL;
    res->icon = NULL;
    res->location = NULL;
    return res;
}

DesktopFile *parse_desktop_file(const gchar *path)
{
    g_autoptr(GFile) file = g_file_new_for_path(path);
    g_autoptr(GInputStream) fstream = G_INPUT_STREAM(g_file_read(file, NULL, NULL));
    g_autoptr(GDataInputStream) stream = g_data_input_stream_new(fstream);
    g_autofree gchar *line = NULL;
    DesktopFile *res = alloc_desktop();
    gboolean flag = FALSE;
    gchar **tab;

    if (fstream == NULL)
    {
        free_desktop_file(res);
        return NULL;
    }
    while ((line = g_data_input_stream_read_line_utf8(stream, NULL, NULL, NULL)) != NULL)
    {
        if (line[0] == '[')
        {
            if (!flag)
                flag = TRUE;
            else
                break;
        }
        if (g_str_has_prefix(line, "NoDisplay") && g_str_has_suffix(line, "true"))
        {
            free_desktop_file(res);
            return NULL;
        }
        else if (g_str_has_prefix(line, "Exec"))
        {
            tab = g_strsplit(line, "=", INT_MAX);
            g_assert(tab != NULL);
            res->exec = g_strdup(tab[1]);
            g_strfreev(tab);
        }
        else if (g_str_has_prefix(line, "Name"))
        {
            tab = g_strsplit(line, "=", INT_MAX);
            g_assert(tab != NULL);
            res->name = g_strdup(tab[1]);
            g_strfreev(tab);
        }
        else if (g_str_has_prefix(line, "Icon"))
        {
            tab = g_strsplit(line, "=", INT_MAX);
            g_assert(tab != NULL);
            res->icon = g_strdup(tab[1]);
            g_strfreev(tab);
        }
        g_free(line);
    }
    if (res->exec == NULL)
    {
        free_desktop_file(res);
        return NULL;
    }
    res->location = g_strdup(path);
    return res;
}

void free_desktop_file(DesktopFile *desktop)
{
    if (desktop == NULL)
        return;
    g_free(desktop->location);
    g_free(desktop->name);
    g_free(desktop->icon);
    g_free(desktop->exec);
    g_free(desktop);
}

GString *g_string_replace(GString *this, const gchar *search, const gchar *replace)
{
    gsize size = strlen(search);
    GString *res = g_string_sized_new(this->len);

    for (gsize i = 0; i < this->len; ++i)
    {
        if (this->str[i] == search[0] && strstr(this->str + i, search))
        {
            res = g_string_append(res, replace);
            i += size - 1;
        }
        else
            res = g_string_append_c(res, this->str[i]);
    }
    return res;
}

gboolean g_set_string(GString **object_ptr, GString *new_object)
{
    if (*object_ptr == new_object)
        return FALSE;
    g_string_free(*object_ptr, TRUE);
    *object_ptr = new_object;
    return TRUE;
}

gchar *desktop_to_command(DesktopFile *desktop)
{
    g_autoptr(GString) str = g_string_new(desktop->exec);
    g_autofree gchar *icon = g_strconcat("--icon ", desktop->icon, NULL);

    g_set_string(&str, g_string_replace(str, "%f", ""));
    g_set_string(&str, g_string_replace(str, "%F", ""));
    g_set_string(&str, g_string_replace(str, "%u", ""));
    g_set_string(&str, g_string_replace(str, "%U", ""));
    g_set_string(&str, g_string_replace(str, "%d", ""));
    g_set_string(&str, g_string_replace(str, "%D", ""));
    g_set_string(&str, g_string_replace(str, "%n", ""));
    g_set_string(&str, g_string_replace(str, "%N", ""));
    g_set_string(&str, g_string_replace(str, "%v", ""));
    g_set_string(&str, g_string_replace(str, "%m", ""));
    g_set_string(&str, g_string_replace(str, "%k", desktop->location));
    g_set_string(&str, g_string_replace(str, "%c", desktop->name));
    g_set_string(&str, g_string_replace(str, "%i", icon));
    return g_strdup(str->str);
}

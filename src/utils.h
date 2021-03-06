/* utils.h
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

#pragma once
#include <glib.h>

struct _DesktopFile
{
    gchar *exec;
    gchar *name;
    gchar *icon;
    gchar *location;
};

typedef struct _DesktopFile DesktopFile;

/* Mini .desktop file parser (https://specifications.freedesktop.org/desktop-entry-spec/desktop-entry-spec-latest.html) */
DesktopFile *parse_desktop_file(const gchar *path);
void free_desktop_file(DesktopFile *desktop);
gchar *desktop_to_command(DesktopFile *desktop);

/* String utilities */
GString *g_string_replace(GString *this, const gchar *search, const gchar *replace);
gboolean g_set_string(GString **object_ptr, GString *new_object);
gint g_fixed_strcasecmp(const gchar *s1, const gchar *s2);


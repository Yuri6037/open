/* app_search.h
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
#include <glib-object.h>

G_BEGIN_DECLS

#define APP_TYPE_SEARCH (app_search_get_type())

G_DECLARE_DERIVABLE_TYPE(AppSearch, app_search, APP, SEARCH, GObject);

struct _AppSearchClass
{
    GObjectClass parent_class;
    gchar *(*find) (AppSearch *self, const gchar *appname, const gboolean strict);
    gchar *(*try_read_command) (AppSearch *self, const gchar *appname, const gchar *entry_name, const gboolean strict);
};

gchar *app_search_find(AppSearch *self, const gchar *appname, const gboolean strict);
gchar *app_search_try_read_command(AppSearch *self, const gchar *appname, const gchar *entry_name, const gboolean strict);

G_END_DECLS


/* app_static_search.h
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
#include "app_search.h"

G_BEGIN_DECLS

#define APP_TYPE_APPIMAGE_SEARCH app_appimage_search_get_type()

G_DECLARE_FINAL_TYPE(AppAppimageSearch, app_appimage_search, APP, APPIMAGE_SEARCH, AppSearch);

AppAppimageSearch *app_appimage_search_new(const gchar *base_directory);

G_END_DECLS


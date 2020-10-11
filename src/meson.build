open_sources = [
    'main.c',
    'utils.h',
    'utils.c',
    'app_search.h',
    'app_search.c',
    'app_desktop_search.h',
    'app_desktop_search.c',
    'app_flatpak_search.h',
    'app_flatpak_search.c',
    'app_appimage_search.h',
    'app_appimage_search.c'
]

open_deps = [
    dependency('glib-2.0'),
    dependency('gio-2.0')
]

executable('open', open_sources,
    dependencies: open_deps,
    install: true,
)

xcb_dep = dependency('xcb', required: false)

if build_xnest
    if xcb_dep.found()
        parse_geometry = executable('parse-geometry',
                ['parse-geometry.c', '../../hw/xnest/parse-geometry.c'],
                dependencies: [xcb_dep],
                include_directories: ['../../include'])
        test('parse-geometry', parse_geometry)
    endif
endif

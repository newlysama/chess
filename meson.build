project(
  'better-chess-engine',
  'cpp',
  version : '0.1.0',
  default_options: [
    'cpp_std=c++23',
    'warning_level=3',
    'default_library=static',
    'buildtype=plain',
    ]
)

#------------------------------#
#        PROJECT MODE          #
#------------------------------#

mode            = get_option('mode')
generate_magics = get_option('generate_magics')
console         = get_option('console')

tbb_dep = dependency(
  'tbb',
  required: true
)

# Global project arguments
add_project_arguments(
  '-DPROJECT_ROOT="' + meson.project_source_root() + '"',
  language: 'cpp'
)

if mode == 'debug'
  add_project_arguments(
    '-DBUILD_DEBUG',
    '-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE',
    '-O0',
    '-g',
    '-fsanitize=address',
    '-fno-omit-frame-pointer',
    language: 'cpp'
  )

  add_project_link_arguments(
    '-fsanitize=address',
    language: 'cpp'
  )

elif mode == 'profile'
  add_project_arguments(
    '-DBUILD_PROFILE',
    '-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE',
    '-O2',
    '-g',
    '-march=native',
    '-mtune=native',
    '-funroll-loops',
    language: 'cpp'
  )

elif mode == 'release'
  add_project_arguments(
    '-DBUILD_RELEASE',
    '-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_WARN',
    '-O3',
    '-march=native',
    '-mtune=native',
    '-funroll-loops',
    '-fomit-frame-pointer',
    language: 'cpp'
  )

elif mode == 'test'
  add_project_arguments(
    '-DBUILD_TEST',
    '-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_WARN',
    '-O2',
    language: 'cpp'
  )
endif

if generate_magics == true
  if console == true or mode == 'test'
    error('Cannot use generate_magics with console option or test build mode.')
  else
    add_project_arguments(
      '-DGENERATE_MAGICS',
      language: 'cpp'
    )
  endif
endif

if console == true
  if generate_magics == true
    error('Cannot use console with generate_magic option.')
  else
    add_project_arguments(
      '-DPLAY_CONSOLE',
      language: 'cpp'
    )
  endif
endif

inc_src = include_directories('src')


subdir('src/')

if mode == 'test'
  subdir('tests')
endif
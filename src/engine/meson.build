#----- src/engine/meson.build -----#


#- Subdirs -#
subdir('board/')
subdir('game')

#- Depedencies -#
engine_deps = [logging_dep, board_dep, game_dep]

# If we want to generate magic numbers, include magics_generator
if generate_magics == true
    subdir('magics_generator/')
    engine_deps += magics_generator_dep
endif

#- Engine Library -#
engine_lib = library(
  'engine',
  include_directories: [inc_src],
  dependencies: engine_deps
)

#- Exportable Module -#
engine_dep = declare_dependency(
  link_with: engine_lib,
  include_directories: [inc_src],
  dependencies: engine_deps
)

#----- src/main/meson.build -----#

#- Source File -#
app_src = files('main.cpp')

#- Depedencies -#
main_deps = [logging_dep, engine_dep, tbb_dep]

# If we compile with -Dconsole=true, compile the console_runner
if console == true
  subdir('console_runner/')
  main_deps += console_runner_dep
endif

#- Chess Executable -#
executable(
  'chess',
  app_src,
  include_directories: [inc_src],
  dependencies: main_deps
)

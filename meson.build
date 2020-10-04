project('riddle',
        'cpp',
        default_options : ['cpp_std=gnu++17'])

riddle_cpp_args = [
  '-Wall',
  '-Wextra',
  '-Wconversion',
  '-Wcast-align',
  '-Wunused',
  '-Wno-missing-braces',
  '-Wno-missing-field-initializers',
  '-Wno-sign-conversion',
  '-fsanitize=address',
]

riddle_c_args = [
  '-fsanitize=address',
]

riddle_link_args = [
  '-fsanitize=address',
]

fmt_dep = dependency('fmt', fallback : ['fmt', 'fmt_dep'])

subdir('common')
subdir('lexer')
subdir('ast')
subdir('parser')

if get_option('build-tests')
  subdir('tests')
endif

if get_option('build-demos')
  subdir('demo')
endif

executable('riddle', [ './riddle.cpp' ],
          cpp_args : riddle_cpp_args,
          c_args : riddle_c_args,
          link_args : riddle_link_args,
          dependencies : [ visitors_dep, parser_dep, ast_dep, fmt_dep, lexer_dep, common_dep ])

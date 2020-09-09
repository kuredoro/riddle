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
  executable('lexer_demo', [ 'demo/lexer_demo.cpp' ],
            cpp_args : riddle_cpp_args,
            dependencies : [ fmt_dep, lexer_dep, common_dep ])
endif

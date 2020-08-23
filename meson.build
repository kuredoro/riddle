project('riddle',
        'cpp',
        default_options : ['cpp_std=c++17'])

fmt_dep = dependency('fmt', fallback : ['fmt', 'fmt_dep'])

subdir('common')
subdir('lexer')
subdir('parser')

if get_option('build-tests')
  subdir('tests')
endif

if get_option('build-demos')
  subdir('demo')
endif

project('riddle',
        'cpp',
        default_options : ['cpp_std=c++17'])

subdir('lexer')
subdir('parser')
if get_option('build-tests')
  subdir('tests')
endif

if get_option('build-demos')
  executable('lexer_demo', 'demo/lexer_demo.cpp',
             dependencies : lexer_dep)
endif

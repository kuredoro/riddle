project('riddle',
        'cpp',
        default_options : ['cpp_std=c++2a'])

subdir('lexer')
subdir('parser')
if get_option('build-tests')
  subdir('tests')
endif

executable('lexer_demo', 'demo/lexer_demo.cpp',
           dependencies : lexer_dep)

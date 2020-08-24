incdir = include_directories('.')
liblexer = static_library('lexer', 
                         sources : ['impl/lexer.cpp'], 
                         include_directories : incdir,
                         dependencies : [ fmt_dep, common_dep ],
                         install : true)


lexer_dep = declare_dependency(include_directories : '.',
                               link_with : liblexer)

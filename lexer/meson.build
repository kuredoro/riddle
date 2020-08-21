incdir = include_directories('.')
liblexer = library('lexer', 
                   include_directories : incdir,
                   sources : ['impl/lexer.cpp'], 
                   install : true)


lexer_dep = declare_dependency(include_directories : '.',
                               link_with : liblexer)

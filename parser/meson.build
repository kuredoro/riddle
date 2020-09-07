incdir = include_directories('.')
libparser = static_library('parser', 
                           sources : ['impl/parser.cpp'],
                           cpp_args : riddle_cpp_args,
                           dependencies : [ lexer_dep, common_dep ],
                           install : true)


parser_dep = declare_dependency(include_directories : '.',
                                link_with : libparser)

incdir = include_directories('.')
libparser = library('parser', 
                    include_directories : incdir,
                    cpp_args : riddle_cpp_args,
                    sources : ['impl/parser.cpp'], 
                    install : true)


parser_dep = declare_dependency(include_directories : '.',
                                link_with : libparser)

incdir = include_directories('.')
libparser = library('parser', 
                    include_directories : incdir,
                    sources : ['impl/parser.cpp'], 
                    install : true)


parser_dep = declare_dependency(include_directories : '.',
                                link_with : libparser)

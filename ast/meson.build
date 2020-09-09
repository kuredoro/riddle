incdir = include_directories('.')
libast = static_library('ast',
                        include_directories : incdir,
                        cpp_args : riddle_cpp_args,
                        dependencies: [ lexer_dep ],
                        install : true)


ast_dep = declare_dependency(include_directories : '.',
                             link_with : libast)

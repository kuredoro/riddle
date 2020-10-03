fmt_dep = dependency('fmt', fallback : [ 'fmt', 'fmt_dep' ])

incdir = include_directories('.')
libcommon = static_library('common',
                          include_directories : incdir,
                          cpp_args : riddle_cpp_args,
                          c_args : riddle_c_args,
                          link_args : riddle_link_args,
                          sources : [ 'impl/strings.cpp' ],
                          dependencies: [ fmt_dep ],
                          install : true)


common_dep = declare_dependency(include_directories : '.',
                                link_with : libcommon)

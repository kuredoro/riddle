incdir = include_directories('.')

llvm_dep = dependency('llvm', version : '>=10.0')

libcg = static_library('code_generator',
                             sources : ['impl/code_generator.cpp', 'impl/cpp_generator.cpp'],
                             cpp_args : riddle_cpp_args,
                             c_args : riddle_c_args,
                             link_args : riddle_link_args,
                             dependencies : [ llvm_dep, ast_dep, lexer_dep, fmt_dep, common_dep ],
                             install : true)

cg_dep = declare_dependency(include_directories : incdir,
                             link_with : libcg)

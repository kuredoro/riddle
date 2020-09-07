catch2_dep = dependency('catch2', fallback : ['catch2', 'catch2_dep'])

common_test = executable('commonTest', ['test_main.cpp', 'common/trie_test.cpp'],
                        include_directories : '.',
                        cpp_args : riddle_cpp_args,
                        dependencies :
                        [ catch2_dep, fmt_dep, common_dep ])

lexer_test = executable('lexerTest', ['test_main.cpp', 'lexer/lexer_test.cpp'],
                        include_directories : '.',
                        cpp_args : riddle_cpp_args,
                        dependencies :
                        [ fmt_dep, catch2_dep, common_dep, lexer_dep ])

parser_test = executable('parserTest', ['test_main.cpp', 'parser/parser_test.cpp'],
                        include_directories : '.',
                        cpp_args : riddle_cpp_args,
                        dependencies :
                        [ fmt_dep, catch2_dep, parser_dep ])

test('common', common_test)
test('lexer', lexer_test)
test('parser', parser_test)

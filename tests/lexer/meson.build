catch2_dep = dependency('catch2', fallback : ['catch2', 'catch2_dep'])

lexer_test = executable('lexerTest', ['../test_main.cpp', 'lexer_test.cpp'],
                        dependencies :
                        [catch2_dep, lexer_dep])

test('Lexer tests', lexer_test)

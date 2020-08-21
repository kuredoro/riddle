catch2_dep = dependency('catch2', fallback : ['catch2', 'catch2_dep'])

parser_test = executable('parserTest', ['../test_main.cpp', 'parser_test.cpp'],
                        dependencies :
                        [catch2_dep, parser_dep])

test('Parser tests', parser_test)

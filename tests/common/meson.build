catch2_dep = dependency('catch2', fallback : ['catch2', 'catch2_dep'])
fmt_dep = dependency('fmt', fallback : ['fmt', 'fmt_dep'])

common_test = executable('commonTest', ['../test_main.cpp', 'trie_test.cpp'],
                        dependencies :
                        [catch2_dep, fmt_dep, common_dep])

test('common', common_test)

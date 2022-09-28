#
# Copyright (C) 2022 Nikolas Koesling <nikolas@koesling.info>.
# This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
#

add_executable(test1_${Target} test_file.cpp)
add_dependencies(test1_${Target} ${Target})
add_test(test1_${Target} test1_${Target})

enable_warnings(test1_${Target})
set_definitions(test1_${Target})
set_options(test1_${Target} FALSE)

if(CLANG_FORMAT)
    target_clangformat_setup(test1_${Target})
endif()

add_executable(test2_${Target} test_stdio.cpp)
add_dependencies(test2_${Target} ${Target})
add_test(test2_${Target} test2_${Target})

enable_warnings(test2_${Target})
set_definitions(test2_${Target})
set_options(test2_${Target} FALSE)

if(CLANG_FORMAT)
    target_clangformat_setup(test2_${Target})
endif()


if (NOT EXISTS "/home/mark/StorminNormin/build/install_manifest.txt")
    message(FATAL_ERROR "First install this package, then you can uninstall it.")
endif()

file(READ "/home/mark/StorminNormin/build/install_manifest.txt" files)
string(REGEX REPLACE "\n" ";" files "${files}")

foreach(file ${files})
    message(STATUS "Uninstalling: $ENV{DESTDIR}${file}")

    if (IS_SYMLINK "$ENV{DESTDIR}${file}" OR EXISTS "$ENV{DESTDIR}${file}")
        execute_process(COMMAND "${CMAKE_COMMAND}" -E remove "$ENV{DESTDIR}${file}")

    else (IS_SYMLINK "$ENV{DESTDIR}${file}" OR EXISTS "$ENV{DESTDIR}${file}")
        message(STATUS "File $ENV{DESTDIR}${file} does not exist.")
    endif()

endforeach()

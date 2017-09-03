SBEAML
======

Stack-Based Embeddable Application Main Loop.

License
-------

zlib License.

Target environments
-------------------

Windows, Linux, Mac OS X.

SBEAML is written in ISO C99 (to be precise, ISO C90 with stdbool.h/stdint.h),
and so probably works fine on other OS.

Usage
-----

1.  Add all source files in [src/](src/) directory to your project.
2.  Implement machdep library.
    See [src/lib/sbeaml_md.h](src/lib/sbeaml_md.h),
    [src/machdep/sample/sbeaml_md.c](src/machdep/sample/sbeaml_md.c).
3.  Implement configuration header file.
    See [src/machdep/sample/sbeaml_config.h](src/machdep/sample/sbeaml_config.h).
4.  Use public API functions.
    See [src/include/sbeaml.h](src/include/sbeaml.h),
    [sample/console/](sample/console/).

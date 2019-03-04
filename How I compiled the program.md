# HOW I COMPILED THE PROGRAMS

I used the gcc for the following reasons:
- Full suport of newer C standards
- Support for pragma directives (easy implementation of parallel computation)

I installed this Windows port of a POSIX (msys2.github.io) system including gcc locally on my account.

The packages are mingw-w64-x86_64-toolchain for the 64 bit toolchain respectively mingw-w64-i686-toolchain for the 32 bit toolchaing (the latter I used only for debugging). You can install them using pacman taking care of setting the correct proxy settings if necessary.

I used drmemory for memory debugging downloaded from http://www.drmemory.org/docs/page_install_windows.html (drmemory necessitates use of the 32 bit gcc).

https://gcc.gnu.org/onlinedocs/libgomp
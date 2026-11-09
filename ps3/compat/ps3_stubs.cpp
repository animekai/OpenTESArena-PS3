// Stubs de funciones POSIX ausentes en newlib/PSL1GHT (std::filesystem las referencia)
#include <cerrno>
extern "C" {
int symlink(const char *, const char *) { errno = ENOSYS; return -1; }
int link(const char *, const char *) { errno = ENOSYS; return -1; }
long readlink(const char *, char *, unsigned long) { errno = ENOSYS; return -1; }
int fchmod(int, unsigned int) { errno = ENOSYS; return -1; }
int chmod(const char *, unsigned int) { errno = ENOSYS; return -1; }
long pathconf(const char *, int) { errno = ENOSYS; return -1; }
long fpathconf(int, int) { errno = ENOSYS; return -1; }
}

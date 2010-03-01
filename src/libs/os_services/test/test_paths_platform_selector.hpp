#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__) || defined(__FreeBSD__) // || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)

const std::string default_dir = "/home/fernando/temp1/";

#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)

const std::string default_dir = "C:\\temp1\\";

#elif defined(__CYGWIN__)
#  error Platform not supported
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#  error No test for the moment
#else
#  error Platform not supported
#endif

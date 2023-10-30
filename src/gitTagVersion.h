#pragma once

#define ECC_VERSION_MAJOR 1
#define ECC_VERSION_MINOR 5
#define ECC_VERSION_PATCH 2
#if !defined _ECC_STR
  #define _ECC_STR(x) #x
#endif
#if !defined ECC_STR
  #define ECC_STR(x) _ECC_STR(x)
#endif
// Macro to convert library version number into an integer
#define ECC_VERSION_VAL(major, minor, patch) ((major << 16) | (minor << 8) | (patch))
// current library version as a string
#define CHIMERA_CORE_VERSION ECC_STR(ECC_VERSION_MAJOR) "." ECC_STR(ECC_VERSION_MINOR) "." ECC_STR(ECC_VERSION_PATCH)
// current library version as an int, to be used in comparisons, such as M5_SD_UPDATER_VERSION_INT >= VERSION_VAL(2, 0, 0)
#define CHIMERA_CORE_VERSION_INT ECC_VERSION_VAL(ECC_VERSION_MAJOR, ECC_VERSION_MINOR, ECC_VERSION_PATCH)

#undef ECC_STR
#undef _ECC_STR

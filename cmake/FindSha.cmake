
FIND_PATH(SHA_INCLUDE_DIR openssl/sha.h
/usr/include/
/usr/local/include/
)

FIND_LIBRARY(SHA_LIBRARY NAMES crypto
PATHS
/usr/${LIB_DESTINATION}
/usr/local/${LIB_DESTINATION}
)

IF(SHA_INCLUDE_DIR AND SHA_LIBRARY)
   SET(SHA_FOUND TRUE)
ENDIF(SHA_INCLUDE_DIR AND SHA_LIBRARY)


IF(SHA_FOUND)
   IF(NOT OpenSSL_FIND_QUIETLY)
      MESSAGE(STATUS "Found Sha: ${SHA_LIBRARY}")
   ENDIF(NOT OpenSSL_FIND_QUIETLY)
ELSE(SHA_FOUND)
   IF(OpenSSL_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find Sha. Please install libssl-dev")
   ENDIF(OpenSSL_FIND_REQUIRED)
ENDIF(SHA_FOUND)

MARK_AS_ADVANCED(
  SHA_INCLUDE_DIR
  SHA_LIBRARY
)

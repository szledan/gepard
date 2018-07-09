include(ExternalProject)

ExternalProject_Add(gtest
                    PREFIX ${PROJECT_SOURCE_DIR}/thirdparty/googletest
                    GIT_REPOSITORY https://github.com/google/googletest.git
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR}/thirdparty -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_INSTALL_INCLUDEDIR=include -DBUILD_GMOCK=OFF -DBUILD_GTEST=ON
                    BINARY_DIR ${PROJECT_BINARY_DIR}/thirdparty/googletest
                    BUILD_COMMAND make gtest
                    INSTALL_COMMAND make install > /dev/null)

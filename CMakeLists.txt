cmake_minimum_required(VERSION 3.10.2)

# CMAKE to create the shared library of the quantum gate decomposition project

# set the project name and version
project(DNAPCA VERSION 1.7)

# reuse compilation time linking for use runtime linking 
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# include CMAKE modules
include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckFunctionExists)
include(cmake/check_AVX.cmake)

# variables for compile and link options
set(CXX_FLAGS_DEBUG)
set(CXX_FLAGS_RELEASE)
set(EXTRA_INCLUDES)
set(BLAS_DIR)
set(PYTHON_PLAT_LIB)


#enable test target
enable_testing()


# find out python packages
find_package(PythonInterp)
find_package(PythonLibs)
find_package(NumPy REQUIRED)


exec_program(${PYTHON_EXECUTABLE}
             ARGS "-c \"from sysconfig import get_paths; info = get_paths(); platlib=info.get('platlib',' '); print(platlib)\""
             OUTPUT_VARIABLE PYTHON_PLAT_LIB
             RETURN_VALUE PLATLIB_NOT_FOUND
            )
if(PLATLIB_NOT_FOUND)
    message(FATAL_ERROR "Python platlib not found")
endif()

set(ENV{PYTHONPATH} "$ENV{PYTHONPATH};${PYTHON_PLAT_LIB}")
message("PYTHONPATH is set to "$ENV{PYTHONPATH})

# contruct library directories
exec_program(${PYTHON_EXECUTABLE}
             ARGS "-c \"from distutils.sysconfig import get_python_lib; tmp = [s + '/../..' for s in [get_python_lib()]]; ret=';'.join(tmp); print(ret)\""
             OUTPUT_VARIABLE PYTHON_SYS_PATH
             RETURN_VALUE SYSPATH_NOT_FOUND
            )
if(SYSPATH_NOT_FOUND)
    message(FATAL_ERROR "Python syspath not found")
endif()

message("Python syspaths: " ${PYTHON_SYS_PATH})




# contruct include directories
exec_program(${PYTHON_EXECUTABLE}
             ARGS "-c \"from distutils.sysconfig import get_python_lib; tmp = [s + '/../../../include' for s in [get_python_lib()]]; ret=';'.join(tmp); print(ret)\""
             OUTPUT_VARIABLE CMAKE_REQUIRED_INCLUDES
             RETURN_VALUE SYSPATH_NOT_FOUND
            )
if(SYSPATH_NOT_FOUND)
    message(FATAL_ERROR "Python include path not found")
endif()

message("Include paths: " ${CMAKE_REQUIRED_INCLUDES})


set(NUMPY_INC_DIR ${NumPy_INCLUDE_DIR})


if(NOT NumPy_FOUND)
    message(FATAL_ERROR "NumPy headers not found")
endif()


# adding BLAS library dir if given by environment variable
if(DEFINED ENV{BLAS_LIB_DIR})

  set(BLAS_DIR "$ENV{BLAS_LIB_DIR}")

else()

# Determine CBLAS library directory behind Numpy
exec_program(
  ${PYTHON_EXECUTABLE}
  ARGS "-c \"import numpy; blas_info=numpy.__config__.get_info('blas_opt_info'); libs = blas_info.get('library_dirs'); print(libs[0])\""
  OUTPUT_VARIABLE BLAS_DIR
  RETURN_VALUE NUMPY_BLAS_NOT_FOUND
)

endif()



find_package(PythonExtensions REQUIRED)


set(CMAKE_VERBOSE_MAKEFILE ON)

#################################################################
# find MPI libraries if MPI is enables by environment variable PIQUASSOBOOST_MPI

if(DEFINED ENV{DNAPCA_MPI})
  find_package(MPI REQUIRED)

  # openmpi which has a different c++ bindings
  #add_definitions(-DOMPI_SKIP_MPICXX)

  # setting basic compile flags
  list(APPEND CXX_FLAGS_DEBUG "-D__MPI__")
  list(APPEND CXX_FLAGS_RELEASE "-D__MPI__")

  list(APPEND EXTRA_INCLUDES "${MPI_C_INCLUDE_PATH}") 

endif()

#################################################################
# find CBLAS libraries. Hints are given by numpy library directory via variable BLAS_DIR

set(ENV{LD_LIBRARY_PATH} "${BLAS_DIR}:$ENV{LD_LIBRARY_PATH}")

find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)

# make difference between MKL and OPENBLAS by checking specific functions
set(CMAKE_REQUIRED_LIBRARIES "${BLAS_LIBRARIES}" "-lm")
check_function_exists(MKL_Set_Num_Threads BLAS_IS_MKL)
check_function_exists(openblas_set_num_threads BLAS_IS_OPENBLAS)

# check the presence of lapacke library
check_function_exists(LAPACKE_zggev LAPACKE_FOUND)
set(CMAKE_REQUIRED_LIBRARIES "")

# If MKL is enabled
if(${BLAS_IS_MKL})
  list(APPEND CXX_FLAGS_DEBUG "-DBLAS=1")
  list(APPEND CXX_FLAGS_RELEASE "-DBLAS=1")

  # If OpenBlas is enabled
elseif(${BLAS_IS_OPENBLAS})
  list(APPEND CXX_FLAGS_DEBUG "-DBLAS=2")
  list(APPEND CXX_FLAGS_RELEASE "-DBLAS=2")
else()

  list(APPEND CXX_FLAGS_DEBUG "-DBLAS=0")
  list(APPEND CXX_FLAGS_RELEASE "-DBLAS=0")
endif()


# if LAPACKE not found try another round
if(${LAPACKE_FOUND} )
    set(LAPACKE_LIBRARIES)
else()
    find_library(LAPACKE_LIBRARIES lapacke REQUIRED)    
endif()

# setting basic compile flags
list(APPEND CXX_FLAGS_DEBUG "-Wall" "-Wpedantic" "-Wextra" "-fexceptions" "-DDEBUG" "-fno-builtin-malloc" "-fno-builtin-calloc" "-fno-builtin-realloc" "-fno-builtin-free" "-fpermissive")
list(APPEND CXX_FLAGS_RELEASE "-Wall" "-O3" "-m64" "-DNDEBUG" "-fno-builtin-malloc" "-fno-builtin-calloc" "-fno-builtin-realloc" "-fno-builtin-free" "-fpermissive")


# checking for AVX/AVX2 support
CHECK_FOR_AVX()

# Identify the compiler type and set compiler specific options
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  # using Clang
  message("-- Using Clang compiler")

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # using GCC
  message("-- Using GNU compiler")
  list(APPEND CXX_FLAGS_DEBUG "-g3" "-ggdb")
  list(APPEND CXX_FLAGS_RELEASE "-ftree-vectorize")

  if (${HAVE_AVX512F_EXTENSIONS})
    list(APPEND CXX_FLAGS_RELEASE "-mavx512f" "-mfma" "-DUSE_AVX")
  elseif (${HAVE_AVX2_EXTENSIONS})
    list(APPEND CXX_FLAGS_RELEASE "-mavx2" "-mfma" "-DUSE_AVX")
    list(APPEND EXTRA_INCLUDES "./gates/kernels/include/")
  elseif (${HAVE_AVX_EXTENSIONS})
    list(APPEND CXX_FLAGS_RELEASE "-mavx" "-mfma" "-DUSE_AVX")
  endif()


elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  # using Intel C++
  message("-- Using Intel compiler")
  if (BLAS_IS_MKL)
    list(APPEND CXX_FLAGS_DEBUG "-mkl" "-tbb")
    list(APPEND CXX_FLAGS_RELEASE "-mkl" "-tbb")
  endif()

  if (${HAVE_AVX512F_EXTENSIONS})
    list(APPEND CXX_FLAGS_RELEASE "-mavx512f" "-mfma" "-DUSE_AVX512F")
  elseif (${HAVE_AVX2_EXTENSIONS})
    list(APPEND CXX_FLAGS_RELEASE "-mavx2" "-mfma" "-DUSE_AVX")
  elseif (${HAVE_AVX_EXTENSIONS})
    list(APPEND CXX_FLAGS_RELEASE "-mavx" "-mfma" "-DUSE_AVX")
  endif()

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # using Visual Studio C++
  message("-- Using Visual Studio C++ compiler")

  if (${HAVE_AVX512F_EXTENSIONS})
    list(APPEND CXX_FLAGS_RELEASE "/arch:AVX512F" "-DUSE_AVX512F")
  elseif (${HAVE_AVX2_EXTENSIONS})
    list(APPEND CXX_FLAGS_RELEASE "/arch:AVX2" "-DUSE_AVX")
  elseif (${HAVE_AVX_EXTENSIONS})
    list(APPEND CXX_FLAGS_RELEASE "/arch:AVX" "-DUSE_AVX")
  endif()

endif()






############################################################xx
# checking TBB libraries and headers

# adding TBB library dir if given by environment variable
if(DEFINED ENV{TBB_LIB_DIR})

  find_library(TBB_LIB tbb
               PATHS $ENV{TBB_LIB_DIR}
               NO_DEFAULT_PATH
               REQUIRED)

  find_library(TBBMALLOC_LIB tbbmalloc
               PATHS $ENV{TBB_LIB_DIR}
               NO_DEFAULT_PATH
               REQUIRED)

  find_library(TBBMALLOC_PROXY_LIB tbbmalloc_proxy
               PATHS $ENV{TBB_LIB_DIR}
               NO_DEFAULT_PATH
               REQUIRED)

else()

  find_library(TBB_LIB tbb
               PATHS ${PYTHON_SYS_PATH}
               NO_DEFAULT_PATH
               REQUIRED)

  find_library(TBBMALLOC_LIB tbbmalloc
               PATHS ${PYTHON_SYS_PATH}
               NO_DEFAULT_PATH
               REQUIRED)

  find_library(TBBMALLOC_PROXY_LIB tbbmalloc_proxy
               PATHS ${PYTHON_SYS_PATH}
               NO_DEFAULT_PATH
               REQUIRED)
  
endif()


# adding TBB include dir
if(DEFINED ENV{TBB_INC_DIR})

  set(CMAKE_REQUIRED_FLAGS "-c")
  check_include_file_cxx(tbb/tbb.h TBB_HEADER "-I$ENV{TBB_INC_DIR}")

  if(NOT TBB_HEADER)
    message(FATAL_ERROR "TBB header tbb.h not found")
  endif()

  message("-- Adding include directory $ENV{TBB_INC_DIR}")
  list(APPEND EXTRA_INCLUDES "$ENV{TBB_INC_DIR}")

else()
 
  set(CMAKE_REQUIRED_FLAGS "-c")
  check_include_file_cxx(tbb/tbb.h TBB_HEADER )
  list(APPEND EXTRA_INCLUDES "${CMAKE_REQUIRED_INCLUDES}") 

  if(NOT TBB_HEADER)
    message(FATAL_ERROR "TBB header tbb.h not found")
  endif()


endif()


list(APPEND dnapca_files 
    ${PROJECT_SOURCE_DIR}/common/common.cpp
    ${PROJECT_SOURCE_DIR}/common/dot.cpp
    ${PROJECT_SOURCE_DIR}/common/matrix.cpp
    ${PROJECT_SOURCE_DIR}/common/matrix_real.cpp
    ${PROJECT_SOURCE_DIR}/common/logging.cpp
    ${PROJECT_SOURCE_DIR}/common/Adam.cpp
    ${PROJECT_SOURCE_DIR}/dnapca/single_node_pca.cpp
)


if(DEFINED ENV{DNAPCA_MPI})

    list(APPEND dnapca_files 
    ${PROJECT_SOURCE_DIR}/common/mpi_base.cpp
    )

endif()



add_library(dnapca SHARED
    ${dnapca_files})

# adding compile options
target_compile_options(dnapca PUBLIC
    ${CXX_FLAGS}
    "$<$<CONFIG:Debug>:${CXX_FLAGS_DEBUG}>"
    "$<$<CONFIG:Release>:${CXX_FLAGS_RELEASE}>"
)




# External libarray for calculating QFT on DFE
add_library( maxtx SHARED IMPORTED )
set_target_properties( maxtx PROPERTIES IMPORTED_LOCATION $ENV{MAXTX_LIB_DIR}/libmaxtx.a )

# adding linking options
target_link_libraries( dnapca PRIVATE
    ${BLAS_LIBRARIES}
    ${LAPACK_LIBRARIES}
    ${LAPACKE_LIBRARIES}
    ${TBBMALLOC_LIB}
    ${TBBMALLOC_PROXY_LIB}
    ${TBB_LIB}
    ${MPI_C_LIBRARIES}
    maxtx
    )




target_include_directories(dnapca PRIVATE
                            .
                            ./common/include
                            $ENV{MAXTX_INC_DIR}/maxtx
                            $ENV{MAXTX_INC_DIR}/backend
                            $ENV{MAXTX_INC_DIR}/xtx
                            ${EXTRA_INCLUDES})




set_target_properties(
    dnapca
    PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dnapcapy
    )



# adding CMAKE files for Python extensions
add_subdirectory (dnapcapy)
#add_subdirectory (dnapcapy/gates)
#add_subdirectory (dnapcapy/nn)


if(DEFINED ENV{DNAPCA_CTEST})
    # adding CMAKE files for executables
    add_subdirectory (test_standalone)
endif()


install(TARGETS dnapca LIBRARY DESTINATION dnapcapy)





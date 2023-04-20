
set(EXT_DIR ${PROJECT_SOURCE_DIR}/dnapcapy)


add_library( dnapcapy_Wrapper MODULE
    ${EXT_DIR}/dnapcapy_Wrapper.cpp
    ${PROJECT_SOURCE_DIR}/common/numpy_interface.cpp        
)


ADD_DEPENDENCIES (dnapcapy_Wrapper dnapca)

target_link_libraries (dnapcapy_Wrapper dnapca  ${BLAS_LIBRARIES}  ${LAPACKE_LIBRARIES})

python_extension_module(dnapcapy_Wrapper)


# adding compile options
target_compile_options(dnapcapy_Wrapper PRIVATE
    ${CXX_FLAGS}
    "$<$<CONFIG:Debug>:${CXX_FLAGS_DEBUG}>"
    "$<$<CONFIG:Release>:${CXX_FLAGS_RELEASE}>"
    "-DCPYTHON"
)


target_include_directories(dnapcapy_Wrapper PRIVATE
                            ${PYTHON_INCLUDE_DIR}
                            ${NUMPY_INC_DIR}
                            ${PROJECT_SOURCE_DIR}
                            ${PROJECT_SOURCE_DIR}/common/include
                            ${EXTRA_INCLUDES})


set_target_properties( dnapcapy_Wrapper PROPERTIES
                        INSTALL_RPATH "$ORIGIN/.."
                        LIBRARY_OUTPUT_DIRECTORY ${EXT_DIR}
)




install(TARGETS dnapcapy_Wrapper LIBRARY 
         DESTINATION dnapcapy)



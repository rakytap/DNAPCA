/*
Created on Fri Jun 26 14:42:56 2020
Copyright (C) 2020 Peter Rakyta, Ph.D.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/.

@author: Peter Rakyta, Ph.D.
*/
/*
\file qgd_dnapca_Wrapper.cpp
\brief Python interface for the N_Qubit_Decomposition class
*/

#define PY_SSIZE_T_CLEAN


#include <Python.h>
#include <numpy/arrayobject.h>
#include "structmember.h"
#include <stdio.h>
//#include "dnapca.h"


#include "numpy_interface.h"




/**
@brief Type definition of the dnapca_Wrapper Python class of the dnapca_Wrapper module
*/
typedef struct dnapca_Wrapper {
    PyObject_HEAD
} dnapca_Wrapper;



/**
@brief Creates an instance of class N_Qubit_Decomposition and return with a pointer pointing to the class instance (C++ linking is needed)
@param Umtx An instance of class Matrix containing the unitary to be decomposed
@param qbit_num Number of qubits spanning the unitary
@param level_limit The maximal number of layers used in the decomposition
@param initial_guess Type to guess the initial values for the optimization. Possible values: ZEROS=0, RANDOM=1, CLOSE_TO_ZERO=2
@return Return with a void pointer pointing to an instance of N_Qubit_Decomposition class.
*/
/*
dnapca* 
create_dnapca() {

    return new dnapca( Umtx, qbit_num, level_limit, level_limit_min, topology_in, accelerator_num );
}
*/



/**
@brief Call to deallocate an instance of dnapca class
@param ptr A pointer pointing to an instance of N_Qubit_Decomposition class.
/*
void
release_dnapca( dnapca*  instance ) {

    if (instance != NULL ) {
        delete instance;
    }
    return;
}
*/





extern "C"
{


/**
@brief Method called when a python instance of the class qgd_dnapca_Wrapper is destroyed
@param self A pointer pointing to an instance of class qgd_dnapca_Wrapper.
*/
static void
dnapca_Wrapper_dealloc(dnapca_Wrapper *self)
{
/*
    if ( self->decomp != NULL ) {
        // deallocate the instance of class N_Qubit_Decomposition
        //release_dnapca( self->decomp );
        //self->decomp = NULL;
    }
*/
/*
    if ( self->Umtx != NULL ) {
        // release the unitary to be decomposed
        //Py_DECREF(self->Umtx);    
        //self->Umtx = NULL;
    }
  */  
    Py_TYPE(self)->tp_free((PyObject *) self);

}

/**
@brief Method called when a python instance of the class qgd_dnapca_Wrapper is allocated
@param type A pointer pointing to a structure describing the type of the class qgd_dnapca_Wrapper.
*/
static PyObject *
dnapca_Wrapper_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    dnapca_Wrapper *self;
    self = (dnapca_Wrapper *) type->tp_alloc(type, 0);
    if (self != NULL) {

        //self->decomp = NULL;
        //self->Umtx = NULL;

    }

    return (PyObject *) self;
}


/**
@brief Method called when a python instance of the class qgd_dnapca_Wrapper is initialized
@param self A pointer pointing to an instance of the class qgd_dnapca_Wrapper.
@param args A tuple of the input arguments: Umtx (numpy array), qbit_num (integer), optimize_layer_num (bool), initial_guess (string PyObject 
@param kwds A tuple of keywords
*/
static int
dnapca_Wrapper_init(dnapca_Wrapper *self, PyObject *args, PyObject *kwds)
{
    // The tuple of expected keywords
    static char *kwlist[] = {NULL};
 
    // initiate variables for input arguments

    int input;

    // parsing input arguments
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|i", kwlist,
                                     &input))
        return -1;
/*
    // convert python object array to numpy C API array
    if ( Umtx_arg == NULL ) return -1;
    self->Umtx = PyArray_FROM_OTF(Umtx_arg, NPY_COMPLEX128, NPY_ARRAY_IN_ARRAY);

    // test C-style contiguous memory allocation of the array
    if ( !PyArray_IS_C_CONTIGUOUS(self->Umtx) ) {
        std::cout << "Umtx is not memory contiguous" << std::endl;
    }


    // create QGD version of the Umtx
    Matrix Umtx_mtx = numpy2matrix(self->Umtx);
*/



    return 0;
}


/**
@brief Wrapper function to call the start_decomposition method of C++ class N_Qubit_Decomposition
@param self A pointer pointing to an instance of the class qgd_dnapca_Wrapper.
@param args A tuple of the input arguments: finalize_decomp (bool), prepare_export (bool)
@param kwds A tuple of keywords
*/
static PyObject *
dnapca_Wrapper_test(dnapca_Wrapper *self, PyObject *args, PyObject *kwds)
{

    // The tuple of expected keywords
    static char *kwlist[] = {(char*)"input", NULL};

    // initiate variables for input arguments
    int input; 

    // parsing input arguments
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|i", kwlist,
                                     &input))
        return Py_BuildValue("i", -1);

        


    return Py_BuildValue("i", 0);

}






/**
@brief Structure containing metadata about the members of class qgd_dnapca_Wrapper.
*/
static PyMemberDef dnapca_Wrapper_members[] = {
    {NULL}  /* Sentinel */
};

/**
@brief Structure containing metadata about the methods of class qgd_dnapca_Wrapper.
*/
static PyMethodDef dnapca_Wrapper_methods[] = {
    {"test", (PyCFunction) dnapca_Wrapper_test, METH_VARARGS | METH_KEYWORDS,
     "Test"
    },
    {NULL}  /* Sentinel */
};

/**
@brief A structure describing the type of the class qgd_dnapca_Wrapper.
*/
static PyTypeObject dnapca_Wrapper_Type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "dnapca_Wrapper.dnapca_Wrapper", /*tp_name*/
  sizeof(dnapca_Wrapper), /*tp_basicsize*/
  0, /*tp_itemsize*/
  (destructor) dnapca_Wrapper_dealloc, /*tp_dealloc*/
  #if PY_VERSION_HEX < 0x030800b4
  0, /*tp_print*/
  #endif
  #if PY_VERSION_HEX >= 0x030800b4
  0, /*tp_vectorcall_offset*/
  #endif
  0, /*tp_getattr*/
  0, /*tp_setattr*/
  #if PY_MAJOR_VERSION < 3
  0, /*tp_compare*/
  #endif
  #if PY_MAJOR_VERSION >= 3
  0, /*tp_as_async*/
  #endif
  0, /*tp_repr*/
  0, /*tp_as_number*/
  0, /*tp_as_sequence*/
  0, /*tp_as_mapping*/
  0, /*tp_hash*/
  0, /*tp_call*/
  0, /*tp_str*/
  0, /*tp_getattro*/
  0, /*tp_setattro*/
  0, /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
  "Object to represent a Gates_block class of the QGD package.", /*tp_doc*/
  0, /*tp_traverse*/
  0, /*tp_clear*/
  0, /*tp_richcompare*/
  0, /*tp_weaklistoffset*/
  0, /*tp_iter*/
  0, /*tp_iternext*/
  dnapca_Wrapper_methods, /*tp_methods*/
  dnapca_Wrapper_members, /*tp_members*/
  0, /*tp_getset*/
  0, /*tp_base*/
  0, /*tp_dict*/
  0, /*tp_descr_get*/
  0, /*tp_descr_set*/
  0, /*tp_dictoffset*/
  (initproc) dnapca_Wrapper_init, /*tp_init*/
  0, /*tp_alloc*/
  dnapca_Wrapper_new, /*tp_new*/
  0, /*tp_free*/
  0, /*tp_is_gc*/
  0, /*tp_bases*/
  0, /*tp_mro*/
  0, /*tp_cache*/
  0, /*tp_subclasses*/
  0, /*tp_weaklist*/
  0, /*tp_del*/
  0, /*tp_version_tag*/
  #if PY_VERSION_HEX >= 0x030400a1
  0, /*tp_finalize*/
  #endif
  #if PY_VERSION_HEX >= 0x030800b1
  0, /*tp_vectorcall*/
  #endif
  #if PY_VERSION_HEX >= 0x030800b4 && PY_VERSION_HEX < 0x03090000
  0, /*tp_print*/
  #endif
};

/**
@brief Structure containing metadata about the module.
*/
static PyModuleDef dnapca_Wrapper_Module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "dnapca_Wrapper",
    .m_doc = "Python binding for QGD N_Qubit_Decomposition class",
    .m_size = -1,
};


/**
@brief Method called when the Python module is initialized
*/
PyMODINIT_FUNC
PyInit_dnapca_Wrapper(void)
{
    // initialize Numpy API
    import_array();

    PyObject *m;
    if (PyType_Ready(&dnapca_Wrapper_Type) < 0)
        return NULL;

    m = PyModule_Create(&dnapca_Wrapper_Module);
    if (m == NULL)
        return NULL;

    Py_INCREF(&dnapca_Wrapper_Type);
    if (PyModule_AddObject(m, "dnapca_Wrapper", (PyObject *) &dnapca_Wrapper_Type) < 0) {
        Py_DECREF(&dnapca_Wrapper_Type);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}


} //extern C


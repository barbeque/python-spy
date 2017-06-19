#include <Python/Python.h>
#include <iostream>
#include <sstream>

void expose_type(PyTypeObject* o) {
    std::cout << "\t\tType name: " << o->tp_name << std::endl;
    std::cout << "\t\tSize: " << o->tp_basicsize << " basic, " << o->tp_itemsize << " item" << std::endl;
}

void expose(PyObject* o, const char* name) {
    std::cout << "Python object (" << name << ") at " << std::hex << (unsigned long)o << std::endl;
    std::cout << "\tRef count " << std::hex << o->ob_refcnt << std::endl;    
    std::cout << "\tType def at " << std::hex << (unsigned long)o->ob_type << std::endl;
    expose_type(o->ob_type);
}

void safedecref(PyObject* o) {
    if(o) {
        Py_DECREF(o);
    }
}

int main(int argc, char* argv[]) {
    // create a demo python environment
    Py_Initialize();
    
    PyObject* pName = PyString_FromString("Hello, World!");

    // globals
    PyObject* globals = PyEval_GetGlobals();
    if(!globals) {
        std::cout << "Why did GetGlobals return null?" << std::endl;
    }
    else if(PyDict_Check(globals)) {
        std::cout << "Good news, globals IS a dictionary!" << std::endl;
    }

    PyObject* locals = PyEval_GetLocals();
    if(!locals) {
        std::cout << "Why did GetLocals return null?" << std::endl;
    }
    else if(PyDict_Check(locals)) {
        std::cout << "Good news, locals is a dict." << std::endl;
    }

    // hack up that python state pretty hard
    // identify all known python objects
    //  1. find the root python object
    //  2. iterate using the doubly linked list in the head
    //  3. expose the type object from each item
    expose(pName, "pName");

    // import the GC module
    PyObject* gcMod = PyImport_ImportModule("gc");
    expose(gcMod, "gc module");

    PyObject* getObjects = PyObject_GetAttrString(gcMod, "get_objects");
    expose(getObjects, "gc.get_objects");

    PyObject* result = PyObject_CallObject(getObjects, NULL);
    expose(result, "result of gc.get_objects");

    // iterate list
    if(PySequence_Check(result)) {
        std::cout << "Result of gc.get_objects is a sequence." << std::endl;
        Py_ssize_t length = PySequence_Length(result);
        std::cout << "gc.get_objects length is " << std::dec << length << " objects" << std::endl;
        
        for(int i = 0; i < length; ++i) {
            PyObject* o = PySequence_GetItem(result, i);
            if(o) {
                std::ostringstream s;
                s << "item " << i;
                expose(o, s.str().c_str());
                Py_DECREF(o); // it's ours now
            }
            else {
                std::cout << "nil result " << i << std::endl;
            }
        }
    }

    safedecref(gcMod);
    safedecref(globals);
    safedecref(locals);
    Py_DECREF(pName); // clean up

    Py_Finalize(); // die

    return 0;
}

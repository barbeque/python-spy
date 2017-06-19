#include <Python/Python.h>
#include <iostream>

void expose_type(PyTypeObject* o) {
    std::cout << "\t\tType name: " << o->tp_name << std::endl;
    std::cout << "\t\tSize: " << o->tp_basicsize << " basic, " << o->tp_itemsize << " item" << std::endl;
}

void expose(PyObject* o) {
    std::cout << "Python object at " << std::hex << (unsigned long)o << std::endl;
    std::cout << "\tRef count " << std::hex << o->ob_refcnt << std::endl;    
    std::cout << "\tType def at " << std::hex << (unsigned long)o->ob_type << std::endl;
    expose_type(o->ob_type);
}

int main(int argc, char* argv[]) {
    // create a demo python environment
    Py_Initialize();
    
    PyObject* pName = PyString_FromString("Hello, World!");

    // hack up that python state pretty hard
    // identify all known python objects
    //  1. find the root python object
    //  2. iterate using the doubly linked list in the head
    //  3. expose the type object from each item
    expose(pName);

    Py_DECREF(pName); // clean up
    Py_Finalize(); // die

    return 0;
}

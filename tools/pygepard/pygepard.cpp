#include <Python.h>
#include <iostream>
#include "gepard.h"
#include "surfaces/gepard-xsurface.h"

typedef struct {
    PyObject_HEAD
    gepard::XSurface *srf;
    gepard::Gepard *gpd;
} pygepard_GepardObject;

/*
 * Gepard object internal functions
 */

static PyObject* Gepard__new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    pygepard_GepardObject *self;
    int x = 600, y = 600;
    const char *title = "Gepard";

    if (!PyArg_ParseTuple(args, "|iis", &x, &y, &title))
        Py_RETURN_NONE;

    self = (pygepard_GepardObject *)type->tp_alloc(type, 0);

    self->srf = new gepard::XSurface(x, y, std::string(title));
    self->gpd = new gepard::Gepard(self->srf);

    return (PyObject *)self;
}

static int Gepard__init(PyObject *self, PyObject *args, PyObject *kwds)
{
    pygepard_GepardObject *obj = (pygepard_GepardObject *) self;
    obj->gpd->setFillColor(0.0f, 0.0f, 0.0f, 1.0f);
    obj->gpd->fillRect(0, 0, obj->srf->width(), obj->srf->height());
    obj->gpd->setFillColor(1.0f, 1.0f, 1.0f, 1.0f);

    return 0;
}

static void Gepard__dealloc(PyObject *self)
{
    delete ((pygepard_GepardObject *)self)->gpd;
    delete ((pygepard_GepardObject *)self)->srf;

    Py_TYPE(self)->tp_free(self);
}

/*
 * Gepard API functions
 */

static PyObject* Gepard__save(PyObject *self, PyObject* args)
{
    ((pygepard_GepardObject *)self)->gpd->save();
    Py_RETURN_NONE;
}

static PyObject* Gepard__restore(PyObject *self, PyObject* args)
{
    ((pygepard_GepardObject *)self)->gpd->restore();
    Py_RETURN_NONE;
}

static PyObject* Gepard__closePath(PyObject *self, PyObject* args)
{
    ((pygepard_GepardObject *)self)->gpd->closePath();
    Py_RETURN_NONE;
}

static PyObject* Gepard__moveTo(PyObject *self, PyObject* args)
{
    float x, y;
    if (!PyArg_ParseTuple(args, "ff", &x, &y))
        Py_RETURN_NONE;

    ((pygepard_GepardObject *)self)->gpd->moveTo(x, y);
    Py_RETURN_NONE;
}

static PyObject* Gepard__lineTo(PyObject *self, PyObject* args)
{
    float x, y;
    if (!PyArg_ParseTuple(args, "ff", &x, &y))
        Py_RETURN_NONE;

    ((pygepard_GepardObject *)self)->gpd->lineTo(x, y);
    Py_RETURN_NONE;
}

static PyObject* Gepard__quadraticCurveTo(PyObject *self, PyObject* args)
{
    float cpx, cpy, x, y;
    if (!PyArg_ParseTuple(args, "ffff", &cpx, &cpy, &x, &y))
        Py_RETURN_NONE;

    ((pygepard_GepardObject *)self)->gpd->quadraticCurveTo(cpy, cpy, x, y);
    Py_RETURN_NONE;
}

static PyObject* Gepard__bezierCurveTo(PyObject *self, PyObject* args)
{
    float cp1x, cp1y, cp2x, cp2y, x, y;
    if (!PyArg_ParseTuple(args, "ffffff", &cp1x, &cp1y, &cp2x, &cp2y, &x, &y))
        Py_RETURN_NONE;

    ((pygepard_GepardObject *)self)->gpd->bezierCurveTo(cp1x, cp1y, cp2x, cp2y, x, y);
    Py_RETURN_NONE;
}

static PyObject* Gepard__arcTo(PyObject *self, PyObject* args)
{
    float x1, y1, x2, y2, r;
    if (!PyArg_ParseTuple(args, "fffff", &x1, &y1, &x2, &y2, &r))
        Py_RETURN_NONE;

    ((pygepard_GepardObject *)self)->gpd->arcTo(x1, y1, x2, y2, r);
    Py_RETURN_NONE;
}

static PyObject* Gepard__rect(PyObject *self, PyObject* args)
{
    float x, y, w, h;
    if (!PyArg_ParseTuple(args, "ffff", &x, &y, &w, &h))
        Py_RETURN_NONE;

    ((pygepard_GepardObject *)self)->gpd->rect(x, y, w, h);
    Py_RETURN_NONE;
}

static PyObject* Gepard__arc(PyObject *self, PyObject* args)
{
    float x, y, r, s, e;
    PyObject *ccw = nullptr;
    if (!PyArg_ParseTuple(args, "fffff|O!", &x, &y, &r, &s, &e, &PyBool_Type, &ccw))
        Py_RETURN_NONE;

    ((pygepard_GepardObject *)self)->gpd->arc(x, y, r, s, e, PyObject_IsTrue(ccw));
    Py_RETURN_NONE;
}

static PyObject* Gepard__scale(PyObject *self, PyObject* args)
{
    float x, y;
    if (!PyArg_ParseTuple(args, "ff", &x, &y))
        Py_RETURN_NONE;

    ((pygepard_GepardObject *)self)->gpd->scale(x, y);
    Py_RETURN_NONE;
}

static PyObject* Gepard__rotate(PyObject *self, PyObject* args)
{
    float a;
    if (!PyArg_ParseTuple(args, "f", &a))
        Py_RETURN_NONE;

    ((pygepard_GepardObject *)self)->gpd->rotate(a);
    Py_RETURN_NONE;
}

static PyObject* Gepard__translate(PyObject *self, PyObject* args)
{
    float x, y;
    if (!PyArg_ParseTuple(args, "ff", &x, &y))
        Py_RETURN_NONE;

    ((pygepard_GepardObject *)self)->gpd->translate(x, y);
    Py_RETURN_NONE;
}

static PyObject* Gepard__transform(PyObject *self, PyObject* args)
{
    float a, b, c, d, e, f;
    if (!PyArg_ParseTuple(args, "ffffff", &a, &b, &c, &d, &e, &f))
        Py_RETURN_NONE;

    ((pygepard_GepardObject *)self)->gpd->transform(a, b, c, d, e, f);
    Py_RETURN_NONE;
}

static PyObject* Gepard__setTransform(PyObject *self, PyObject* args)
{
    float a, b, c, d, e, f;
    if (!PyArg_ParseTuple(args, "ffffff", &a, &b, &c, &d, &e, &f))
        Py_RETURN_NONE;

    ((pygepard_GepardObject *)self)->gpd->setTransform(a, b, c, d, e, f);
    Py_RETURN_NONE;
}

static PyObject* Gepard__clearRect(PyObject *self, PyObject* args)
{
    float x, y, w, h;
    if (!PyArg_ParseTuple(args, "ffff", &x, &y, &w, &h))
        Py_RETURN_NONE;

    ((pygepard_GepardObject *)self)->gpd->clearRect(x, y, w, h);
    Py_RETURN_NONE;
}

static PyObject* Gepard__fillRect(PyObject *self, PyObject* args)
{
    float x, y, w, h;
    if (!PyArg_ParseTuple(args, "ffff", &x, &y, &w, &h))
        Py_RETURN_NONE;

    ((pygepard_GepardObject *)self)->gpd->fillRect(x, y, w, h);
    Py_RETURN_NONE;
}

static PyObject* Gepard__strokeRect(PyObject *self, PyObject* args)
{
    float x, y, w, h;
    if (!PyArg_ParseTuple(args, "ffff", &x, &y, &w, &h))
        Py_RETURN_NONE;

    ((pygepard_GepardObject *)self)->gpd->strokeRect(x, y, w, h);
    Py_RETURN_NONE;
}

static PyObject* Gepard__beginPath(PyObject *self, PyObject* args)
{
    ((pygepard_GepardObject *)self)->gpd->beginPath();
    Py_RETURN_NONE;
}

static PyObject* Gepard__fill(PyObject *self, PyObject* args)
{
    ((pygepard_GepardObject *)self)->gpd->fill();
    Py_RETURN_NONE;
}

static PyObject* Gepard__stroke(PyObject *self, PyObject* args)
{
    ((pygepard_GepardObject *)self)->gpd->stroke();
    Py_RETURN_NONE;
}

static PyObject* Gepard__drawFocusIfNeeded(PyObject *self, PyObject* args)
{
    ((pygepard_GepardObject *)self)->gpd->drawFocusIfNeeded();
    Py_RETURN_NONE;
}

static PyObject* Gepard__clip(PyObject *self, PyObject* args)
{
    ((pygepard_GepardObject *)self)->gpd->clip();
    Py_RETURN_NONE;
}

static PyObject* Gepard__isPointInPath(PyObject *self, PyObject* args)
{
    float x, y;
    if (!PyArg_ParseTuple(args, "ff", &x, &y))
        Py_RETURN_NONE;

    ((pygepard_GepardObject *)self)->gpd->isPointInPath(x, y);
    Py_RETURN_NONE;
}

static PyObject* Gepard__setFillColor(PyObject *self, PyObject* args)
{
    float red, green, blue, alpha = 1.0f;

    if (!PyArg_ParseTuple(args, "fff|f", &red, &green, &blue, &alpha))
        Py_RETURN_NONE;

    ((pygepard_GepardObject *)self)->gpd->setFillColor(red, green, blue, alpha);

    Py_RETURN_NONE;
}

static PyMethodDef pClassMethods[] = {
    {"save", (PyCFunction) Gepard__save, METH_NOARGS, "save"},
    {"restore", (PyCFunction) Gepard__restore, METH_NOARGS, "restore"},
    {"closePath", (PyCFunction) Gepard__closePath, METH_VARARGS, "closePath"},
    {"moveTo", (PyCFunction) Gepard__moveTo, METH_VARARGS, "moveTo"},
    {"lineTo", (PyCFunction) Gepard__lineTo, METH_VARARGS, "lineTo"},
    {"quadraticCurveTo", (PyCFunction) Gepard__quadraticCurveTo, METH_VARARGS, "quadraticCurveTo"},
    {"bezierCurveTo", (PyCFunction) Gepard__bezierCurveTo, METH_VARARGS, "bezierCurveTo"},
    {"arcTo", (PyCFunction) Gepard__arcTo, METH_VARARGS, "arcTo"},
    {"rect", (PyCFunction) Gepard__rect, METH_VARARGS, "rect"},
    {"arc", (PyCFunction) Gepard__arc, METH_VARARGS, "arc"},
    {"scale", (PyCFunction) Gepard__scale, METH_VARARGS, "scale"},
    {"rotate", (PyCFunction) Gepard__rotate, METH_VARARGS, "rotate"},
    {"translate", (PyCFunction) Gepard__translate, METH_VARARGS, "translate"},
    {"transform", (PyCFunction) Gepard__transform, METH_VARARGS, "transform"},
    {"setTransform", (PyCFunction) Gepard__setTransform, METH_VARARGS, "setTransform"},
    {"clearRect", (PyCFunction) Gepard__clearRect, METH_VARARGS, "clearRect"},
    {"fillRect", (PyCFunction) Gepard__fillRect, METH_VARARGS, "fillRect"},
    {"strokeRect", (PyCFunction) Gepard__strokeRect, METH_VARARGS, "strokeRect"},
    {"beginPath", (PyCFunction) Gepard__beginPath, METH_NOARGS, "beginPath"},
    {"fill", (PyCFunction) Gepard__fill, METH_NOARGS, "fill"},
    {"stroke", (PyCFunction) Gepard__stroke, METH_NOARGS, "stroke"},
    {"drawFocusIfNeeded", (PyCFunction) Gepard__drawFocusIfNeeded, METH_NOARGS, "drawFocusIfNeeded"},
    {"clip", (PyCFunction) Gepard__clip, METH_NOARGS, "clip"},
    {"isPointInPath", (PyCFunction) Gepard__isPointInPath, METH_VARARGS, "isPointInPath"},
    {"setFillColor", (PyCFunction) Gepard__setFillColor, METH_VARARGS, "setFillColor"},
    {NULL, NULL, 0, NULL}
};

/*
 * Gepard object and module definition
 */

static PyTypeObject pygepard_GepardType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "pygepard.Gepard",             /* tp_name */
    sizeof(pygepard_GepardObject), /* tp_basicsize */
    0,                             /* tp_itemsize */
    Gepard__dealloc,               /* tp_dealloc */
    0,                             /* tp_print */
    0,                             /* tp_getattr */
    0,                             /* tp_setattr */
    0,                             /* tp_compare */
    0,                             /* tp_repr */
    0,                             /* tp_as_number */
    0,                             /* tp_as_sequence */
    0,                             /* tp_as_mapping */
    0,                             /* tp_hash */
    0,                             /* tp_call */
    0,                             /* tp_str */
    0,                             /* tp_getattro */
    0,                             /* tp_setattro */
    0,                             /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,            /* tp_flags */
    "Gepard object",               /* tp_doc */
    0,                             /* tp_traverse */
    0,                             /* tp_clear */
    0,                             /* tp_richcompare */
    0,                             /* tp_weaklistoffset */
    0,                             /* tp_iter */
    0,                             /* tp_iternext */
    pClassMethods,                 /* tp_methods */
    0,                             /* tp_members */
    0,                             /* tp_getset */
    0,                             /* tp_base */
    0,                             /* tp_dict */
    0,                             /* tp_descr_get */
    0,                             /* tp_descr_set */
    0,                             /* tp_dictoffset */
    Gepard__init,                  /* tp_init */
    0,                             /* tp_alloc */
    Gepard__new,                   /* tp_new */
};

static PyMethodDef pModuleMethods[] = {
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC
initpygepard(void)
{
    if (PyType_Ready(&pygepard_GepardType) < 0)
        return;

    PyObject *pModule = Py_InitModule("pygepard", pModuleMethods);

    PyModule_AddObject(pModule, "Gepard", (PyObject *)&pygepard_GepardType);
}


int main(int argc, char **argv)
{
    Py_Initialize();
    Py_SetProgramName(argv[0]);

    initpygepard();

    Py_Finalize();
    return 0;
}


import gdb
import itertools
import re
import pickle

class ArrayIterator():
    def __init__(self, start, finish):
        self._item = start
        self._finish = finish
        self._count = 0

    def __iter__(self):
        return self

    def __next__(self):
        count = self._count
        self._count = self._count + 1
        if self._item == self._finish:
            raise StopIteration
        elt = self._item.dereference()
        self._item = self._item + 1
        return ('[%d]' % count, elt)

class DictItererator():
    def __init__(self, **fields):
        self._count = 0
        self._order = []
        self._children = {}
        for key, value in fields.items():
            self._order.append(key)
            self._children[key] = value

    def __iter__(self):
        return self

    def __next__(self):
        count = self._count
        self._count = self._count + 1
        if (count == len(self._children)):
            raise StopIteration
        child = self._order[count]
        value = self._children[child]
        return (f"{child}", value)

class AssgnNodePrinter():
    def __init__(self, val):
        self._val = val

    def children(self):
        size = self._val['Size']
        expr = self._val['Expr']
        return DictItererator(Size=size, Expr=expr)

    def to_string(self):
        type = self._val.dynamic_type
        isDecl= self._val['IsDecl']
        if isDecl:
            header = "Dec"
        else:
            header = "Assgn"
        name = self._val["Name"]
        return f"({header}) {name}"

    def display_hint(self):
        return 'assign'

class NumberExprNodePrinter():
    def __init__(self, val):
        self._val = val

    def to_string(self):
        type = self._val.dynamic_type
        val = self._val['Val']
        return f"({type}) Val = {val}"

    def display_hint(self):
        return 'num'

class ArrayNodePrinter():
    def __init__(self, val):
        self._val = val

    def children(self):
        return ArrayIterator(self._val['Entries']['_M_impl']['_M_start'],
                              self._val['Entries']['_M_impl']['_M_finish'])

    def to_string(self):
        type = self._val.dynamic_type
        return f"({type}) Entries"

    def display_hint(self):
        return 'arr'

class PrototypeNodePrinter():
    def __init__(self, val):
        self._val = val

    def children(self):
        return ArrayIterator(self._val['Args']['_M_impl']['_M_start'],
                              self._val['Args']['_M_impl']['_M_finish'])

    def to_string(self):
        name = self._val['Name']
        type = self._val.dynamic_type
        return f"({type}) {name} Args"

    def display_hint(self):
        return 'prototype'

class FunctionNodePrinter():
    def __init__(self, val):
        self._val = val

    def children(self):
        prototype = self._val['Prototype']
        body = self._val['Body']
        return DictItererator(Prototype=prototype, Body=body)

    def to_string(self):
        type = self._val.dynamic_type
        return None
        #return f"({type}):"

    def display_hint(self):
        return 'function'

class BinaryExprNodePrinter():
    def __init__(self, val):
        self._val = val

    def children(self):
        lhs = self._val['LHS']
        rhs = self._val['RHS']
        return DictItererator(LHS=lhs, RHS=rhs)

    def to_string(self):
        type = self._val.dynamic_type
        op = self._val['Op']
        op.cast(gdb.lookup_type("char"))
        return f"({type}) {op}"

    def display_hint(self):
        return 'function'
    
class NodePointerPrinter():
    def __init__(self, val):
        self._val = val

    def to_string(self):
        uniq_ptr_type = self._val.dynamic_type.name
        m = re.search("std::unique_ptr<(.*),.*>", uniq_ptr_type)
        base_type = m.group(1)
        ptr_type = gdb.lookup_type(base_type).pointer()
        ptr_val = self._val.cast(ptr_type)
        deref = ptr_val.dereference()
        real_type = deref.dynamic_type
        return deref.cast(real_type)

    def display_hint(self):
        return 'pointer'

# def node_lookup_function(val):
#     print(gdb.objfiles())
#     lookup_tag = val.type.tag
#     if lookup_tag is None:
#         return None
#     print(f"Tag is: {lookup_tag}\n")
#     regex = re.compile("^NumberExprNode$")
#     if regex.match(lookup_tag):
#         return NodePrinter(val)
#     regex = re.compile("^std::unique_ptr<.*>$")
#     if regex.match(lookup_tag):
#         return NodePointerPrinter(val)
#     regex = re.compile("^ArrayExprNode$")
#     if regex.match(lookup_tag):
#         return ArrayPrinter(val)
#     return None

def register_printers(objfile):
    import gdb.printing
    pp = gdb.printing.RegexpCollectionPrettyPrinter('myprinters')
    pp.add_printer('ArrayNodePrinter', "^ArrayExprNode$", ArrayNodePrinter)
    pp.add_printer('NodePointerPrinter', "^std::unique_ptr<.*>$", NodePointerPrinter)
    pp.add_printer('NumberExprNodePrinter', "^NumberExprNode$", NumberExprNodePrinter)
    pp.add_printer('AssgnNodePrinter', "^AssgnNode$", AssgnNodePrinter)
    pp.add_printer('FunctionNodePrinter', "^FunctionNodePrinter$", FunctionNodePrinter)
    pp.add_printer('BinaryExprNodePrinter', "^BinaryExprNodePrinter$", BinaryExprNodePrinter)
    gdb.printing.register_pretty_printer(gdb.objfiles()[0], pp, replace=True)
    # objfile.pretty_printers.append(node_lookup_function)


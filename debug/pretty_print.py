import gdb
import itertools
import re
import pickle

class NumberExprNodePrinter():

    def __init__(self, val):
        self._val = val

    def to_string(self):
        type = self._val.dynamic_type
        val = self._val['Val']
        return f"({type}) Val: {val}"

    def display_hint(self):
        return 'num'

class ArrayNodePrinter():

    class _iterator():

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
    
    def __init__(self, val):
        self._val = val

    def children(self):
        return self._iterator(self._val['Entries']['_M_impl']['_M_start'],
                              self._val['Entries']['_M_impl']['_M_finish'])

    def to_string(self):
        type = self._val.dynamic_type
        return f"({type}) Entries"

    def display_hint(self):
        return 'arr'

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
    gdb.printing.register_pretty_printer(gdb.objfiles()[0], pp, replace=True)
    pp.add_printer('ArrayNodePrinter', "^ArrayExprNode$", ArrayNodePrinter)
    pp.add_printer('NodePointerPrinter', "^std::unique_ptr<.*>$", NodePointerPrinter)
    pp.add_printer('NumberExprNodePrinter', "^NumberExprNode$", NumberExprNodePrinter)
    # objfile.pretty_printers.append(node_lookup_function)

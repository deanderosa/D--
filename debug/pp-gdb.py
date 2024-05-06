import gdb
import sys
sys.path.insert(1, '/mathworks/home/dderosa/Hobby/D--/debug')
import pretty_print

# python
# import sys
# sys.path.insert(0, '/usr/share/gcc-8/python')
# from libstdcxx.v6.printers import register_libstdcxx_printers
# register_libstdcxx_printers (None)
pretty_print.register_printers(gdb.current_progspace())

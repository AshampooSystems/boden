
import sys
from . import main

exit_code = main.do_main(sys.argv[1:])
sys.exit( exit_code )



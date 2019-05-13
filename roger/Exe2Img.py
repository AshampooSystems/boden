import os
import tempfile
import subprocess
from string import Template

def create(command, input_file, output_type):
    tf = tempfile.NamedTemporaryFile(mode='w+b', delete=False, suffix=".%s" %(output_type))

    command_template = Template(command)
    command = command_template.substitute(input=input_file, output=tf.name)

    subprocess.check_call(command, shell=True)

    return tf.name
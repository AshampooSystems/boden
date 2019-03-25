import os, sys, subprocess

output = subprocess.run([sys.executable, '-m', 'mkdocs', 'build', '-c'], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

print(output.stdout.decode("utf-8") )
print(output.stderr.decode("utf-8") )

if output.returncode != 0:
	print("Command exited with error")
	exit(output.returncode)

for line in output.stderr.decode("utf-8").split('\n'):
	if line.startswith("WARNING"):
		print("Failed due to: %s" % line)
		exit(1)
	if "The following pages exist in the docs directory, but are not included" in line:
		print("Failed due to non-referenced pages")
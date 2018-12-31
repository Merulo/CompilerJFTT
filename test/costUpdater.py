import re
import subprocess
import filecmp
import os
import sys

os.chdir("04.FourthIRTests")

regex1 = re.compile(r"\.result")

for path, subdirs, files in os.walk("."):
	for name in files:
		result = os.path.join(path, name)
		if regex1.search(result):
			with open(result) as f:
				value = [i for i in f.read().split('\n') if i][-1]
				target = result.replace(".result", ".target")
				lines = open(target).read().splitlines()
				lines[-1] = value
				open(target,'w').write('\n'.join(lines))


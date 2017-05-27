import sys, os, subprocess


proc = subprocess.Popen(['./server', '1'])

def stop():
	print('Stop')
	self.proc.kill()
	self.close()

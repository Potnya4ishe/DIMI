import sys, os, subprocess
import socket
import time

def run():
	proc = subprocess.Popen(['./client'])
	while True:
		exprToEval = sys.stdin.readline()
		print (exprToEval)
		if exprToEval == 'stop\n':
			stop(proc, client)
			return
		client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		client.connect(('localhost', 9999))
		print(client.send(bytes(exprToEval, 'UTF-8')), 'bytes sent.')
		time.sleep(0.2)
		print(client.recv(1024).decode('UTF-8')+'\n')
	

def stop(proc, client):
	client.send(bytes('666', 'UTF-8'))
	proc.kill()

run()
		
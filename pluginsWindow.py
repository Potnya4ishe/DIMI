from PyQt5.QtWidgets import QWidget, QPushButton, QLabel, QGridLayout, QLineEdit, QTextEdit
from PyQt5.QtCore import QCoreApplication, pyqtSignal, pyqtSlot


import socket
import subprocess
import time

class ClientMenu(QWidget):



	def __init__(self):
		super(QWidget, self).__init__()
		self.initUI()


	def initUI(self):
		windowWidth = 250
		windowHeight = 100
		windowPositionX = 300
		windowPositionY = 300
		lineDistance = 5

		grid = QGridLayout()
		grid.setSpacing(lineDistance)

		self.setWindowTitle('Client')


		self.exprToEval = QLineEdit('')

		#instructions = QLabel('Enter plugin\'s name:')

		self.runButton = QPushButton('Run', self)
		self.runButton.clicked.connect(self.run)
		
		self.closeButton = QPushButton('Stop', self)
		self.closeButton.clicked.connect(self.stop)

		#self.a = QTextEdit
		#self.b = 

		self.logOutput = QTextEdit()
		self.logOutput.setReadOnly(True)
		self.logOutput.setLineWrapMode(QTextEdit.NoWrap)

		self.resultLabel = QLabel('Results')

		font = self.logOutput.font()
		font.setFamily("Courier")
		font.setPointSize(10)

		grid.addWidget(self.exprToEval, 0, 0)
		grid.addWidget(self.runButton, 1, 0)
		grid.addWidget(self.closeButton, 4, 0)
		grid.addWidget(self.resultLabel, 2, 0)
		grid.addWidget(self.logOutput, 3, 0)
		self.setLayout(grid)
		self.resize(windowWidth, windowHeight)
		self.proc = subprocess.Popen(['./client'])
		self.show()

	def run(self):
		#self.logOutput.clear()
		# file = open("expr.txt", "w")
		# file.write(self.exprToEval.text())
		# file.close()
		# time.sleep(10)
		# file = open("res.txt", "r");
		# string = file.read()
		# file.close()
		# self.logOutput.insertPlainText(string+'\n')


		self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.client.connect(('localhost', 9999))
		print(self.client.send(bytes(self.exprToEval.text(), 'UTF-8')), 'bytes sent.')
		time.sleep(0.2)
		self.logOutput.insertPlainText(self.client.recv(1024).decode('UTF-8')+'\n')
		

	def stop(self):
		self.client.send(bytes('666', 'UTF-8'))
		self.proc.kill()
		self.close()



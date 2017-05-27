from PyQt5.QtWidgets import QWidget, QPushButton, QLabel, QGridLayout, QLineEdit
from PyQt5.QtCore import QCoreApplication, pyqtSignal, pyqtSlot

import sys, os
import json
import collections
import subprocess


class ServerWindow(QWidget):

	def __init__(self):
		super(QWidget, self).__init__()
		self.initUI()
		self.proc = subprocess.Popen(['./server', '1'])

	def stop(self):
		print('Stop')
		self.proc.kill()
		self.close()

	def initUI(self):

		windowWidth = 200
		windowHeight = 80
		windowPositionX = 300
		windowPositionY = 300
		lineDistance = 5

		grid = QGridLayout()
		grid.setSpacing(lineDistance)

		self.setWindowTitle('Server')


		self.running = QLabel('Running...')

		self.close_b = QPushButton('Stop', self)
		self.close_b.clicked.connect(self.stop)

		grid.addWidget(self.running, 1, 0)
		grid.addWidget(self.close_b, 1, 1)

		self.setLayout(grid)
		self.resize(windowWidth, windowHeight)

		self.show()


from PyQt5.QtWidgets import QWidget, QPushButton, QLabel, QGridLayout, QApplication
from PyQt5.QtCore import QCoreApplication, Qt, pyqtSignal, pyqtSlot
from PyQt5.QtGui import QFont, QPixmap

import sys, os, subprocess


from settingsWindow import *
from pluginsWindow import *
from errorWindow import *
from interface import *



class BinnaviPlugin(object):
	def __init__(self):
		print ("Running...")

		
	def runClient(self):	
		self.interface.openClientMenu()

	def runServer(self):
		#servers = self.interface.numbOfServers.text()
		self.interface.openServerWindow()

	def exit(self):
		self.interface.exit()

	def run(self):
		app = QApplication(sys.argv)
		self.interface = Interface()
		self.interface.clientButton.clicked.connect(self.runClient)
		self.interface.serverButton.clicked.connect(self.runServer)
		self.interface.exitButton.clicked.connect(self.exit)
		sys.exit(app.exec_())


plugin = BinnaviPlugin()
plugin.run()



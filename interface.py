from PyQt5.QtWidgets import QWidget, QPushButton, QLabel, QGridLayout, QDesktopWidget
from PyQt5.QtCore import QCoreApplication, Qt, pyqtSignal, pyqtSlot
from PyQt5.QtGui import QFont, QPixmap

from settingsWindow import *
from pluginsWindow import *


__version__ = 1.0

class Interface(QWidget):
	def __init__(self):
		super(QWidget, self).__init__()

		self.initUI()

	def openClientMenu(self):
		self.clientMenu = ClientMenu()

	def openServerWindow(self):
		self.pluginsWindow= ServerWindow()

	def exit(self):
		self.close()

	def initUI(self):

		windowPositionX = 150
		windowPositionY = 150
		windowWidth = 300
		windowHeight = 160
		lineDistance = 10

		self.setWindowTitle('DIMI' + str(__version__))

		grid = QGridLayout()
		grid.setSpacing(lineDistance)

		titleFont = QFont("Times", 16, QFont.Bold)
		statusFont = QFont("Comic Sans", 8, QFont.Bold)

		title = QLabel('Select mode')
		title.setFont(titleFont)

		#self.numbOfServers = QTextEdit()
		self.serverButton  = QPushButton('Server')
		self.clientButton = QPushButton('Client')
		self.exitButton = QPushButton('Exit')

		verticalSpace = QLabel("")

		grid.addWidget(title, 1, 1, alignment = Qt.AlignCenter)
		grid.addWidget(self.serverButton, 2, 1, alignment = Qt.AlignTop)
		#grid.addWidget(self.numbOfServers, 2, 2)
		grid.addWidget(self.clientButton, 3, 1, alignment = Qt.AlignTop)
		grid.addWidget(self.exitButton, 4, 1, alignment = Qt.AlignTop)
		grid.addWidget(verticalSpace, 6, 1, 6, -1)

		self.setLayout(grid)

		self.move(QDesktopWidget().availableGeometry().center()	- self.frameGeometry().center())
		self.resize(windowWidth, windowHeight)
		self.show()











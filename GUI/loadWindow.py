import sys
from PyQt5.QtWidgets import (QWidget, QProgressBar,
    QPushButton, QApplication)
from PyQt5 import QtCore
from PyQt5.QtCore import QThread
from PyQt5.QtCore import QProcess
#from PyQt5 import QString
import threading
import time


class LoadThread(QThread):

    def __init__(self):
        super(LoadThread, self).__init__()

    def __del__(self):
        self.wait()

    def run(self):
        loading = LoadWindow()
        time.sleep(10)



class LoadWindow(QWidget):

    def __init__(self, parent = None):
        super(QWidget, self).__init__()
        self.initUI()


    def initUI(self):

        self.pbar = QProgressBar(self)
        self.pbar.setGeometry(20, 20, 160, 20)

        self.pbar.setRange(0,0)
        self.pbar.setAlignment(QtCore.Qt.AlignCenter)

        #self.pbar.setFormat("Loading" )

        self.btn = QPushButton('Cancel', self)
        self.btn.setGeometry(70, 50, 60, 20)
        self.btn.clicked.connect(self.Cancel)


        self.setGeometry(300, 300, 200, 80)
        self.setWindowTitle('Loading...')
        self.show()

    def Cancel(self):
        self.close()


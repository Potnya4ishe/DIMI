from PyQt5.QtWidgets import QMessageBox, QWidget


class ErrorWindow(QMessageBox):

    def __init__(self, msg):
        super(QWidget, self).__init__()
        self.message = msg
        self.initUI()



    def initUI(self):
        self.setIcon(QMessageBox.Critical)
        self.setText(self.message)
        self.setWindowTitle("Can't load database")
        self.setStandardButtons(QMessageBox.Ok)
        self.show()


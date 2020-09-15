# Python and Arduino Oscilloscope
# James Ferguson

from PyQt5 import QtCore, QtGui, QtWidgets
import pyqtgraph as pg
import sys
import numpy as np
import serial

class serialGraphApp(QtGui.QMainWindow):
    def __init__(self):
        super().__init__()
        self.port = serial.Serial()
        self.initUi()

    def initUi(self):
        self.setWindowTitle("Oscilloscope")
        self.setFixedSize(1400, 300)
        self.btn = QtGui.QPushButton("Connect", self)
        self.btn.clicked.connect(self.startStop)
        self.btn.resize(100, 30)
        self.btn.move(0,30)
        self.collect = False

        self.btnTrig = QtGui.QPushButton("turn trigger on", self)
        self.btnTrig.clicked.connect(self.trig)
        self.btnTrig.resize(100, 30)
        self.btnTrig.move(0,90)
        self.trigger = False

        self.triggerOfset = 0

        self.comText = QtGui.QLabel("COM", self)
        self.comText.move(13, 1)

        self.comTextBox = QtGui.QLineEdit(self)
        self.comTextBox.resize(50, 30)
        self.comTextBox.move(50,0)

        self.zoom = QtGui.QSlider(QtCore.Qt.Horizontal, self)
        self.zoom.setRange(50, 2000)
        self.zoom.resize(100, 30)
        self.zoom.move(0, 60)
        self.zoom.setValue(1000)

        self.tSize = QtGui.QSlider(QtCore.Qt.Horizontal, self)
        self.tSize.setRange(5, 500)
        self.tSize.resize(100, 30)
        self.tSize.move(0, 120)
        self.tSize.setValue(30)

        self.plot = pg.PlotWidget(self)
        self.plot.hideAxis("bottom")
        self.plot.resize(1300, 300)
        self.plot.move(100, 0)
        self.data = []
        for i in range(2000):
           self.data.append(0)
        self.X = np.arange(2000)
        self.penn = pg.mkPen('g', style = QtCore.Qt.SolidLine)
        self.plot.setXRange(1000, 2000)
        self.plot.setYRange(0, 5)
        style = {"color": "#000", "font-size": "20px"}
        self.plot.setLabel("left", "Voltage", 'V', **style)

    def startStop(self):
        if self.collect:
            self.port.close()
            self.collect = False
            self.btn.setText("Connect")
            self.comTextBox.setReadOnly(False)
        else:
            try:
                self.port = serial.Serial('COM' + str(self.comTextBox.text()), 115200, timeout=0.5) 
                self.collect = True
                self.btn.setText("Stop")
                self.comTextBox.setReadOnly(True)
            except:
                pass
    
    def trig(self):
        if self.trigger:
            self.trigger = False
            self.triggerOfset = 0
            self.btnTrig.setText("turn trigger on")
        else:
            self.trigger = True
            self.btnTrig.setText("turn trigger off")

    def update(self):
        if self.collect:
            byte = self.port.read(30)
            for b in byte:
                num = int(b)
                self.data.append((num / 255) * 5)
                self.data.pop(0)
        if self.trigger:
            maxSlope = 0
            maxIndex = 0 
            for i in range(self.tSize.value()):
                if self.data[-i - 1] - self.data[-i -2] > maxSlope:
                    maxIndex = i
                    maxSlope = self.data[-i - 1] - self.data[-i - 2]
            self.triggerOfset = maxIndex
        self.plot.setXRange(2000 - self.zoom.value(), 2000)
        self.plot.plot(self.X + self.triggerOfset, self.data, pen = self.penn, clear = True)
        QtCore.QTimer.singleShot(1, self.update)

    def closeEvent(self, event):
        try:
            self.port.close()
        except:
            pass

app = QtGui.QApplication(sys.argv)
window = serialGraphApp()
window.show()
window.update()
app.exec_()

import wx
import numpy as np
from ui.panels import TrainingVisualizer

class MainFrame(wx.Frame):
    def __init__(self, parent, title):
        wx.Frame.__init__(self, parent, title=title, size=(1000, 600))
        self.InitUI()
        self.Show(True)

    def InitUI(self):
        visual = TrainingVisualizer(self)
        visual.SetPaintData(np.array([[0.1, 0.2, 0.3], [0.4, 0.5, 0.6], [0.7, 0.8, 0.9]]))

ex = wx.App(False)
MainFrame(None, "Training Control")
ex.MainLoop()
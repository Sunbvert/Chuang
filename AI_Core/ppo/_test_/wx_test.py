import wx
import numpy as np
from ui.panels import TrainingVisualizer, ActionLog, ParameterControl, Logs


class MainFrame(wx.Frame):
    def __init__(self, parent, title):
        wx.Frame.__init__(self, parent, title=title)
        self.InitUI()
        self.Show(True)

    def InitUI(self):
        self.panel = wx.Panel(self, wx.ID_ANY)
        topSizer = wx.GridSizer(2, 2, 2, 2)

        visual = TrainingVisualizer(self)
        action = ActionLog(self)
        control = ParameterControl(self)
        logs = Logs(self)

        topSizer.Add(visual, 0, wx.EXPAND)
        topSizer.Add(action, 0, wx.EXPAND)
        topSizer.Add(control, 0, wx.EXPAND)
        topSizer.Add(logs, 0, wx.EXPAND)

        self.panel.SetSizerAndFit(topSizer)
        topSizer.Fit(self)


ex = wx.App(False)
MainFrame(None, "Training Control")
ex.MainLoop()
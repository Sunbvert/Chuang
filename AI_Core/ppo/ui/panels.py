import wx
import math
import subprocess
import psutil
import os
import numpy as np


class ArrayVisualizer(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, size=(400, 400))
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.paintData = None
        self.pixelSize = 10

    def SetPaintData(self, data):
        self.paintData = data
        self.Refresh()

    def OnPaint(self, e):
        if self.paintData is None:
            print("ERROR: Have not set painting data!")
            return

        canvas = wx.PaintDC(self)
        brush = wx.Brush('white')
        canvas.SetBackground(brush)
        canvas.Clear()
        canvas.SetPen(wx.Pen(wx.Colour('white'), 0, wx.PENSTYLE_TRANSPARENT))

        for i in range(self.paintData.shape[0]):
            for j in range(self.paintData.shape[1]):
                canvas.SetBrush(wx.Brush(wx.Colour(int(255 * self.paintData[i][j]), 255, 255, 255)))
                canvas.DrawRectangle(j * self.pixelSize, i * self.pixelSize, self.pixelSize, self.pixelSize)


class TrainingVisualizer(wx.Panel):
    def __init__(self, parent, num_envs):
        wx.Panel.__init__(self, parent)
        self.num_envs = num_envs
        self.selectedEnvId = 0
        self.parent = parent
        self.InitUI()

    def InitUI(self):
        title = wx.StaticText(self, wx.ID_ANY, 'Training Visualizer')
        title.SetBackgroundColour(wx.Colour('red'))
        title.SetForegroundColour(wx.Colour('white'))
        topSizer = wx.BoxSizer(wx.VERTICAL)
        topSizer.Add(title, 0, wx.ALL | wx.EXPAND, 5)
        topSizer.Add(wx.StaticLine(self, ), 0, wx.ALL | wx.EXPAND, 5)

        self.choices = []
        for i in range(self.num_envs):
            self.choices.append('env' + str(i))

        self.envChoice = wx.Choice(self, wx.ID_ANY, choices=self.choices)
        self.envChoice.SetSelection(0)
        self.envChoice.Bind(wx.EVT_CHOICE, self.OnChoice)
        topSizer.Add(self.envChoice, 0, wx.ALL | wx.EXPAND, 5)

        self.visual = ArrayVisualizer(self)
        topSizer.Add(self.visual, 1, wx.ALL | wx.EXPAND, 5)

        self.SetSizerAndFit(topSizer)

    def SetVisualData(self, data):
        # TODO: 4o to be responsive
        data = np.reshape(data, (40, 40))
        self.visual.SetPaintData(data)

    def OnChoice(self, e):
        self.selectedEnvId = self.envChoice.GetSelection()
        self.parent.selectedEnvId = self.selectedEnvId
        self.parent.logs.Clear()
        self.parent.actionLog.Clear()


class ActionLog(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        self.InitUI()

    def InitUI(self):
        title = wx.StaticText(self, wx.ID_ANY, 'Actions')
        title.SetBackgroundColour(wx.Colour('red'))
        title.SetForegroundColour(wx.Colour('white'))
        topSizer = wx.BoxSizer(wx.VERTICAL)
        topSizer.Add(title, 0, wx.ALL | wx.EXPAND, 5)
        topSizer.Add(wx.StaticLine(self, ), 0, wx.ALL | wx.EXPAND, 5)

        inoutLabel = wx.StaticText(self, wx.ID_ANY, 'In                                          Out')
        topSizer.Add(inoutLabel)

        actionHeader = wx.StaticText(self, wx.ID_ANY, 'Waist | Knee | Ankle  ==>  Waist | Knee | Ankle | Award | Done')
        topSizer.Add(actionHeader)

        self.logText = wx.TextCtrl(self, wx.ID_ANY, 'Training has not yet started.', style=wx.TE_MULTILINE)
        topSizer.Add(self.logText, 1, wx.ALL | wx.EXPAND, 5)

        self.SetSizerAndFit(topSizer)

    def PushActionsIn(self, actions):
        separator = '|'
        actions = np.around(actions, 2)
        for i in range(actions.size):
            if i == actions.size - 1:
                separator = '|==>|'
            self.logText.write(str(actions[i]) + separator)

    def PushActionsOut(self, actions, reward, done):
        separator = '|'
        actions = np.around(actions, 2)
        for i in range(actions.size):
            if i == actions.size - 1:
                separator = '|' + str(reward) + '|' + str(done) + '\n'
            self.logText.write(str(actions[i]) + separator)

    def Clear(self):
        self.logText.SetValue('')

class ParameterControl(wx.Panel):
    def __init__(self, parent, settings, startFuc):
        wx.Panel.__init__(self, parent)
        self.settings = settings
        self.startFuc = startFuc
        self.parent = parent
        self.InitUI()

    def InitUI(self):
        title = wx.StaticText(self, wx.ID_ANY, 'Parameter Control')
        title.SetBackgroundColour(wx.Colour('red'))
        title.SetForegroundColour(wx.Colour('white'))
        topSizer = wx.BoxSizer(wx.VERTICAL)
        topSizer.Add(title, 0, wx.ALL | wx.EXPAND, 5)
        topSizer.Add(wx.StaticLine(self, ), 0, wx.ALL | wx.EXPAND, 5)

        self.connectionBtn = wx.Button(self, wx.ID_ANY, 'Connect RabbitMQ Server')
        self.connectionStatus = wx.StaticText(self, wx.ID_ANY, 'Not Connected')
        connSizer = wx.BoxSizer(wx.HORIZONTAL)
        connSizer.Add(self.connectionBtn, 0, wx.LEFT, 5)
        connSizer.Add((0, 0), 1)
        connSizer.Add(self.connectionStatus, 0, wx.RIGHT, 20)
        topSizer.Add(connSizer, 0, wx.EXPAND)

        for key, value in self.settings.items():
            label = wx.StaticText(self, wx.ID_ANY, key)
            editText = wx.TextCtrl(self, wx.ID_ANY, str(value), size=(200, 30))
            sizer = wx.BoxSizer(wx.HORIZONTAL)
            sizer.Add(label, 0, wx.LEFT, 20)
            sizer.Add((0, 0), 1)
            sizer.Add(editText, 0, wx.RIGHT, 20)
            topSizer.Add(sizer, 0, wx.EXPAND)

        self.startBtn = wx.Button(self, wx.ID_ANY, 'Start Training')
        self.startBtn.Bind(wx.EVT_BUTTON, self.startFuc)
        self.pauseBtn = wx.ToggleButton(self, wx.ID_ANY, 'Pause Training')
        self.pauseBtn.Bind(wx.EVT_TOGGLEBUTTON, self.OnPauseToggle)
        self.startEnvBtn = wx.Button(self, wx.ID_ANY, 'Start Environment')
        self.startEnvBtn.Bind(wx.EVT_BUTTON, self.OnStartEnv)

        btnSizer = wx.BoxSizer(wx.HORIZONTAL)
        btnSizer.Add(self.startEnvBtn, 0, wx.Right, 5)
        btnSizer.Add(self.startBtn, 0, wx.LEFT | wx.RIGHT, 5)
        btnSizer.Add(self.pauseBtn, 0, wx.LEFT, 5)

        topSizer.Add(btnSizer, 0, wx.ALIGN_CENTER_HORIZONTAL | wx.TOP | wx.BOTTOM, 20)

        self.SetSizerAndFit(topSizer)

    def OnPauseToggle(self, e):
        state = e.GetEventObject().GetValue()
        if state == True:
            self.parent.PauseTraining()
            e.GetEventObject().SetLabel('Resume Training')
        else:
            self.parent.ResumeTraining()
            e.GetEventObject().SetLabel('Pause Training')

    def OnStartEnv(self, e):
        cmd = 'cd /home/sunchuan/AI/Chuang/Build/bin/x86_64/Debug/ && ./Chuang'
        self.process = subprocess.Popen([cmd], shell=True)

class Logs(wx.Panel):
    def __init__(self, parent, messageQueue):
        wx.Panel.__init__(self, parent)
        self.messageQueue = messageQueue
        self.InitUI()

    def InitUI(self):
        title = wx.StaticText(self, wx.ID_ANY, 'Logs')
        title.SetBackgroundColour(wx.Colour('red'))
        title.SetForegroundColour(wx.Colour('white'))
        topSizer = wx.BoxSizer(wx.VERTICAL)
        topSizer.Add(title, 0, wx.ALL | wx.EXPAND, 5)
        topSizer.Add(wx.StaticLine(self, ), 0, wx.ALL | wx.EXPAND, 5)

        self.logArea = wx.TextCtrl(self, wx.ID_ANY, 'Training has not yet started.', style=wx.TE_MULTILINE)
        topSizer.Add(self.logArea, 1, wx.ALL | wx.EXPAND, 5)

        self.SetSizerAndFit(topSizer)

    def Log(self, message):
        self.logArea.write(message + '\n')

    def WriteLogsInQueue(self):
        while not self.messageQueue.empty():
            self.Log(self.messageQueue.get())

    def Clear(self):
        self.logArea.SetValue('')
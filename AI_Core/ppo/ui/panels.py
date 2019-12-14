import wx
import numpy as np


class ArrayVisualizer(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent, size=(500, 500))
        self.Bind(wx.EVT_PAINT, self.OnPaint)
        self.paintData = None
        self.pixelSize = 30

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
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        self.num_envs = 8
        self.InitUI()

    def InitUI(self):
        title = wx.StaticText(self, wx.ID_ANY, 'Training Visualizer')

        topSizer = wx.BoxSizer(wx.VERTICAL)
        topSizer.Add(title, 0, wx.ALL | wx.EXPAND, 5)
        topSizer.Add(wx.StaticLine(self, ), 0, wx.ALL | wx.EXPAND, 5)

        envChoice = wx.Choice(self, wx.ID_ANY, choices=['env2', 'env3'])
        topSizer.Add(envChoice, 0, wx.ALL | wx.EXPAND, 5)

        visual = ArrayVisualizer(self)
        visual.SetPaintData(np.array([[0.1, 0.2, 0.3], [0.4, 0.5, 0.6], [0.7, 0.8, 0.9]]))
        topSizer.Add(visual, 1, wx.ALL | wx.EXPAND, 5)

        self.SetSizerAndFit(topSizer)


class ActionLog(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        self.InitUI()

    def InitUI(self):
        title = wx.StaticText(self, wx.ID_ANY, 'Actions')

        topSizer = wx.BoxSizer(wx.VERTICAL)
        topSizer.Add(title, 0, wx.ALL | wx.EXPAND, 5)
        topSizer.Add(wx.StaticLine(self, ), 0, wx.ALL | wx.EXPAND, 5)

        inoutLabel = wx.StaticText(self, wx.ID_ANY, 'In                                          Out')
        topSizer.Add(inoutLabel)

        actionHeader = wx.StaticText(self, wx.ID_ANY, 'Waist | Knee | Ankle  ==>  Waist | Knee | Ankle | Award | Done')
        topSizer.Add(actionHeader)

        self.logText = wx.TextCtrl(self, wx.ID_ANY, 'Simulation has not yet begin', style=wx.TE_MULTILINE)
        topSizer.Add(self.logText, 1, wx.ALL | wx.EXPAND, 5)

        self.SetSizerAndFit(topSizer)

    def PushActionsIn(self, actions):
        separator = ' | '
        for i in range(actions.size):
            if i == actions.size - 1:
                separator = '  ==>  '
            self.logText.write(actions[i] + separator)

    def PushActionsOut(self, actions, reward, done):
        separator = ' | '
        for i in range(actions.size):
            if i == actions.size - 1:
                separator = '\n'
            self.logText.write(actions[i] + separator)


# NUM_ENVS            = 8
# #ENV_ID              = "Pendulum-v0"
# HIDDEN_SIZE         = 256
# LEARNING_RATE       = 1e-4
# GAMMA               = 0.99
# GAE_LAMBDA          = 0.95
# PPO_EPSILON         = 0.2
# CRITIC_DISCOUNT     = 0.5
# ENTROPY_BETA        = 0.001
# PPO_STEPS           = 256
# MINI_BATCH_SIZE     = 64
# PPO_EPOCHS          = 10
# TEST_EPOCHS         = 10
# NUM_TESTS           = 10
# TARGET_REWARD       = 8000

settings = {
    'NUM_ENVS': 8,
    'ENV_NAME': 'HopperRobot',
    'HIDDEN_SIZE': 256,
    'LEARNING_RATE': 1e-4,
    'GAMMA': 0.99,
    'GAE_LAMBDA': 0.95,
    'PPO_EPSILON': 0.2,
    'CRITIC_DISCOUNT': 0.5,
    'ENTROPY_BETA': 0.001,
    'PPO_STEPS': 256,
    'MINI_BATCH_SIZE': 64,
    'PPO_EPOCHS': 10,
    'TEST_EPOCHS': 10,
    'NUM_TESTS': 10,
    'TARGET_REWARD': 8000
}


class ParameterControl(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
        self.InitUI()

    def InitUI(self):
        title = wx.StaticText(self, wx.ID_ANY, 'ParameterControl')

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

        for key, value in settings.items():
            label = wx.StaticText(self, wx.ID_ANY, key)
            editText = wx.TextCtrl(self, wx.ID_ANY, str(value), size=(200, 30))
            sizer = wx.BoxSizer(wx.HORIZONTAL)
            sizer.Add(label, 0, wx.LEFT, 20)
            sizer.Add((0, 0), 1)
            sizer.Add(editText, 0, wx.RIGHT, 20)
            topSizer.Add(sizer, 0, wx.EXPAND)

        self.startBtn = wx.Button(self, wx.ID_ANY, 'Start Training')
        topSizer.Add(self.startBtn, 0, wx.ALIGN_CENTER_HORIZONTAL)

        self.SetSizerAndFit(topSizer)
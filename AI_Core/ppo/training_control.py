import wx
import time
import os
import numpy as np
from ui.panels import TrainingVisualizer, ActionLog, ParameterControl, Logs
from algorithms.ppo import PPO_Train
from multiprocessing import Process, Queue
from _test_.subprocess_test import subprocess_test


class MainFrame(wx.Frame):
    def __init__(self, parent, title):
        wx.Frame.__init__(self, parent, title=title)
        self.selectedEnvId = 0
        self.debugQueue = None
        self.InitUI()
        self.Show(True)


    def InitUI(self):
        self.panel = wx.Panel(self, wx.ID_ANY)
        topSizer = wx.GridSizer(2, 2, 2, 2)

        self.logQueue = Queue()
        self.ppo = PPO_Train(self.logQueue)
        self.logs = Logs(self, self.logQueue)
        self.logTimer = wx.Timer(self)
        self.Bind(wx.EVT_TIMER, self.OnLogTimer, self.logTimer)

        self.visual = TrainingVisualizer(self, 8)
        self.actionLog = ActionLog(self)
        self.control = ParameterControl(self, self.ppo.settings, self.StartTraining)

        topSizer.Add(self.visual, 0, wx.EXPAND)
        topSizer.Add(self.actionLog, 0, wx.EXPAND)
        topSizer.Add(self.control, 0, wx.EXPAND)
        topSizer.Add(self.logs, 0, wx.EXPAND)

        self.panel.SetSizerAndFit(topSizer)
        topSizer.Fit(self)

        self.logTimer.Start(1000)

    def InitTrainingProcess(self, f_path=''):
        self.logs.Clear()
        self.actionLog.Clear()

        self.debugQueue = Queue()
        self.pauseQueue = Queue()
        self.trainingProcess = Process(target=self.ppo.Start, args=(self.debugQueue, self.pauseQueue, f_path, ))

        self.debugInfoRefreshTimer = wx.Timer(self)
        self.Bind(wx.EVT_TIMER, self.OnDebugData, self.debugInfoRefreshTimer)

        self.trainingProcess.start()
        self.debugInfoRefreshTimer.Start(1000 / 60)


    def OnLogTimer(self, e):
        self.logs.WriteLogsInQueue()

    def OnDebugData(self, e):
        data = ()
        while not self.debugQueue.empty():
            data = self.debugQueue.get()
        if len(data) > 0:
            index = len(data[0][0]) - 3
            self.visual.SetVisualData(data[0][self.selectedEnvId][:index])
            # self.actionLog.PushActionsIn(data[0][self.selectedEnvId][index:])
            # self.actionLog.PushActionsOut(data[3][self.selectedEnvId], data[1][self.selectedEnvId], data[2][self.selectedEnvId])
            # if data[2][self.selectedEnvId]:
            #     self.actionLog.Clear()

    def PauseTraining(self):
        self.pauseQueue.put(1)
        self.logs.Log('Training Paused...')

    def ResumeTraining(self):
        self.pauseQueue.put(1)
        self.logs.Log('Training Resumed...')

    def StartTrainingFromFile(self, f_path):
        self.control.StartEnv()
        self.logQueue.put('Starting Environment, sleeping for 1 second...')
        time.sleep(1)
        self.InitTrainingProcess(f_path)

    def StartTraining(self, e):
        self.control.StartEnv()
        self.logQueue.put('Starting Environment, sleeping for 1 second...')
        time.sleep(1)
        self.InitTrainingProcess()

    def StopTraining(self):
        if self.debugQueue is not None:
            self.debugQueue.close()
            self.logQueue.close()
            self.trainingProcess.terminate()

ex = wx.App(False)
main = MainFrame(None, "Training Control")
ex.MainLoop()
main.StopTraining()

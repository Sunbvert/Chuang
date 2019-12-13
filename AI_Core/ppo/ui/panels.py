import wx
import numpy as np


class ArrayVisualizer(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)
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
        canvas.SetPen(wx.Pen(wx.Colour('white'), 0,  wx.PENSTYLE_TRANSPARENT))

        for i in range(self.paintData.shape[0]):
            for j in range(self.paintData.shape[1]):
                canvas.SetBrush(wx.Brush(wx.Colour(int(255 * self.paintData[i][j]), 255, 255, 255)))
                canvas.DrawRectangle(j * self.pixelSize, i * self.pixelSize, self.pixelSize, self.pixelSize)

class TrainingVisualizer(wx.Panel):
    def __init__(self, parent, array_shape):
        wx.Panel.__init__(self, parent)
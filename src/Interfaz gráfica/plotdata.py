# importing required modules
import matplotlib
matplotlib.use("TkAgg")
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import tkinter
import os

class plotdata:

    def __init__(self, frame):
        # creating an empty plot/frame
        if os.environ.get('DISPLAY', '') == '':
            print('no display found. Using :0.0')
            os.environ.__setitem__('DISPLAY', ':0.0')

        self.fig = plt.figure()
        self.update = True
        self.ax = plt.axes(xlim=(0, 100), ylim=(0, 100))
        self.ax.grid(True)
        self.lineSP, = self.ax.plot([], [], 'k-', label="SP", lw=2)
        self.linePV, = self.ax.plot([], [], 'b-', label="PV", lw=2)
        self.lineError, = self.ax.plot([], [], 'r-', label="Error", lw=2)
        self.lineCP, = self.ax.plot([], [], 'g-', label="CP", lw=2)
        self.xdata_SP = []
        self.ydata_SP = []
        self.xdata_PV = []
        self.ydata_PV = []
        self.xdata_Error = []
        self.ydata_Error = []
        self.xdata_CP = []
        self.ydata_CP = []
        canvas = FigureCanvasTkAgg(self.fig, master=frame)  # A tk.DrawingArea.
        canvas.draw()
        canvas.get_tk_widget().pack(side=tkinter.TOP, fill=tkinter.X, expand=0)

        # call the animator
        self.anim = animation.FuncAnimation(self.fig, self.animate, init_func=None,
                                       frames=500, interval=100, blit=False)


    def addPoint(self, point):
        self.xdata_SP.append(point[0][0])
        self.ydata_SP.append(point[0][1])
        self.xdata_PV.append(point[1][0])
        self.ydata_PV.append(point[1][1])
        self.xdata_Error.append(point[2][0])
        self.ydata_Error.append(point[2][1])
        self.xdata_CP.append(point[3][0])
        self.ydata_CP.append(point[3][1])

        self.update = True

    def getXDataSize(self):
        return len(plt.getp(self.lineSP, 'xdata'))

    def hide_show_line(self, states):
        if(self.xdata_SP):
            lines = [self.lineSP, self.linePV, self.lineError, self.lineCP]
            for i in range(len(states)):
                lines[i].set_visible(states[i])

    def clear(self):
        self.xdata_SP = []
        self.ydata_SP = []
        self.xdata_PV = []
        self.ydata_PV = []
        self.xdata_Error = []
        self.ydata_Error = []
        self.xdata_CP = []
        self.ydata_CP = []
        self.ax.set_xlim(0, 50)

    # animation function
    def animate(self, i):
        if self.update:
            self.lineSP.set_data(self.xdata_SP, self.ydata_SP)
            self.linePV.set_data(self.xdata_PV, self.ydata_PV)
            self.lineError.set_data(self.xdata_Error, self.ydata_Error)
            self.lineCP.set_data(self.xdata_CP, self.ydata_CP)
            if len(self.xdata_SP) > 1:
                i_min = 0
                i_max = len(self.xdata_SP)
                if self.xdata_SP[-1] > 50:
                    x_min = self.xdata_SP[-1] - 50
                    x_max = self.xdata_SP[-1]
                    i_min = i_max - 50
                    self.ax.set_xlim(x_min, x_max)
                y_max = max([max(self.ydata_SP[i_min:i_max]), max(self.ydata_PV[i_min:i_max]),
                                 max(self.ydata_Error[i_min:i_max]), max(self.ydata_CP[i_min:i_max]),105])
                y_min = min([min(self.ydata_SP[i_min:i_max]), min(self.ydata_PV[i_min:i_max]),
                                 min(self.ydata_Error[i_min:i_max]), min(self.ydata_CP[i_min:i_max]),0])
                self.ax.set_ylim(y_min, y_max)

        # return line object
        return self.lineSP, self.linePV, self.lineError, self.lineCP


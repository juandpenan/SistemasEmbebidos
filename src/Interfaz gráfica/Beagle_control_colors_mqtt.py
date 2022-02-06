# Hector Quintian
# Lab control plant v1.0


import matplotlib
matplotlib.use("TkAgg")
import plotdata
import time
import tkinter as tk
import os
import sys
import tkinter.ttk as ttk
import time
import paho.mqtt.client as mqtt

tic = time.time()  # A partir de aqui se medira el tiempo
client = mqtt.Client("Beagle")
t1 = 0
t2 = 0
change1 = False
change2 = False
on1 = False
on2 = False
broker_address = "10.20.30.242" 

def vp_start_gui():
    '''Starting point when module is the main routine.'''
    global root, SP, close, top
    if os.environ.get('DISPLAY', '') == '':
        print('no display found. Using :0.0')
        os.environ.__setitem__('DISPLAY', ':0.0')

    root = tk.Tk()
    root.protocol("WM_DELETE_WINDOW", destroy_beagle)
    top = beagle(root)
    startmqtt()
    root.mainloop()


def on_message(client, userdata, message):
    global t1, t2, change1, change2, on1, on2
    topic1_1 = "plant"+str(top.PlantTCombobox1.current()+1)+"/data"
    topic1_2 = "plant"+str(top.PlantTCombobox1.current()+1)+"/parameters"

    topic2_1 = "plant" + str(top.PlantTCombobox2.current() + 1) + "/data"
    topic2_2 = "plant" + str(top.PlantTCombobox2.current() + 1) + "/parameters"
    values = list(map(float, str(message.payload.decode("utf-8")).split(";")))

    if message.topic == topic1_1:
        points = [[t1,values[0]],[t1,values[1]],[t1,values[0]-values[1]],[t1,values[2]]]  #SP,PV,CP, time
        t1 += float(values[3])
        top.newgraph1.addPoint(points)
        if change1:
            checkbutton1()
            change1 = False

    if message.topic == topic1_2:
        top.SPEntry1_var.set(str(values[0]))
        top.kEntry1_var.set(str(values[1]))
        top.TiEntry1_var.set(str(values[2]))
        top.TdEntry1_var.set(str(values[3]))
        on1 = bool(values[4])
        change_status(on1, top.ON_OFFButton1)

    if message.topic == topic2_1:
        points = [[t2,values[0]],[t2,values[1]],[t2,values[0]-values[1]],[t2,values[2]]]  #SP,PV,Error,CP, time
        t2 += float(values[3])
        top.newgraph2.addPoint(points)
        if change2:
            checkbutton2()
            change2 = False

    if message.topic == topic2_2:
        top.SPEntry2_var.set(str(values[0]))
        top.kEntry2_var.set(str(values[1]))
        top.TiEntry2_var.set(str(values[2]))
        top.TdEntry2_var.set(str(values[3]))
        on2 = bool(values[4])
        change_status(on2, top.ON_OFFButton2)


def update_parameters1(): #send parameters from GUI 1 by mqtt topic: update_parameters
    update_parameters(top.PlantTCombobox1.current() + 1, str(top.SPEntry1_var.get()), str(top.kEntry1_var.get()),
                      str(top.TiEntry1_var.get()), str(top.TdEntry1_var.get()))


def update_parameters2(): #send parameters from GUI 2 by mqtt topic: update_parameters
    update_parameters(top.PlantTCombobox2.current() + 1, str(top.SPEntry2_var.get()), str(top.kEntry2_var.get()),
                      str(top.TiEntry2_var.get()), str(top.TdEntry2_var.get()))


def update_parameters(plant, sp, k, ti, td):
    global client
    topic = "plant" + str(plant) + "/update_parameters"
    client.publish(topic, (sp + ";" + k + ";" + ti + ";" + td))
    get_parameters(plant)


def get_parameters(plant):
    client.publish("plant" + str(plant) + "/get_parameters", " ")

def on_off1():
    global on1
    on1 = not on1
    change_status(on1, top.ON_OFFButton1)
    topic = "plant" + str(top.PlantTCombobox1.current()+1) + "/on_off"
    client.publish(topic, int(on1))
    get_parameters(top.PlantTCombobox1.current()+1)


def on_off2():
    global on2
    on2 = not on2
    change_status(on2, top.ON_OFFButton2)
    topic = "plant" + str(top.PlantTCombobox2.current()+1) + "/on_off"
    client.publish(topic, int(on2))
    get_parameters(top.PlantTCombobox2.current() + 1)


def change_status(on, button):
    if on:
        button.configure(image=top.imageon)
    else:
        button.configure(image=top.imageoff)


def startmqtt():
    global client
    client.connect(broker_address)
    client.on_message = on_message
    subscribemqtt(1)
    combo2_change()
    client.loop_start()


def unsubscribemqtt(plant):
    global client
    topic1 = "plant"+str(plant)+"/data"
    topic2 = "plant"+str(plant)+"/parameters"
    client.unsubscribe(topic1)
    client.unsubscribe(topic2)


def subscribemqtt(plant):
    global client
    topic1 = "plant"+str(plant)+"/data"
    topic2 = "plant"+str(plant)+"/parameters"
    client.subscribe(topic1)
    client.subscribe(topic2)
    get_parameters(plant)


def destroy_beagle():
    global root, close
    root.quit()
    root = None


def combo1_change(event=None):
    global top, t1, change1
    plant1 = top.PlantTCombobox1.current()+1
    plant2 = top.PlantTCombobox2.current()+1
    if plant1 != top.previous_selection1:
        top.newgraph1.clear()
        t1 = 0
        if plant1 != plant2:
            if plant2 != top.previous_selection1:
                unsubscribemqtt(top.previous_selection1)
            subscribemqtt(plant1)
        get_parameters(plant1)
        top.previous_selection1 = plant1
        change1 = True


def combo2_change(event=None):
    global top, t2, change2
    plant1 = top.PlantTCombobox1.current()+1
    plant2 = top.PlantTCombobox2.current()+1
    if plant2 != top.previous_selection2:
        top.newgraph2.clear()
        t2 = 0
        if plant1 != plant2:
            if plant1 != top.previous_selection2:
                unsubscribemqtt(top.previous_selection2)
            subscribemqtt(plant2)
        get_parameters(plant2)
        top.previous_selection2 = plant2
        change2 = True


def checkbutton1():
    global top
    states = list(map(bool,[top.spCheck1.get(), top.pvCheck1.get(), top.errorCheck1.get(), top.cpCheck1.get()]))
    top.newgraph1.hide_show_line(states)


def checkbutton2():
    global top
    states = list(map(bool,[top.spCheck2.get(), top.pvCheck2.get(), top.errorCheck2.get(), top.cpCheck2.get()]))
    top.newgraph2.hide_show_line(states)

class beagle:
    def __init__(self, top=None):
        '''This class configures and populates the toplevel window.
           top is the toplevel containing window.'''
        _bgcolor = '#d9d9d9'  # X11 color: 'gray85'
        _fgcolor = '#000000'  # X11 color: 'black'
        _compcolor = '#d9d9d9'  # X11 color: 'gray85'
        _ana1color = '#d9d9d9'  # X11 color: 'gray85'
        _ana2color = '#ececec'  # Closest X11 color: 'gray92'
        self.style = ttk.Style()
        if sys.platform == "win32":
            self.style.theme_use('winnative')
        self.style.configure('.', background=_bgcolor)
        self.style.configure('.', foreground=_fgcolor)
        self.style.configure('.', font="TkDefaultFont")
        self.style.map('.', background=
        [('selected', _compcolor), ('active', _ana2color)])

        script_dir = os.path.dirname(__file__)  # <-- absolute dir the script is in
        rel_path_off = "off2.gif"
        rel_path_on = "on.gif"
        abs_file_path_off = os.path.join(script_dir, rel_path_off)
        abs_file_path_on = os.path.join(script_dir, rel_path_on)
        self.imageoff = tk.PhotoImage(file=abs_file_path_off)
        self.imageoff = self.imageoff.subsample(6)
        self.imageon = tk.PhotoImage(file=abs_file_path_on)
        self.imageon = self.imageon.subsample(4)

        top.geometry("1000x550+176+197")
        top.minsize(72, 1)
        top.maxsize(1440, 809)
        top.resizable(0, 0)
        top.title("Control Center")
        top.configure(background="#d9d9d9")
        top.configure(highlightbackground="#d9d9d9")
        top.configure(highlightcolor="black")

        self.Frame1 = tk.Frame(top)
        self.Frame1.place(relx=0.0, rely=0.0, relheight=0.818, relwidth=0.5)
        self.Frame1.configure(relief='groove')
        self.Frame1.configure(borderwidth="2")
        self.Frame1.configure(relief="groove")
        self.Frame1.configure(background="#d9d9d9")
        self.Frame1.configure(highlightbackground="#d9d9d9")
        self.Frame1.configure(highlightcolor="black")

        self.Frame2 = tk.Frame(top)
        self.Frame2.place(relx=0.5, rely=0.0, relheight=0.818, relwidth=0.5)
        self.Frame2.configure(relief='groove')
        self.Frame2.configure(borderwidth="2")
        self.Frame2.configure(relief="groove")
        self.Frame2.configure(background="#d9d9d9")
        self.Frame2.configure(highlightbackground="#d9d9d9")
        self.Frame2.configure(highlightcolor="black")

        self.SPButton1 = tk.Button(top)
        self.SPButton1.place(relx=0.08, rely=0.927, height=22, width=41)
        self.SPButton1.configure(activebackground="#ececec")
        self.SPButton1.configure(activeforeground="#000000")
        self.SPButton1.configure(background="#d9d9d9")
        self.SPButton1.configure(foreground="#000000")
        self.SPButton1.configure(highlightbackground="#d9d9d9")
        self.SPButton1.configure(highlightcolor="black")
        self.SPButton1.configure(text='''SP''')
        self.SPButton1.configure(command=update_parameters1)

        self.SPEntry1 = tk.Entry(top)
        self.SPEntry1.place(relx=0.03, rely=0.927, height=25, relwidth=0.042)
        self.SPEntry1.configure(background="white")
        self.SPEntry1.configure(borderwidth="1")
        self.SPEntry1.configure(font="TkFixedFont")
        self.SPEntry1.configure(foreground="#000000")
        self.SPEntry1.configure(highlightbackground="#d9d9d9")
        self.SPEntry1.configure(highlightcolor="black")
        self.SPEntry1.configure(insertbackground="black")
        self.SPEntry1.configure(selectbackground="blue")
        self.SPEntry1.configure(selectforeground="white")
        self.SPEntry1_var = tk.StringVar()
        self.SPEntry1.configure(textvariable=self.SPEntry1_var)

        self.kEntry1 = tk.Entry(top)
        self.kEntry1.place(relx=0.21, rely=0.927, height=25, relwidth=0.035)
        self.kEntry1.configure(background="white")
        self.kEntry1.configure(borderwidth="1")
        self.kEntry1.configure(font="TkFixedFont")
        self.kEntry1.configure(foreground="#000000")
        self.kEntry1.configure(highlightbackground="#d9d9d9")
        self.kEntry1.configure(highlightcolor="black")
        self.kEntry1.configure(insertbackground="black")
        self.kEntry1.configure(selectbackground="blue")
        self.kEntry1.configure(selectforeground="white")
        self.kEntry1_var = tk.StringVar()
        self.kEntry1.configure(textvariable=self.kEntry1_var)

        self.kLabel1 = tk.Label(top)
        self.kLabel1.place(relx=0.216, rely=0.891, height=22, width=13)
        self.kLabel1.configure(activebackground="#f9f9f9")
        self.kLabel1.configure(activeforeground="black")
        self.kLabel1.configure(background="#d9d9d9")
        self.kLabel1.configure(foreground="#000000")
        self.kLabel1.configure(highlightbackground="#d9d9d9")
        self.kLabel1.configure(highlightcolor="black")
        self.kLabel1.configure(justify='left')
        self.kLabel1.configure(text='''k''')

        self.TiEntry1 = tk.Entry(top)
        self.TiEntry1.place(relx=0.26, rely=0.927, height=25, relwidth=0.035)
        self.TiEntry1.configure(background="white")
        self.TiEntry1.configure(borderwidth="1")
        self.TiEntry1.configure(font="TkFixedFont")
        self.TiEntry1.configure(foreground="#000000")
        self.TiEntry1.configure(highlightbackground="#d9d9d9")
        self.TiEntry1.configure(highlightcolor="black")
        self.TiEntry1.configure(insertbackground="black")
        self.TiEntry1.configure(selectbackground="blue")
        self.TiEntry1.configure(selectforeground="white")
        self.TiEntry1_var = tk.StringVar()
        self.TiEntry1.configure(textvariable=self.TiEntry1_var)

        self.TiLabel1 = tk.Label(top)
        self.TiLabel1.place(relx=0.266, rely=0.891, height=22, width=18)
        self.TiLabel1.configure(activebackground="#f9f9f9")
        self.TiLabel1.configure(activeforeground="black")
        self.TiLabel1.configure(background="#d9d9d9")
        self.TiLabel1.configure(foreground="#000000")
        self.TiLabel1.configure(highlightbackground="#d9d9d9")
        self.TiLabel1.configure(highlightcolor="black")
        self.TiLabel1.configure(justify='left')
        self.TiLabel1.configure(text='''Ti''')

        self.TdEntry1 = tk.Entry(top)
        self.TdEntry1.place(relx=0.31, rely=0.927, height=25, relwidth=0.04)
        self.TdEntry1.configure(background="white")
        self.TdEntry1.configure(borderwidth="1")
        self.TdEntry1.configure(font="TkFixedFont")
        self.TdEntry1.configure(foreground="#000000")
        self.TdEntry1.configure(highlightbackground="#d9d9d9")
        self.TdEntry1.configure(highlightcolor="black")
        self.TdEntry1.configure(insertbackground="black")
        self.TdEntry1.configure(selectbackground="blue")
        self.TdEntry1.configure(selectforeground="white")
        self.TdEntry1_var = tk.StringVar()
        self.TdEntry1.configure(textvariable=self.TdEntry1_var)

        self.TdLabel1 = tk.Label(top)
        self.TdLabel1.place(relx=0.316, rely=0.891, height=22, width=19)
        self.TdLabel1.configure(activebackground="#f9f9f9")
        self.TdLabel1.configure(activeforeground="black")
        self.TdLabel1.configure(background="#d9d9d9")
        self.TdLabel1.configure(foreground="#000000")
        self.TdLabel1.configure(highlightbackground="#d9d9d9")
        self.TdLabel1.configure(highlightcolor="black")
        self.TdLabel1.configure(justify='left')
        self.TdLabel1.configure(text='''Td''')

        self.UpdateButton1 = tk.Button(top)
        self.UpdateButton1.place(relx=0.36, rely=0.927, height=22, width=65)
        self.UpdateButton1.configure(activebackground="#ececec")
        self.UpdateButton1.configure(activeforeground="#000000")
        self.UpdateButton1.configure(background="#d9d9d9")
        self.UpdateButton1.configure(foreground="#000000")
        self.UpdateButton1.configure(highlightbackground="#d9d9d9")
        self.UpdateButton1.configure(highlightcolor="black")
        self.UpdateButton1.configure(text='''Update''')
        self.UpdateButton1.configure(command=update_parameters1)

        self.ON_OFFButton1 = tk.Button(top)
        self.ON_OFFButton1.place(relx=0.43, rely=0.92, height=32, width=70)
        self.ON_OFFButton1.configure(activebackground="#ececec")
        self.ON_OFFButton1.configure(activeforeground="#000000")
        self.ON_OFFButton1.configure(background="#e80202")
        self.ON_OFFButton1.configure(foreground="#000000")
        self.ON_OFFButton1.configure(highlightbackground="#d9d9d9")
        self.ON_OFFButton1.configure(highlightcolor="black")
        self.ON_OFFButton1.configure(text='''OFF''')
        self.ON_OFFButton1.configure(image=self.imageoff)
        self.ON_OFFButton1.configure(command=on_off1)

        self.ON_OFFButton2 = tk.Button(top)
        self.ON_OFFButton2.place(relx=0.93, rely=0.92, height=32, width=70)
        self.ON_OFFButton2.configure(activebackground="#ececec")
        self.ON_OFFButton2.configure(activeforeground="#000000")
        self.ON_OFFButton2.configure(background="#e80202")
        self.ON_OFFButton2.configure(foreground="#000000")
        self.ON_OFFButton2.configure(highlightbackground="#d9d9d9")
        self.ON_OFFButton2.configure(highlightcolor="black")
        self.ON_OFFButton2.configure(text='''OFF''')
        self.ON_OFFButton2.configure(image=self.imageoff)
        self.ON_OFFButton2.configure(command=on_off2)

        self.SPEntry2 = tk.Entry(top)
        self.SPEntry2.place(relx=0.53, rely=0.927, height=25, relwidth=0.042)
        self.SPEntry2.configure(background="white")
        self.SPEntry2.configure(borderwidth="1")
        self.SPEntry2.configure(font="TkFixedFont")
        self.SPEntry2.configure(foreground="#000000")
        self.SPEntry2.configure(highlightbackground="#d9d9d9")
        self.SPEntry2.configure(highlightcolor="black")
        self.SPEntry2.configure(insertbackground="black")
        self.SPEntry2.configure(selectbackground="blue")
        self.SPEntry2.configure(selectforeground="white")
        self.SPEntry2_var = tk.StringVar()
        self.SPEntry2.configure(textvariable=self.SPEntry2_var)

        self.SPButton2 = tk.Button(top)
        self.SPButton2.place(relx=0.59, rely=0.927, height=22, width=41)
        self.SPButton2.configure(activebackground="#ececec")
        self.SPButton2.configure(activeforeground="#000000")
        self.SPButton2.configure(background="#d9d9d9")
        self.SPButton2.configure(foreground="#000000")
        self.SPButton2.configure(highlightbackground="#d9d9d9")
        self.SPButton2.configure(highlightcolor="black")
        self.SPButton2.configure(text='''SP''')
        self.SPButton2.configure(command=update_parameters2)

        self.kEntry2 = tk.Entry(top)
        self.kEntry2.place(relx=0.71, rely=0.927, height=25, relwidth=0.035)
        self.kEntry2.configure(background="white")
        self.kEntry2.configure(borderwidth="1")
        self.kEntry2.configure(font="TkFixedFont")
        self.kEntry2.configure(foreground="#000000")
        self.kEntry2.configure(highlightbackground="#d9d9d9")
        self.kEntry2.configure(highlightcolor="black")
        self.kEntry2.configure(insertbackground="black")
        self.kEntry2.configure(selectbackground="blue")
        self.kEntry2.configure(selectforeground="white")
        self.kEntry2_var = tk.StringVar()
        self.kEntry2.configure(textvariable=self.kEntry2_var)

        self.TiEntry2 = tk.Entry(top)
        self.TiEntry2.place(relx=0.76, rely=0.927, height=25, relwidth=0.035)
        self.TiEntry2.configure(background="white")
        self.TiEntry2.configure(borderwidth="1")
        self.TiEntry2.configure(font="TkFixedFont")
        self.TiEntry2.configure(foreground="#000000")
        self.TiEntry2.configure(highlightbackground="#d9d9d9")
        self.TiEntry2.configure(highlightcolor="black")
        self.TiEntry2.configure(insertbackground="black")
        self.TiEntry2.configure(selectbackground="blue")
        self.TiEntry2.configure(selectforeground="white")
        self.TiEntry2_var = tk.StringVar()
        self.TiEntry2.configure(textvariable=self.TiEntry2_var)

        self.menubar = tk.Menu(top, font="TkMenuFont", bg=_bgcolor, fg=_fgcolor)
        top.configure(menu=self.menubar)

        self.TdEntry2 = tk.Entry(top)
        self.TdEntry2.place(relx=0.81, rely=0.927, height=25, relwidth=0.035)
        self.TdEntry2.configure(background="white")
        self.TdEntry2.configure(borderwidth="1")
        self.TdEntry2.configure(font="TkFixedFont")
        self.TdEntry2.configure(foreground="#000000")
        self.TdEntry2.configure(highlightbackground="#d9d9d9")
        self.TdEntry2.configure(highlightcolor="black")
        self.TdEntry2.configure(insertbackground="black")
        self.TdEntry2.configure(selectbackground="blue")
        self.TdEntry2.configure(selectforeground="white")
        self.TdEntry2_var = tk.StringVar()
        self.TdEntry2.configure(textvariable=self.TdEntry2_var)

        self.kLabel2 = tk.Label(top)
        self.kLabel2.place(relx=0.716, rely=0.891, height=22, width=13)
        self.kLabel2.configure(activebackground="#f9f9f9")
        self.kLabel2.configure(activeforeground="black")
        self.kLabel2.configure(background="#d9d9d9")
        self.kLabel2.configure(foreground="#000000")
        self.kLabel2.configure(highlightbackground="#d9d9d9")
        self.kLabel2.configure(highlightcolor="black")
        self.kLabel2.configure(justify='left')
        self.kLabel2.configure(text='''k''')

        self.TiLabel2 = tk.Label(top)
        self.TiLabel2.place(relx=0.766, rely=0.891, height=22, width=18)
        self.TiLabel2.configure(activebackground="#f9f9f9")
        self.TiLabel2.configure(activeforeground="black")
        self.TiLabel2.configure(background="#d9d9d9")
        self.TiLabel2.configure(foreground="#000000")
        self.TiLabel2.configure(highlightbackground="#d9d9d9")
        self.TiLabel2.configure(highlightcolor="black")
        self.TiLabel2.configure(justify='left')
        self.TiLabel2.configure(text='''Ti''')

        self.TdLabel2 = tk.Label(top)
        self.TdLabel2.place(relx=0.816, rely=0.891, height=22, width=19)
        self.TdLabel2.configure(activebackground="#f9f9f9")
        self.TdLabel2.configure(activeforeground="black")
        self.TdLabel2.configure(background="#d9d9d9")
        self.TdLabel2.configure(foreground="#000000")
        self.TdLabel2.configure(highlightbackground="#d9d9d9")
        self.TdLabel2.configure(highlightcolor="black")
        self.TdLabel2.configure(justify='left')
        self.TdLabel2.configure(text='''Td''')

        self.UpdateButton2 = tk.Button(top)
        self.UpdateButton2.place(relx=0.86, rely=0.927, height=22, width=65)
        self.UpdateButton2.configure(activebackground="#ececec")
        self.UpdateButton2.configure(activeforeground="#000000")
        self.UpdateButton2.configure(background="#d9d9d9")
        self.UpdateButton2.configure(foreground="#000000")
        self.UpdateButton2.configure(highlightbackground="#d9d9d9")
        self.UpdateButton2.configure(highlightcolor="black")
        self.UpdateButton2.configure(text='''Update''')
        self.UpdateButton2.configure(command=update_parameters2)

        self.SPLabel_color1 = tk.Label(top)
        self.SPLabel_color1.place(relx=0.03, rely=0.851, height=5, width=20)
        self.SPLabel_color1.configure(activebackground="#f9f9f9")
        self.SPLabel_color1.configure(activeforeground="black")
        self.SPLabel_color1.configure(background="#000000")
        self.SPLabel_color1.configure(foreground="#000000")
        self.SPLabel_color1.configure(highlightbackground="#d9d9d9")
        self.SPLabel_color1.configure(highlightcolor="black")

        self.SPCheckbutton1 = tk.Checkbutton(top)
        self.SPCheckbutton1.place(relx=0.05, rely=0.836, relheight=0.04
                                  , relwidth=0.063)
        self.SPCheckbutton1.configure(activebackground="#ececec")
        self.SPCheckbutton1.configure(activeforeground="#000000")
        self.SPCheckbutton1.configure(background="#d9d9d9")
        self.SPCheckbutton1.configure(foreground="#000000")
        self.SPCheckbutton1.configure(highlightbackground="#d9d9d9")
        self.SPCheckbutton1.configure(highlightcolor="black")
        self.SPCheckbutton1.configure(justify='left')
        self.SPCheckbutton1.configure(text='''SP''')
        self.SPCheckbutton1.configure(command=checkbutton1)
        self.spCheck1 = tk.IntVar(value=1)
        self.SPCheckbutton1.configure(variable=self.spCheck1)

        self.SPLabel_color2 = tk.Label(top)
        self.SPLabel_color2.place(relx=0.53, rely=0.851, height=5, width=20)
        self.SPLabel_color2.configure(activebackground="#f9f9f9")
        self.SPLabel_color2.configure(activeforeground="black")
        self.SPLabel_color2.configure(background="#000000")
        self.SPLabel_color2.configure(foreground="#000000")
        self.SPLabel_color2.configure(highlightbackground="#d9d9d9")
        self.SPLabel_color2.configure(highlightcolor="black")
        self.SPLabel_color2.configure(text='''Label''')

        self.SPCheckbutton2 = tk.Checkbutton(top)
        self.SPCheckbutton2.place(relx=0.55, rely=0.836, relheight=0.04
                                  , relwidth=0.063)
        self.SPCheckbutton2.configure(activebackground="#ececec")
        self.SPCheckbutton2.configure(activeforeground="#000000")
        self.SPCheckbutton2.configure(background="#d9d9d9")
        self.SPCheckbutton2.configure(foreground="#000000")
        self.SPCheckbutton2.configure(highlightbackground="#d9d9d9")
        self.SPCheckbutton2.configure(highlightcolor="black")
        self.SPCheckbutton2.configure(justify='left')
        self.SPCheckbutton2.configure(text='''SP''')
        self.SPCheckbutton2.configure(command=checkbutton2)
        self.spCheck2 = tk.IntVar(value=1)
        self.SPCheckbutton2.configure(variable=self.spCheck2)

        self.PVLabel_color1 = tk.Label(top)
        self.PVLabel_color1.place(relx=0.13, rely=0.851, height=5, width=20)
        self.PVLabel_color1.configure(activebackground="#f9f9f9")
        self.PVLabel_color1.configure(activeforeground="#2b00ff")
        self.PVLabel_color1.configure(background="#0008ff")
        self.PVLabel_color1.configure(foreground="#000000")
        self.PVLabel_color1.configure(highlightbackground="#d9d9d9")
        self.PVLabel_color1.configure(highlightcolor="black")

        self.PVCheckbutton1 = tk.Checkbutton(top)
        self.PVCheckbutton1.place(relx=0.15, rely=0.836, relheight=0.04
                                  , relwidth=0.063)
        self.PVCheckbutton1.configure(activebackground="#ececec")
        self.PVCheckbutton1.configure(activeforeground="#000000")
        self.PVCheckbutton1.configure(background="#d9d9d9")
        self.PVCheckbutton1.configure(foreground="#000000")
        self.PVCheckbutton1.configure(highlightbackground="#d9d9d9")
        self.PVCheckbutton1.configure(highlightcolor="black")
        self.PVCheckbutton1.configure(justify='left')
        self.PVCheckbutton1.configure(text='''PV''')
        self.PVCheckbutton1.configure(command=checkbutton1)
        self.pvCheck1 = tk.IntVar(value=1)
        self.PVCheckbutton1.configure(variable=self.pvCheck1)

        self.PVCheckbutton2 = tk.Checkbutton(top)
        self.PVCheckbutton2.place(relx=0.65, rely=0.836, relheight=0.04
                                  , relwidth=0.063)
        self.PVCheckbutton2.configure(activebackground="#ececec")
        self.PVCheckbutton2.configure(activeforeground="#000000")
        self.PVCheckbutton2.configure(background="#d9d9d9")
        self.PVCheckbutton2.configure(foreground="#000000")
        self.PVCheckbutton2.configure(highlightbackground="#d9d9d9")
        self.PVCheckbutton2.configure(highlightcolor="black")
        self.PVCheckbutton2.configure(justify='left')
        self.PVCheckbutton2.configure(text='''PV''')
        self.PVCheckbutton2.configure(command=checkbutton2)
        self.pvCheck2 = tk.IntVar(value=1)
        self.PVCheckbutton2.configure(variable=self.pvCheck2)

        self.PVLabel_color2 = tk.Label(top)
        self.PVLabel_color2.place(relx=0.63, rely=0.851, height=5, width=20)
        self.PVLabel_color2.configure(activebackground="#f9f9f9")
        self.PVLabel_color2.configure(activeforeground="#2b00ff")
        self.PVLabel_color2.configure(background="#0008ff")
        self.PVLabel_color2.configure(foreground="#000000")
        self.PVLabel_color2.configure(highlightbackground="#d9d9d9")
        self.PVLabel_color2.configure(highlightcolor="black")

        self.ErrorLabel_color1 = tk.Label(top)
        self.ErrorLabel_color1.place(relx=0.23, rely=0.851, height=5, width=20)
        self.ErrorLabel_color1.configure(activebackground="#f9f9f9")
        self.ErrorLabel_color1.configure(activeforeground="#2b00ff")
        self.ErrorLabel_color1.configure(background="#e80202")
        self.ErrorLabel_color1.configure(foreground="#000000")
        self.ErrorLabel_color1.configure(highlightbackground="#d9d9d9")
        self.ErrorLabel_color1.configure(highlightcolor="black")

        self.ErrorCheckbutton1 = tk.Checkbutton(top)
        self.ErrorCheckbutton1.place(relx=0.25, rely=0.836, relheight=0.04
                                     , relwidth=0.063)
        self.ErrorCheckbutton1.configure(activebackground="#ececec")
        self.ErrorCheckbutton1.configure(activeforeground="#000000")
        self.ErrorCheckbutton1.configure(background="#d9d9d9")
        self.ErrorCheckbutton1.configure(foreground="#000000")
        self.ErrorCheckbutton1.configure(highlightbackground="#d9d9d9")
        self.ErrorCheckbutton1.configure(highlightcolor="black")
        self.ErrorCheckbutton1.configure(justify='left')
        self.ErrorCheckbutton1.configure(text='''Error''')
        self.ErrorCheckbutton1.configure(command=checkbutton1)
        self.errorCheck1 = tk.IntVar(value=1)
        self.ErrorCheckbutton1.configure(variable=self.errorCheck1)

        self.ErrorLabel_color2 = tk.Label(top)
        self.ErrorLabel_color2.place(relx=0.73, rely=0.851, height=5, width=20)
        self.ErrorLabel_color2.configure(activebackground="#f9f9f9")
        self.ErrorLabel_color2.configure(activeforeground="#2b00ff")
        self.ErrorLabel_color2.configure(background="#e80202")
        self.ErrorLabel_color2.configure(foreground="#000000")
        self.ErrorLabel_color2.configure(highlightbackground="#d9d9d9")
        self.ErrorLabel_color2.configure(highlightcolor="black")

        self.ErrorCheckbutton2 = tk.Checkbutton(top)
        self.ErrorCheckbutton2.place(relx=0.75, rely=0.836, relheight=0.04
                                     , relwidth=0.063)
        self.ErrorCheckbutton2.configure(activebackground="#ececec")
        self.ErrorCheckbutton2.configure(activeforeground="#000000")
        self.ErrorCheckbutton2.configure(background="#d9d9d9")
        self.ErrorCheckbutton2.configure(foreground="#000000")
        self.ErrorCheckbutton2.configure(highlightbackground="#d9d9d9")
        self.ErrorCheckbutton2.configure(highlightcolor="black")
        self.ErrorCheckbutton2.configure(justify='left')
        self.ErrorCheckbutton2.configure(text='''Error''')
        self.ErrorCheckbutton2.configure(command=checkbutton2)
        self.errorCheck2 = tk.IntVar(value=1)
        self.ErrorCheckbutton2.configure(variable=self.errorCheck2)

        self.CPLabel_color1 = tk.Label(top)
        self.CPLabel_color1.place(relx=0.33, rely=0.851, height=5, width=20)
        self.CPLabel_color1.configure(activebackground="#f9f9f9")
        self.CPLabel_color1.configure(activeforeground="#2b00ff")
        self.CPLabel_color1.configure(background="#0ee815")
        self.CPLabel_color1.configure(foreground="#000000")
        self.CPLabel_color1.configure(highlightbackground="#d9d9d9")
        self.CPLabel_color1.configure(highlightcolor="black")

        self.CPCheckbutton1 = tk.Checkbutton(top)
        self.CPCheckbutton1.place(relx=0.35, rely=0.836, relheight=0.04
                                  , relwidth=0.063)
        self.CPCheckbutton1.configure(activebackground="#ececec")
        self.CPCheckbutton1.configure(activeforeground="#000000")
        self.CPCheckbutton1.configure(background="#d9d9d9")
        self.CPCheckbutton1.configure(foreground="#000000")
        self.CPCheckbutton1.configure(highlightbackground="#d9d9d9")
        self.CPCheckbutton1.configure(highlightcolor="black")
        self.CPCheckbutton1.configure(justify='left')
        self.CPCheckbutton1.configure(text='''CP''')
        self.CPCheckbutton1.configure(command=checkbutton1)
        self.cpCheck1 = tk.IntVar(value=1)
        self.CPCheckbutton1.configure(variable=self.cpCheck1)

        self.CPLabel_color2 = tk.Label(top)
        self.CPLabel_color2.place(relx=0.83, rely=0.851, height=5, width=20)
        self.CPLabel_color2.configure(activebackground="#f9f9f9")
        self.CPLabel_color2.configure(activeforeground="#2b00ff")
        self.CPLabel_color2.configure(background="#0ee815")
        self.CPLabel_color2.configure(foreground="#000000")
        self.CPLabel_color2.configure(highlightbackground="#d9d9d9")
        self.CPLabel_color2.configure(highlightcolor="black")

        self.CPCheckbutton2 = tk.Checkbutton(top)
        self.CPCheckbutton2.place(relx=0.85, rely=0.836, relheight=0.04
                                  , relwidth=0.063)
        self.CPCheckbutton2.configure(activebackground="#ececec")
        self.CPCheckbutton2.configure(activeforeground="#000000")
        self.CPCheckbutton2.configure(background="#d9d9d9")
        self.CPCheckbutton2.configure(foreground="#000000")
        self.CPCheckbutton2.configure(highlightbackground="#d9d9d9")
        self.CPCheckbutton2.configure(highlightcolor="black")
        self.CPCheckbutton2.configure(justify='left')
        self.CPCheckbutton2.configure(text='''CP''')
        self.CPCheckbutton2.configure(command=checkbutton2)
        self.cpCheck2 = tk.IntVar(value=1)
        self.CPCheckbutton2.configure(variable=self.cpCheck2)

        self.PlantTCombobox1 = ttk.Combobox(top)
        self.PlantTCombobox1.place(relx=0.425, rely=0.836, relheight=0.049
                                   , relwidth=0.07)
        self.value_list = ['Plant 1', 'Plant 2', 'Plant 3', 'Plant 4', 'Plant 5', 'Plant 6']
        self.PlantTCombobox1.configure(values=self.value_list)
        self.PlantTCombobox1.configure(state='readonly')
        self.PlantTCombobox1.configure(takefocus="")
        self.previous_selection1 = 0
        self.PlantTCombobox1.current(self.previous_selection1)
        self.PlantTCombobox1.bind("<<ComboboxSelected>>", combo1_change)

        self.PlantTCombobox2 = ttk.Combobox(top)
        self.PlantTCombobox2.place(relx=0.925, rely=0.836, relheight=0.049
                                   , relwidth=0.07)
        self.value_list = ['Plant 1', 'Plant 2', 'Plant 3', 'Plant 4', 'Plant 5', 'Plant 6']
        self.PlantTCombobox2.configure(values=self.value_list)
        self.PlantTCombobox2.configure(state='readonly')
        self.PlantTCombobox2.configure(takefocus="")
        self.previous_selection2 = 0
        self.PlantTCombobox2.current(self.previous_selection2)
        self.PlantTCombobox2.bind("<<ComboboxSelected>>", combo2_change)

        self.newgraph1 = plotdata.plotdata(self.Frame1)
        self.newgraph1.ax.set_xlim(0, 50)

        self.newgraph2 = plotdata.plotdata(self.Frame2)
        self.newgraph2.ax.set_xlim(0, 50)
if __name__ == '__main__':
    vp_start_gui()






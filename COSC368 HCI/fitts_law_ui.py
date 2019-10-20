from tkinter import *
from tkinter.ttk import *
import random
import itertools
import time
import csv

master = Tk()
c = Canvas(master, width=1000, height=800)
c.pack()

distances = [64, 128, 256, 512]
widths = [4, 8, 16, 32]
tuples = list(itertools.product(distances, widths))
random.shuffle(tuples)

logfile = 'log.txt'
Var = IntVar()
Var.set(1)

def swap(flag):
    if not flag and Var.get()%2 == 0:
        c.itemconfigure(green, fill='green')
        c.itemconfigure(blue, fill='blue')
        Var.set(Var.get()+1)
    elif flag and Var.get()%2 != 0:
        c.itemconfigure(green, fill='blue')
        c.itemconfigure(blue, fill='green')
        Var.set(Var.get()+1)

with open(logfile, mode='w') as log:
    log = csv.writer(log, delimiter='\t', quotechar='"', quoting=csv.QUOTE_MINIMAL)
    for tuple in tuples:
        distance, width = tuple
        while Var.get() <= 4:
            left_start = (1000 - distance - width)/2
            right_start = (1000 + distance - width)/2
            if 'blue' in locals() and 'green' in locals():
                c.coords(blue, left_start, 0, left_start + width, 0)
                c.coords(green, right_start, 0, right_start + width, 0)
            else:
                blue = c.create_rectangle(left_start, 0, left_start + width, 0, fill="blue")
                green = c.create_rectangle(right_start, 0, right_start + width, 0, fill="green")
                c.tag_bind(green, "<ButtonPress-1>", swap(True))
                c.tag_bind(blue, "<ButtonPress-1>", swap(False))
            
            start = time.time()
            master.wait_variable(Var)
            total_time = (time.time() - start) * 1000
            log.writerow(['Frankler', distance, width, Var.get(), total_time])

master.mainloop()

from tkinter.ttk import *
from tkinter import *
import random
import time
import csv
import sys

# run only from IDLE
if __name__ == "__main__":
    sys.argv = [sys.argv[0], 'dynamic']

def generateBoard() :
    alphabet = 'qwertyuiopasdfghjklzxcvbnm'
    alphabet = ''.join(random.sample(alphabet,len(alphabet)))
    board = [alphabet[0:10], alphabet[10:19], alphabet[19:]]

    try:
        board_frame.destroy()
    except:
        pass
    board_frame = Frame(main_frame, borderwidth=2, relief=RAISED)
    board_frame.grid(row=1, column=0)
    # for each row in board, create a row frame inside frame.
    for row in board:
        row_frame = Frame(board_frame)
        row_frame.pack()

        # for each character in row, create button inside row frame
        for char in row:
            button_frame = Frame(row_frame, height=32, width=32)
            button_frame.pack_propagate(0) # don't shrink
            button_frame.pack(side=LEFT)
            button = Button(button_frame, text=char, command=lambda x=char: Var.set(x))
            button.pack(fill=BOTH, expand=1)


window = Tk()
s = Style()
s.configure('TButton', font='helvetica 12')
Var = StringVar()
mode = sys.argv[1]

main_frame = Frame(window)
main_frame.pack(fill=BOTH, padx=5, pady=5)

label = Label(main_frame, text="", font=('helvetica', 12), background='white')
label.grid(row=0, column=0, sticky=EW, padx=2, pady=10)

generateBoard()

logfile = 'experiment_'+mode+'_log.txt'
alphabet = 'qwertyuiopasdfghjklzxcvbnm'
with open(logfile, mode='w') as log:
    log = csv.writer(log, delimiter='\t', quotechar='"', quoting=csv.QUOTE_MINIMAL)
    n = 6
    target = ''.join(random.sample(alphabet,6))
    while n>0:
        target = random.sample(target,len(target)) # randomise
        for char in target:
            label['text'] = char
            start = time.time()
            while Var.get() != char:
                window.wait_variable(Var)
            if mode == 'dynamic':
                generateBoard()
            total_time = (time.time() - start) * 1000
            log.writerow(['Frankler', 'static', char, n, total_time])
        n-=1
    label['text'] = 'Thank you, program completed!'



main_frame.columnconfigure(0, weight=1)
window.mainloop()

import tkinter as tk
from tkinter import ttk
import serial
import threading
import time
import matplotlib.pyplot as plt

class SerialPlotter:
    def __init__(self, root):
        self.root = root
        self.root.title("Serial Plotter")

        # Configuração da porta serial
        self.serial_port = serial.Serial('COM3', 115200, timeout=1)

        # Frames
        self.frame_control = ttk.Frame(root)
        self.frame_control.pack(padx=10, pady=10)

        self.frame_plot = ttk.Frame(root)
        self.frame_plot.pack(padx=10, pady=(0, 10))

        # Labels e entradas
        ttk.Label(self.frame_control, text="Tempo de Leitura (s):").grid(row=0, column=0, padx=5, pady=5)
        self.entry_duration = ttk.Entry(self.frame_control)
        self.entry_duration.grid(row=0, column=1, padx=5, pady=5)
        self.entry_duration.insert(0, "5")  # Valor padrão

        # Botão de Leitura
        self.btn_read = ttk.Button(self.frame_control, text="Ler Dados", command=self.read_data)
        self.btn_read.grid(row=0, column=2, padx=5, pady=5)

        # Plot
        self.figure, self.ax = plt.subplots()
        self.line, = self.ax.plot([], [], 'b-')
        self.ax.set_xlabel('Tempo (s)')
        self.ax.set_ylabel('Dado')
        self.ax.set_title('Dados da Porta Serial')
        #self.canvas = FigureCanvasTkAgg(self.figure, self.frame_plot)
        self.canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=True)
        self.canvas.draw()

    def read_data(self):
        duration = int(self.entry_duration.get())
        data_x, data_y = [], []

        start_time = time.time()
        while (time.time() - start_time) < duration:
            try:
                line = self.serial_port.readline().decode().strip()
                if line:
                    x, y = map(float, line.split(','))  # Supondo que os dados sejam enviados como "x,y"
                    data_x.append(x)
                    data_y.append(y)
            except Exception as e:
                print("Erro ao ler dados da porta serial:", e)
                break

        self.plot_data(data_x, data_y)

    def plot_data(self, data_x, data_y):
        self.line.set_xdata(data_x)
        self.line.set_ydata(data_y)
        self.ax.relim()
        self.ax.autoscale_view(True, True, True)
        self.canvas.draw()

if __name__ == "__main__":
    root = tk.Tk()
    app = SerialPlotter(root)
    root.mainloop()

import serial
import matplotlib.pyplot as plt
from time import sleep, time
import numpy as np

# Configurações da porta serial (ajuste conforme necessário)
porta_serial = 'COM6'  # Porta serial do Arduino
baudrate = 115200  # Taxa de transmissão em baud

valores = []  # Lista para armazenar os valores recebidos da porta serial


# Função para inicializar a conexão serial
def iniciar_conexao_serial():
    try:
        serial_port = serial.Serial(porta_serial, baudrate, timeout=1)
        return serial_port
    except serial.SerialException as e:
        print("Erro ao abrir a porta serial:", e)
        return None

# Função para ler dados da porta serial
def read_csv(serial_port, start_msg):
    
    # arduino reinicia ao conectar na serial...
    sleep( 2 )
    #serial_port.write( "move 100".encode())
    #sleep( 0.3 )
    serial_port.write( "pwm.max 200".encode())
    sleep( 0.5 )
    serial_port.write( "kp 1.2".encode())
    sleep( 0.5 )
    serial_port.write( "ki 15".encode())
    sleep( 0.5 )
    serial_port.write(start_msg.encode())
    
    # Ler e imprimir as leituras da porta serial
    start = False
    result = ""

    while( True ):
      
      line = serial_port.readline().decode('utf-8', errors='replace').strip()
      #if( line[5] == "BEGIN" )

      if line:
        valores = line.split()
        if( valores[0] == "BEGIN" ):
            start = True
            print( "BEGIN" )
            continue
        elif( valores[0] == "END" ):
            start = False
            print( "END" )
            break
        if( start ):
            print( line )
            result += ( line + '\n' )
    print(result)
    return result

def call( speed = 100, count = 100, Io = 0 ):
    serial_port = iniciar_conexao_serial()
    if serial_port:
        result = read_csv(serial_port, f"test {speed} {count} {Io}\n" )
        print( "--------------------------" )
        print( result )
        print( "--------------------------" )
        path = "C:/Users/UERJBotz/Documents/LF18/(0) Mestrado/artigos/sbai2024/motor_magico/"
        file = path + f"resultado_PI_{speed}_{count}_{Io}.txt"
        f = open( file, "w")
        f.write( result )
        f.close()
        f = open(file, "r")
        print(f.read())
        return result


# Função para plotar o gráfico
def plot( csv, tipo = "speed", save = False ):

    lines = csv.splitlines()
    data = np.array(np.genfromtxt( lines, delimiter='\t', dtype=float, names=True, encoding=None, skip_header=0))
    data = np.array([list(row) for row in data])
    print(lines[0])
    print( data.transpose() )
    t0 = data.transpose()[1][0]
    t  = data.transpose()[1] - t0

    # Plotar os gráficos
    plt.figure(figsize=(16, 10))
    plt.rcParams.update({'font.size': 18})

    if( tipo == "speed" ):
        plt.title('Gráfico dos Dados da Porta Serial')
        input_raw  = data.transpose()[3]
        input  = data.transpose()[4]
        set    = data.transpose()[2]
        out    = data.transpose()[-1]
        plt.plot( t, input, label="speed" )
        plt.plot( t, input_raw, label="raw" )
        plt.plot( t, set, label="set" )
        plt.plot( t, out, label="out" )
        plt.ylabel('medidas')
    elif( tipo == "ind" ):
        plt.title( 'Tensão induzida medida com PWM = 100' )
        input_raw  = data.transpose()[3]
        input      = data.transpose()[4]
        set        = data.transpose()[2]
        out        = data.transpose()[-1]
        plt.plot( t, (1023.0/20.0)*input_raw, 'b', label="sem filtro" )
        plt.plot( t, (1023.0/20.0)*input, 'r', label="filtrado" )
        #plt.plot( t, set, label="set" )
        #plt.plot( t, out, label="out" )
        plt.ylabel('Tensão induzida medida [V]')
    plt.xlabel('tempo [ms]')
    plt.grid(True)
    plt.show()

if __name__ == '__main__':
    #csv = call( 205, 300, 0 ) #4V
    #csv = call( 0, 200, 153.235 )
    #csv = call( 51, 300, 0 ) #1V
    csv = call( 0, 200, 0 )
    #csv = call( 0, 180, 153.235 )
    plot( csv, tipo="speed" )

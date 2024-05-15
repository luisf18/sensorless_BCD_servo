import serial
import matplotlib.pyplot as plt

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
def ler_dados(serial_port):
    #try:
    # Envie a mensagem "test" para iniciar a transmissão de dados
    from time import sleep
    sleep( 2 )
    serial_port.write(b'test\n')
    sleep( 0.1 )
    serial_port.flushInput()
    # Ler e imprimir as leituras da porta serial
    while True:
        
        linha = serial_port.readline().decode('utf-8').strip()
        
        if linha:
            global valores
            valores += [ int( linha ) ]
            print(linha,valores[-1])
        else:
            serial_port.close()
            plotar_grafico(valores)  # Descomente esta linha para plotar o gráfico
            return
    #except KeyboardInterrupt:
        #pass
    #finally:
        #serial_port.close()

# Função para plotar o gráfico
def plotar_grafico(valores):
    plt.plot(valores)
    plt.xlabel('Amostras')
    plt.ylabel('Valor')
    plt.title('Gráfico dos Dados da Porta Serial')
    plt.grid(True)
    plt.show()

if __name__ == '__main__':
    serial_port = iniciar_conexao_serial()
    if serial_port:
        ler_dados(serial_port)

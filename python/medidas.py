import matplotlib.pyplot as plt
import numpy as np

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
    plt.figure(figsize=(12, 6))
    plt.rcParams.update({'font.size': 18})

    if( tipo == "speed" ):
        plt.title( 'Tensão induzida controlada $V_{ind} = 1V$' )
        input_raw  = data.transpose()[3]
        input  = data.transpose()[4]
        set    = data.transpose()[2]
        out    = data.transpose()[-1]
        #plt.plot( t, input, label="Tensão induzida" )
        plt.plot( t, (20.0/1023.0)*input_raw, label="$V_{ind}$ sem filtro [V]" )
        plt.plot( t, (20.0/1023.0)*input, label="$V_{ind}$ filtrado [V]" )
        plt.plot( t, (20.0/1023.0)*set, label="$V_{ind}$ = 1V" )
        plt.plot( t, (5.0/255.0)*out, label="sinal do atuador [V]" )
        #plt.ylabel('medidas')
    elif( tipo == "ind" ):
        plt.title( 'Tensão induzida medida com PWM = 100' )
        input_raw  = data.transpose()[3]
        input      = data.transpose()[4]
        set        = data.transpose()[2]
        out        = data.transpose()[-1]
        plt.plot( t, (20.0/1023.0)*input_raw, 'b', label="sem filtro" )
        plt.plot( t, (20.0/1023.0)*input, 'r', label="filtrado" )
        #plt.plot( t, set, label="set" )
        #plt.plot( t, out, label="out" )
        plt.ylabel('Tensão induzida medida [V]')
    elif( tipo == "pos" ):
        #plt.title( 'Girar 45° (uma volta)' )
        input_raw  = data.transpose()[3]
        input      = data.transpose()[4]
        set        = data.transpose()[2]
        out        = data.transpose()[-1]
        I          = data.transpose()[-2]
        #plt.plot( t, (20.0/1023.0)*input_raw, label="$V_{ind}$ sem filtro [V]" )
        #plt.plot( t, (20.0/1023.0)*input, label="$V_{ind}$ filtrado [V]" )
        #plt.plot( t, (20.0/1023.0)*input, label="$V_{ind}$ filtrado [V]" )
        plt.plot( t, -(0*set), label="angulo de referência [°]" )
        plt.plot( t, -360*I/153.235, label="angulo estimado por integração [°]" )

        #plt.plot( t, (5.0/255.0)*out, label="sinal do atuador [V]" )
        #plt.ylabel('Angulo estimado')

    plt.legend()
    plt.xlabel('tempo [ms]')
    plt.grid(True)
    plt.show()


speed = 0 #51 #205
count = 100
Io = 19.154375 #0#153.235
path = "C:/Users/UERJBotz/Documents/LF18/(0) Mestrado/artigos/sbai2024/motor_magico/"
#file = path + f"resultado_PI_{speed}_{count}_{Io}.txt"
file = path + f"resultado_PI_50pwm_pertuba90.txt"
f = open(file, "r")

#plot( str(f.read()), tipo="speed" )
plot( str(f.read()), tipo="pos" )

print(f.read())
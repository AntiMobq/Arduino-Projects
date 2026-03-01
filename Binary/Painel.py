import serial
import time

arduino = serial.Serial('COM5', 9600, timeout=1)
time.sleep(2)

def explicar_byte(n):
    bin_str = format(n, '016b')
    msb = bin_str[:8]
    lsb = bin_str[8:]
    
    print("\n" + "="*40)
    print(f"DECIMAL: {n}")
    print(f"BINÁRIO: {msb} | {lsb}")
    print(f"         (MSB/Escravo)  (LSB/Mestre)")
    print("="*40)

while True:
    try:
        val = input("\nDigite um valor (0-65535) ou 'bin 1010...' : ")
        
        if val.startswith('bin '):
            n = int(val.replace('bin ', ''), 2)
        else:
            n = int(val)
            
        if 0 <= n <= 65535:
            explicar_byte(n)
            arduino.write(f"{n}\n".encode())
        else:
            print("Valor fora da faixa")
    except ValueError:
        print("Sla")
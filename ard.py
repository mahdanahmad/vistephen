import serial
ser = serial.Serial("COM3", 115200)

def arduino_read():
	global x, y, angle, force
    x = y = angle = force = 0.0
    line_done = 0

    # request data by sending a dot
    ser.write(".")
    #while not line_done:
    line = ser.readline() 
    angles = line.split(", ")
    if len(angles) == 4:    
        x = float(angles[0])
        y = float(angles[1])
        angle = float(angles[2])
		force = float(angles[3])
        line_done = 1 
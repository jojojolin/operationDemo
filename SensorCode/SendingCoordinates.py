'''
This file helps interact with the UR3 over socket programming
'''

# Echo client program
import socket
import time
import serial
import json
#from ast import literal_eval


HOST = "192.168.8.105" # IP of this laptop
PORT = 30000 # port for this server 
count = 0

def socket_communication():
    f = open("C:\\Users\\Room One\\Desktop\\RoomOne\\OperationDemo\\SensorCode\\Coordinates.txt","r")
    message = f.read()
    delay = 0.8 #average
    try:
        tup = eval(message)
        delay = tup[7]
    except Exception:
        print ("EOF exception!")
 
    print(message)
    print(delay)
    global count
    global HOST
    global PORT
    try:
      s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
      s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
      s.bind((HOST, PORT)) # Bind to the port 
      s.listen(5) # Now wait for client connection.
      print("Waiting for client connection...")
      c, addr = s.accept() # Establish connection with client.
      print("Connected by the robot")
      msg = c.recv(1024)
      print("Pose Position = ", msg)
      time.sleep(delay)
      if (msg.decode('utf-8') == "sensor_val"):
        count = count + 1
      print("")
      c.send((message).encode());
      f.close()
    except socket.error as socketerror:
      print(count)
    except KeyboardInterrupt:
        f.close()
        raise

    


def main():
    try:
        while True:
            socket_communication()
        print("Program finish")
    except KeyboardInterrupt:
        raise
    

if __name__  == "__main__":
    print("Starting Program")
    main()

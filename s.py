# -*- coding: utf-8 -*
import serial
import time
import binascii

def getMessageContent(msg):
  Rows = msg.split("OK+ANCS:")
  content = ""
  for row in Rows:
    if row[0:2] == "OK":
      continue
    #print row
    content += row[2:]
  content = content[8:]
  print content

ser = serial.Serial("/dev/ttyAMA0",9600)
def main():
    while True:
        count = ser.inWaiting()
        if count != 0:
            recv = ser.read(count)
            #print("接收到字符串:"+recv)
            #print binascii.b2a_hex(recv)
            
            pos = recv.find("OK+ANCS8")
            if pos >= 0: 
              msgID         = recv[pos+12:pos+16]
              EventID       = recv[pos+8:pos+9]
              CategoryID    = recv[pos+9:pos+10]
              CategoryCount = recv[pos+10:pos+12]

              #print "msgID          ="+msgID
              #print "EventID        ="+EventID
              #print "CategoryID     ="+CategoryID
              #print "CategoryCount  ="+CategoryCount

              if EventID == "0":
                ser.flushInput()
                ser.write("AT+ANCS"+msgID+"199")
                time.sleep(0.5)
                count = ser.inWaiting()
                recv = ser.read(count)
                #print("接收到字符串-1:"+recv)
                getMessageContent(recv)
                #print binascii.b2a_hex(recv)


                ser.flushInput()
                ser.write("AT+ANCS"+msgID+"299")
                time.sleep(0.5)
                count = ser.inWaiting()
                recv = ser.read(count)
                #print("接收到字符串-2:"+recv)
                getMessageContent(recv)
                #print binascii.b2a_hex(recv)

                ser.flushInput()
                ser.write("AT+ANCS"+msgID+"399")
                time.sleep(0.5)
                count = ser.inWaiting()
                recv = ser.read(count)
                #print("接收到字符串-3:"+recv)
                getMessageContent(recv)
                #print binascii.b2a_hex(recv)

              #ser.write("AT+ANCS"+msgID+"299")
        ser.flushInput()
        time.sleep(0.5)

if __name__ == '__main__':
    try:
       main()
    except KeyboardInterrupt:
       if ser != None:
           ser.close()


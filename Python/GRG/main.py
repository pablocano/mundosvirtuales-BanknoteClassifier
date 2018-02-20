# -*- coding: utf-8 -*-
"""
Created on Thu Feb 15 14:53:51 2018

@author: David
"""
import xml.etree.cElementTree as ET
import time
import socket

#XML tag
DEVICE = 0
ERROR = 1
STACKERLIST = 2
REJECTLIST = 3
FEEDERLIST  = 4

#ERROR
CODE = 0
DESCRIPTION = 1

#Rejected
REJECT = 0

#Feeder
FEEDER = 0

#Header server
MAGIC_NUMBER = (0x87).to_bytes(2, 'little')
WRITE_REG = (0x10).to_bytes(2, 'little')
READ_REG = (0x40).to_bytes(2,'little')
ID_PACKAGE = (1).to_bytes(4, 'little')
REG_ROBOT_STATE = (81).to_bytes(4, 'little')

# Dict of GRG states
Device_dict = {'READY' : 1, 'BUSY' : 2 ,'ERROR' : 3} #State of device
Stacker_dict = {'EMPTY' : 0, 'EXIST' : 1, 'FULL': 2} #State of stackers



class GRG:
     
    def __init__(self,name,ip, port, f_register, timeout):
        self.name = name
        self.ip = ip
        self.port = port
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.settimeout(timeout)
        self.registers = {'reg_device' : f_register, 'reg_error' : f_register + 1, 
                          'reg_stack' : f_register + 2, 'reg_reject' : f_register + 3, 
                          'reg_feeder' : f_register + 4}
        self.states = {'state_device' : 0, 'state_error' : 0, 'state_stack' : 0, 'state_reject' : 0,
                       'state_feeder' : 0}
    
    def connect(self):
        print('Connecting to {0} ({1}, {2})'.format(self.name, self.ip, self.port))
        try:
            (self.socket).connect((self.ip,self.port))
        except socket.timeout:
            print('TIMEOUT')
        except socket.error:
            print('Couldnt connect with {0}'.format(self.name))

    
    def send(self,msg):
        try:
            (self.socket).sendall(msg.encode())
        except KeyboardInterrupt:
            print('Cancelled')
        except socket.error:
            print('Cant send')
        
    def close(self):
        self.socket.close()
        
    def handle_machine(self, msg):
        command = """<CsmRequest><Command>""" + msg + """</Command></CsmRequest>"""
        self.send(command)



#To create header of server
def create_msg(msg, reg, cmd):
    PAYLOAD= b""
    PAYLOAD += MAGIC_NUMBER
    PAYLOAD += cmd
    PAYLOAD += ID_PACKAGE
    PAYLOAD += reg
    PAYLOAD += (msg).to_bytes(44,'little')

    return(PAYLOAD)

#To decode xml from GRG and send to server
def send_data(tree, server, client):

    #State of device
    client.states['state_device'] = Device_dict[tree[DEVICE].text]
    
    #State of stacker
    client.states['state_stack'] = 0
    for child in tree[STACKERLIST]:
        if child.attrib['State'] == 'FULL':
            client.states['state_stack'] = int(child.attrib['ID'])
            break
    
    #State of rejected zone
    client.states['state_reject'] = Stacker_dict[tree[REJECTLIST][REJECT].attrib['State']]
    
    #State of feeder
    client.states['state_feeder'] = Stacker_dict[tree[FEEDERLIST][FEEDER].attrib['State']]
    
    for tag in client.states:
        print('{0} : {1}'.format(tag , client.states[tag]))

     
    #Create messages
    for state,register in zip(client.states, client.registers):
        print(client.states[state])
        try:
            server.send(create_msg(client.states[state],client.registers[register].to_bytes(4,'little'), WRITE_REG))
            time.sleep(0.05)
        except socket.error:
            print('Couldnt send to Server')
        except socket.timeout:
            print('Timeout - Server doesnt respond')
       
        
    
def check_robot_state(server):
    server.send(create_msg(' ', REG_ROBOT_STATE, READ_REG)) #Ask state of robot
    robot_resp = (server.socket.recv(1024)) # Get robot response
    print(robot_resp)
    
    
    
    



if __name__ == "__main__":
    
    #Server
    server = GRG('server','127.0.0.1', 3333, 0, 3)
    
    #Create devices
    cm800 = GRG('cm800', '10.0.42.88', 5000, 10, 3)
    #cm400 = GRG('cm400', '10.0.42.77', 5000, 15)
    
    #add devices to list
    devices = [cm800]
    
    #Connet to server
    server.connect()
      
    #Connect to devices
    for item in devices:
        item.connect()
        #item.handle_machine('pause_count') #To stop machine until exist banknotes
               
    while True:
        try:
            for item in devices:
                # sending xml_data            
                print('Send request to {0}'.format(item.name))
                item.handle_machine('stackers')

                # Get response
                resp = (item.socket).recv(1024)
                
                #Create xml tree    
                resp_tree = ET.fromstring(resp)
                
                print('Response {0} : {1}'.format(item.name, resp_tree.tag))
                
                #Read/Send data to server
                #check_robot_state(server)
                send_data(resp_tree, server.socket, item)
                
        # catch socket errors     
        except KeyboardInterrupt:
            break
        except socket.timeout:
            print("No response - Timeout {0} - Retrying...".format(item.name))

    print('Close Socket')
    server.close()       
    for item in devices:
        item.close()
        
    
    
            
    
    
    
    
    
    
# -*- coding: utf-8 -*-
"""
Created on Thu Feb 08 11:40:00 2018

@author: David
"""

import socket
import xml.etree.cElementTree as ET
import numpy as np
import time

MAGIC_NUMBER = (0x87).to_bytes(2, 'little')
WRITE_REG = (0x10).to_bytes(2, 'little')
ID_PACKAGE = (1).to_bytes(4, 'little')


#State device
reg_state = (10).to_bytes(4, 'little')
#Feeder
reg_feeder = (11).to_bytes(4,'little')
#Stacker
reg_stacker2 = (12).to_bytes(4,'little')
#Rejected
reg_rejected = (13).to_bytes(4,'little')


Device_dict = {'READY' : 1, 'BUSY' : 2 ,'ERROR' : 3}
Stacker_dict = {'EMPTY' : 0, 'EXIST' : 1, 'FULL': 2}


def create_msg(msg, reg):
    PAYLOAD= b""
    PAYLOAD += MAGIC_NUMBER
    PAYLOAD += WRITE_REG
    PAYLOAD += ID_PACKAGE
    PAYLOAD += reg
    PAYLOAD += (msg).to_bytes(44,'little')

    return(PAYLOAD)


def send_data(tree, sock):
    #get info
    
    #State of device
    state_device = tree[0].text
    
    #State of stacker 2
    stacker_2 = tree[2][1].attrib['State']
    
    #State of rejected zone
    rejected = tree[3][0].attrib['State']
    
    #State of feeder
    feeder = tree[4][0].attrib['State']
    
    print("state device {0} ; stacker 2 {1} ; rejected {2} ; feeder {3}".format(state_device, 
          stacker_2, rejected, feeder))
    
    
    #Create messages
    send_state = create_msg(Device_dict[state_device], reg_state)
    sock.send(send_state)
    
    send_feeder = create_msg(Stacker_dict[feeder], reg_feeder)
    sock.send(send_feeder)
    
    send_stack = create_msg(Stacker_dict[stacker_2], reg_stacker2)
    sock.send(send_stack)
    
    send_reject = create_msg(Stacker_dict[rejected], reg_rejected)
    sock.send(send_reject)    
    

def main(IP, PORT):
    
    #Create xml message"
    command = """<CsmRequest><Command>stackers</Command></CsmRequest>"""
    
    # Creating a socket TCP/IP
    sock_grg = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock_server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    
    # Connecting socket
    grg_address = ('10.0.42.77', 5000)
    server_adress = ('127.0.0.1',3333)
    
    print('Connecting to CM400 (ip: %s, port: %s)' % grg_address)
    print('Connecting to Server (ip: %s, port: %s)' % server_adress)
    
    sock_grg.connect(grg_address)
    sock_server.connect(server_adress)
    
    
    while True:
        try:  
            # sending xml_data
            print('Send request "%s"' % command)
            sock_grg.sendall(command.encode())
            
            
            # Get response
            resp = sock_grg.recv(1024)
            
            #Create xml tree    
            resp_tree = ET.fromstring(resp)
            
            print('Response CM400 "%s"' % resp_tree.tag)
            
            #Send data to server
            send_data(resp_tree, sock_server)
            
            #Sleep
            time.sleep(0.2)
            
        
        # catch socket errors     
        except socket.error:
            print('Couldnt connect with the socket-server, terminating test')
            
        except socket.timeout:
            print('Timeout error')
            
        except KeyboardInterrupt:
            break

    print('Close Socket')
        
    sock_grg.close()
    sock_server.close()
    print("Bye")
        

    
if __name__ == "__main__":
    main('10.0.42.77',5000)   
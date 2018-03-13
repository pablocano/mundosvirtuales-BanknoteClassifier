# -*- coding: utf-8 -*-
"""
Created on Tue Feb 20 18:19:10 2018

@author: David
"""
import socket
import time
import grg_variables as var

#To create header of server
def create_msg(msg, reg, cmd):
    PAYLOAD= b""
    PAYLOAD += var.MAGIC_NUMBER
    PAYLOAD += cmd
    PAYLOAD += var.ID_PACKAGE
    PAYLOAD += reg
    PAYLOAD += (msg).to_bytes(44,'little')

    return(PAYLOAD)



#To decode xml from GRG and send to server
def send_data(tree, server, client):

    #State of device
    client.states['state_device'] = (var.device_dict)[tree[var.DEVICE].text]
    
    #State of stacker
    client.states['state_stack'] = 0
    for child in tree[var.STACKERLIST]:
        if child.attrib['State'] == 'FULL':
            client.states['state_stack'] = int(child.attrib['ID'])
            break
    
    #State of rejected zone
    client.states['state_reject'] = (var.stacker_dict)[tree[var.REJECTLIST][var.REJECT].attrib['State']]
    
    #State of feeder
    client.states['state_feeder'] = (var.stacker_dict)[tree[var.FEEDERLIST][var.FEEDER].attrib['State']]
    
    #for tag in client.states:
        #print('{0} : {1}'.format(tag , client.states[tag]))

     
    #Create messages
    for state,register in zip(client.states, client.registers):
        try:
            server.send(create_msg(client.states[state],client.registers[register].to_bytes(4,'little'), var.WRITE_REG))
            resp_ = server.recv(1024)
            #print(resp_[2:4])
            #print(resp_server)
            time.sleep(0.05)
        except socket.error:
            print('Couldnt send to Server')
        except socket.timeout:
            print('Timeout - Server doesnt respond')
            
            
#To check position of robot and start/stop GRG
def check_robot_state(server, item):
    try:
        server.socket.send(create_msg(0, var.REG_ROBOT_STATE, var.READ_REG))
        server_response = server.socket.recv(56)
        
        server_msg = int.from_bytes(server_response[12:], byteorder = 'little')
        
        #server_msg indicate position of robot
        
        #Leave notes in feeder
        if server_msg == 1:
            print("Robot leave notes in feeder")
            item.handle_machine('start_count')

        elif server_msg == 2:
            print("Robot taking notes")
            item.handle_machine('pause_count')
        server.socket.send(create_msg(0, var.REG_ROBOT_STATE, var.WRITE_REG))
        server.socket.recv(56)
        
        return True

    except socket.error:
        print("error server")
        return False
        
    except socket.timeout:
        print("timeout server")
        return False

        
        
def error(tree):
    if ((var.device_dict)[tree[var.DEVICE].text]) == 3:
        print("Error code  = {0}. Description = {1}".format(tree[var.ERROR][var.CODE], tree[var.ERROR][var.DESCRIPTION]))
        return True
    else:
        return False
        
    
    
        
    
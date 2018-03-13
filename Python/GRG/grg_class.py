# -*- coding: utf-8 -*-
"""
Created on Tue Feb 20 18:19:10 2018

@author: David
"""
import socket

class GRGClass:
     
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
            return (self.socket).recv(1024)
        except socket.error:
            print('Cant send')
            return ''
        except socket.timeout:
            print('Timeout')
            return ''
        
    def close(self):
        self.socket.close()
        
        
    def handle_machine(self, msg):
        command = """<CsmRequest><Command>""" + msg + """</Command></CsmRequest>"""
        return self.send(command)
    
    
# -*- coding: utf-8 -*-
"""
Created on Thu Feb 15 14:53:51 2018

@author: David
"""
import xml.etree.cElementTree as ET
from grg_class import GRGClass
import grg_functions as fn



if __name__ == "__main__":
    
    #Server
    server = GRGClass('server','127.0.0.1', 3333, 0, 3)
    
    #Create devices
    cm800 = GRGClass('cm800', '10.0.42.88', 5000, 10, 3)
    #cm400 = GRG('cm400', '10.0.42.77', 5000, 15)
    
    #add devices to list
    devices = [cm800]
    
    #Connet to server
    server.connect()
      
    #Connect to devices
    for item in devices:
        item.connect()
               
    while True:
        try:
            for item in devices:
                # getting xml_data            
                print('Send request to {0}'.format(item.name))
                resp = item.handle_machine('stackers')
                
                #If GRG respond
                if resp != '':
                    
                    #Create xml tree    
                    resp_tree = ET.fromstring(resp)
                    
                    print('Response {0} : {1}'.format(item.name, resp_tree[0].text))
                    
                    #Check error state GRG
                    if not (fn.error(resp_tree)): 
                        #Check robot position
                        server_ok = fn.check_robot_state(server, item)
                        
                        if server_ok:
                            #Send data to robot from GRG
                            fn.send_data(resp_tree, server.socket, item)
                else:
                    print("{0} doesnt respond".format(item.name))
                    
        # catch socket errors     
        except KeyboardInterrupt:
            break

    print('Close Socket')
    server.close()       
    for item in devices:
        item.close()
        
    
    
            
    
    
    
    
    
    
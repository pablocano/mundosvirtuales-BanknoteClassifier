# -*- coding: utf-8 -*-
"""
Created on Thu Feb 22 11:22:25 2018

@author: David
"""

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
device_dict = {'READY' : 1, 'BUSY' : 2 ,'ERROR' : 3} #State of device
stacker_dict = {'EMPTY' : 0, 'EXIST' : 1, 'FULL': 2} #State of stackers

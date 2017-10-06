# -*- coding: utf-8 -*-
"""
Created on Wed Oct 04 10:03:02 2017

@author: PabloCano
"""

import yaml
import cv2
import numpy as np
import cPickle

class PCKeyPoint(yaml.YAMLObject):
    yaml_tag = u"tag:yaml.org,2002:PCKeypoint5"
    def __init__(self, keyPoint):
        self.pos = [keyPoint.pt[0],keyPoint.pt[1]]
        self.size = keyPoint.size
        self.angle = keyPoint.angle
        self.response = keyPoint.response
        self.octave = keyPoint.octave
        self.class_id = keyPoint.class_id
    def __repr__(self):
         return "%s(pos=%r, size=%r, angle=%r, response=%r, octave=%r, class_id=%r)" % (self.__class__.__name__, self.pos, self.size, self.angle, self.response, self.octave, self.class_id)

# A yaml representer is for dumping structs into a yaml node.
# So for an opencv_matrix type (to be compatible with c++'s FileStorage) we save the rows, cols, type and flattened-data  
class PCMat(yaml.YAMLObject):
    yaml_tag = u"tag:yaml.org,2002:opencv-matrix"
    def __init__(self, mat):
        self.rows = int(mat.shape[0])
        self.cols = int(mat.shape[1])
        self.dt = 'd'
        self.data = mat.reshape(-1).tolist()
    def __repr__(self):
         return "%s(rows=%r, cols=%r, dt=%r, data=%r)" % (self.__class__.__name__, self.rows, self.cols, self.dt, self.data)
     
gt_name = ['1000_Cara', '1000_Sello', '2000_Cara', '2000_Sello', '10000_Cara', '10000_Sello', '5000_Cara', '5000_Sello']

for name in gt_name:
    
    stream = file('../Data/keypoints/ckp' + name + '.yaml', 'w')
    
    stream.write("%YAML:1.0\n")
    
    #Read the pickle
    aux_list = cPickle.loads(open("../Data/keypoints/kp3" + name + ".txt").read())
    kp_original = list()
    des_original = np.float32(np.zeros((len(aux_list),128)))
    i = 0
    for point in aux_list:
        temp = cv2.KeyPoint(x=point[0][0],y=point[0][1],_size=point[1], _angle=point[2], _response=point[3], _octave=point[4], _class_id=point[5]) 
        
        kp_original.append(PCKeyPoint(temp))
        des_original[i] = point[6]
        i += 1
        
    yaml.dump({"keypoints": kp_original, "descriptors": PCMat(des_original)},stream)
    stream.close()
    
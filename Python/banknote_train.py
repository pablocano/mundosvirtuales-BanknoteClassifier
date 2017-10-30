# -*- coding: utf-8 -*-
"""
Created on Fri Sep 29 10:33:12 2017

@author: David
"""

# Standard imports
import cv2
import numpy as np
import time
import matplotlib.mlab as mlab
import matplotlib.pyplot as plt
import cPickle

#create CLAHE
clahe = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(4,4))

# create surf
descriptor = cv2.xfeatures2d.SURF_create(400, nOctaves =2, upright = False, extended = 1)
# create Orb
#descriptor = cv2.ORB_create(nfeatures = 500)

# create BFMatcher 
bf = cv2.BFMatcher(crossCheck=False)

# To compute keypoints and descriptors
def compute(descriptor_,img):
    kp,des = descriptor_.detectAndCompute(img,None)
    return kp,des

#To draw matches and text
def draw_matches(img_orig, img_query, kp_orig, kp_query, matches, mask):  
    output = cv2.drawMatches(img_orig,kp_orig, img_query,kp_query,matches, None, matchColor = (0,255,0), matchesMask = mask, singlePointColor = (255,0,0), flags =4)
    return output

# To draw countour of banknote
def draw_countour(img_in, M, img_out, text):
    
    h,w = img_in.shape
    pts = np.float32([ [0,0],[0,h-1],[w-1,h-1],[w-1,0] ]).reshape(-1,1,2)
    if M is not None:
        dst = cv2.perspectiveTransform(pts,M)
        text = text + ' ' + 'x= ' + str(dst[0][0][0]) + ' ' + 'y= ' + str(dst[0][0][1])
        cv2.putText(img_out,text,(30,30), cv2.FONT_HERSHEY_SIMPLEX, 1, (255,255,255), 1, cv2.LINE_AA)
        img_out = cv2.polylines(img_out,[np.int32(dst)],True,255,3, cv2.LINE_AA)
    return img_out, h, w

MIN_MATCHES = 20
def compare(kp_dict, kp_query, des_query):
    aux = 0
    flag = 0

    for name_ in kp_dict:
        #Matching between descriptors

        match_ = bf.knnMatch(kp_dict[name_][1], des_query, k = 2)
        
        # Apply ratio test
        good = []
        #for i in range(len(match_)):
        #    if len(match_[i])>1:
        #        if match_[i][0].distance < 0.75*match_[i][1].distance:
        #            good.append(match_[i][0])
        
        try:
            for m,n in match_:
                if m.distance < 0.75*n.distance:
                    good.append(m)
            #print("{0} : número de matches previos a ransac : {1}".format(name_, len(good)))
            #Get keypoints in both image that matches
            
            if len(good) > MIN_MATCHES:
                flag = 1
                src_pts = np.float32([ (kp_dict[name_][0])[m.queryIdx].pt for m in good ]).reshape(-1,1,2)
                dst_pts = np.float32([ kp_query[m.trainIdx].pt for m in good ]).reshape(-1,1,2)
                
                #RANSAC
                
                M_, mask_ = cv2.findHomography(src_pts, dst_pts, cv2.RANSAC, 4.0)
                
                #inliers
                match_mask_ = mask_.ravel().tolist()
                
                aux_count = (np.count_nonzero(match_mask_))/float(len(good))
                #aux_count = len(good)
                #print("{0} : matches {1} : filtrados {2}".format(name_, len(good), aux_count))
                if aux_count >= aux:
                    M = M_
                    match = good
                    match_mask = match_mask_
                    aux = aux_count
                    name = name_
        except:
            continue
    if flag == 1:
        return M, match, match_mask, name
    else:
        return [], [], [], 'none'


def predict(kp_des, des_query):
    aux = 0
    name = 'none'

    for name_ in kp_des:
        #Matching between descriptors

        match_ = bf.knnMatch(kp_des[name_][1], des_query, k = 2)
        
        # Apply ratio test
        good = []
        for m,n in match_:
            if m.distance < 0.75*n.distance:
                good.append(m)
        
        aux_count = len(good)
        print("{0} : matches {1} : filtrados {2}".format(name_, aux_count, aux_count/float(len(kp_des[name_][1]))))
        if aux_count >= aux:
            aux = aux_count
            name = name_
    return name



def read_image(img):
    img_ = cv2.resize((cv2.imread(img,1)),(0,0), fx = 0.12, fy = 0.12)
    img_gray = clahe.apply(cv2.cvtColor(img_, cv2.COLOR_BGR2GRAY))
    print(img_.shape)
    
    return img_, img_gray

def udpateHistogram(hist_array, matches, mask_matches):
    for i in range(len(matches)):
        if mask_matches[i] == 1:
            hist_array[matches[i].trainIdx] += 1
            
    return hist_array


#read image and resize
mil, mil_gray = read_image('../Data/img/mil_real.jpg')
milB, milB_gray = read_image('../Data/img/milB_real.jpg')
dosmil, dosmil_gray = read_image('../Data/img/dosmil_real.jpg')
dosmilB, dosmilB_gray = read_image('../Data/img/dosmilB_real.jpg')
veintemil, veintemil_gray = read_image('../Data/img/veintemil_real.jpg')
veintemilB, veintemilB_gray = read_image('../Data/img/veintemilB_real.jpg')
diezmil, diezmil_gray = read_image('../Data/img/diezmil_real.jpg')
diezmilB, diezmilB_gray = read_image('../Data/img/diezmilB_real.jpg')
cincomil, cincomil_gray = read_image('../Data/img/cincomil_real.jpg')
cincomilB, cincomilB_gray = read_image('../Data/img/cincomilB_real.jpg')


#Ground truth list
gt_list = [mil_gray, milB_gray, dosmil_gray, dosmilB_gray, veintemil_gray, veintemilB_gray, diezmil_gray, diezmilB_gray, cincomil_gray, cincomilB_gray]
gt_name = ['1000_Cara', '1000_Sello', '2000_Cara', '2000_Sello', '20.000_Cara', '20.000_Sello', '10.000_Cara', '10.000_Sello', '5000_Cara', '5000_Sello']

kp_hist = dict()

#Get descriptors
kp_des = dict()
for img, name in zip(gt_list, gt_name):
    kp_original, des_original = compute(descriptor, img)
    kp_des[name] = [kp_original, des_original]
    kp_hist[name] = np.zeros(len(kp_original))
    print len(kp_hist[name])
    
    
#init cam
video_capture = cv2.VideoCapture("../Data/vid/video1.mp4")
ret, frame = video_capture.read()
#print("frame : ", frame.shape)

while(ret):
    start = time.time()
    
    #to gray
    gray = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
    
    output = frame
    
    #Clahe
    gray = clahe.apply(gray)
    
    #compute keypoints and descriptors
    kp_query, des_query = compute(descriptor,gray)
       
    # do only if exists keypoints and descriptors. if not, show camera image
    if kp_query is not None and des_query is not None:
        #banknote = predict(kp_des, des_query)
        #cv2.putText(output,banknote,(30,30), cv2.FONT_HERSHEY_SIMPLEX, 1, (255,255,255), 1, cv2.LINE_AA)        
        
        #Get results of image analysis.
        M, total_match, mask_matches, name = compare(kp_des, kp_query, des_query)
        
        if name != 'none' and np.count_nonzero(mask_matches) >= 7 :
            
            kp_hist[name] = udpateHistogram(kp_hist[name], total_match, mask_matches)
            
            print(np.count_nonzero(mask_matches))
            if name == '1000_Cara':
                #draw countour
                gray, h, w = draw_countour(mil_gray, M, frame, name)
                output = draw_matches(mil,frame,kp_des[name][0],kp_query, total_match, mask_matches)
            elif name == '1000_Sello':
                #draw countour
                gray, h, w = draw_countour(milB_gray, M, frame, name)
                output = draw_matches(milB,frame,kp_des[name][0],kp_query, total_match, mask_matches)
            elif name == '2000_Cara':
                #draw countour
                gray, h, w = draw_countour(dosmil_gray, M, frame, name)
                output = draw_matches(dosmil,frame,kp_des[name][0],kp_query, total_match, mask_matches)
            elif name == '2000_Sello':
                #draw countour
                gray, h, w = draw_countour(dosmilB_gray, M, frame, name)
                output = draw_matches(dosmilB,frame,kp_des[name][0],kp_query, total_match, mask_matches)
            elif name == '20000_Cara':
                #draw countour
                gray, h, w = draw_countour(veintemil_gray, M, frame, name)
                output = draw_matches(veintemil,frame,kp_des[name][0],kp_query, total_match, mask_matches)
            elif name == '20000_Sello':
                #draw countour
                gray, h, w = draw_countour(veintemilB_gray, M, frame, name)
                output = draw_matches(veintemilB,frame,kp_des[name][0],kp_query, total_match, mask_matches)
            elif name == '10000_Cara':
                #draw countour
                gray, h, w = draw_countour(diezmil_gray, M, frame, name)
                output = draw_matches(diezmil,frame,kp_des[name][0],kp_query, total_match, mask_matches)
            elif name == '10000_Sello':
                #draw countour
                gray, h, w = draw_countour(diezmilB_gray, M, frame, name)
                output = draw_matches(diezmilB,frame,kp_des[name][0],kp_query, total_match, mask_matches)
            elif name == '5000_Cara':
                #draw countour
                gray, h, w = draw_countour(cincomil_gray, M, frame, name)
                output = draw_matches(cincomil,frame,kp_des[name][0],kp_query, total_match, mask_matches)
            elif name == '5000_Sello':
                #draw countour
                gray, h, w = draw_countour(cincomilB_gray, M, frame, name)
                output = draw_matches(cincomilB,frame,kp_des[name][0],kp_query, total_match, mask_matches)
        
    # Show current frame
    #cv2.imshow('Frame', output)
    
    #Get new frame
    ret, frame = video_capture.read()
    

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    print("takes ", time.time() - start)
    
for name in gt_name:
    
    plt.bar(range(len(kp_hist[name])),kp_hist[name])
    plt.savefig("../Data/img/hist_" + name + ".png")
    
    aux_list = []
    i = 0
    for point in kp_des[name][0]:
        if kp_hist[name][i] > 1:
            temp = [point.pt, point.size, point.angle, point.response, point.octave, point.class_id, kp_des[name][1][i]]
            aux_list.append(temp)
        i += 1
    f = open("../Data/keypoints" + name + ".txt", "wb")
    f.write(cPickle.dumps(aux_list))
    f.close()

video_capture.release()
cv2.destroyAllWindows()

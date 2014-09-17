'''
Created on Sep 12, 2014

@author: busta
'''

import numpy as np
from numpy.random import randn
import pandas as pd
from scipy import stats
import matplotlib as mpl
import matplotlib.pyplot as plt
import math

from collections import defaultdict

import cv2
import os

from plotDensity import init_plotting

if __name__ == '__main__':
    
    init_plotting()
    data_dir = '/tmp/minSet'
    resultsAll = np.genfromtxt('{0}/results.csv'.format(data_dir), delimiter=',', skip_header=0)
    
    font_faces = np.unique(resultsAll[:, 7]).astype(np.int)
    letters = np.unique(resultsAll[:, 6]).astype(np.int)
    detectors = np.unique(resultsAll[:, 0]).astype(np.int)
    
    if not os.path.exists('/tmp/report'):
        os.mkdir('/tmp/report')
    if not os.path.exists('/tmp/report/images'):
        os.mkdir('/tmp/report/images')
    f = open('/tmp/report/out.tex', 'w')
    f.write('\\documentclass{slides}\n')
    f.write('\\usepackage{multirow,tabularx}\n')
    f.write('\\usepackage{graphicx}\n')
    f.write('\\usepackage{changepage}\n')
    f.write('\\usepackage[paperheight=7in,paperwidth=8in]{geometry}\n')
    f.write('\\begin{document}\n\\small\n')
    f.write('\\begin{adjustwidth}{-4.0em}{-4.0em}\n')
    f.write('\\resizebox{7.8in}{!} {\n')
    #f.write('\\begin{table}\n')
    #print header
    delim = ''
    f.write('\\begin{tabular}{c|c|')
    for letter in letters:
        f.write('p{0.4cm}')
    f.write('|c}\n')
    f.write('\\hline\n')
    
    font_images = []
    font_images.append('/datagrid/personal/TextSpotter/fonts/genGoogleMin/NanumGothic-1.png')
    font_images.append('/datagrid/personal/TextSpotter/fonts/genGoogleMin/AlfaSlab-2.png')
    font_images.append('/datagrid/personal/TextSpotter/fonts/genGoogleMin/Amaranth-3.png')
    font_images.append('/datagrid/personal/TextSpotter/fonts/genGoogleMin/ABeeZee-4.png')
    font_images.append('/datagrid/personal/TextSpotter/fonts/genGoogleMin/Adamina-5.png')
    font_images.append('/datagrid/personal/TextSpotter/fonts/genGoogleMin/DroidSerif-6.png')
    font_images.append('/datagrid/personal/TextSpotter/fonts/genGoogleMin/Wellfleet-7.png')
    font_images.append('/datagrid/personal/TextSpotter/fonts/genGoogleMin/AbrilFatface-8.png')
    font_images.append('/datagrid/personal/TextSpotter/fonts/genGoogleMin/Ubuntu-9.png')
    font_images.append('/datagrid/personal/TextSpotter/fonts/genGoogleMin/StintUltraExpanded-10.png')
    
    font_names = []
    font_names.append('NanumGothicCoding-Bold')
    font_names.append('AlfaSlabOne-Regular')
    font_names.append('Amaranth-Bold')
    font_names.append('ABeeZee-Regular')
    font_names.append('Adamina-Regular')
    font_names.append('DroidSerif-Regular')
    font_names.append('Wellfleet-Regular')
    font_names.append('AbrilFatface-Regular')
    font_names.append('Ubuntu-Regular')
    font_names.append('StintUltraExpanded-Regular')
    
    
    counter = 0
    letters_counter = defaultdict(int)
    det_letter_couter = defaultdict(lambda : defaultdict(int))
    for face in font_faces:
        font_face_mask = resultsAll[:, 7] == face
        results_face = resultsAll[font_face_mask, :]
        
        det_names = {}
        det_names[0] = "VD"
        det_names[1] = "VC"
        det_names[2] = "LE"
        det_names[3] = "TP"
        det_names[4] = "SG"
        
        
        f.write('\\multirow{{{0}}}{{*}}{{ \\includegraphics[height=2cm]{{{1}}} }} '.format(detectors.shape[0] + 1, font_images[face] ) )
        if face == 0:
            f.write('Font & Est.')
        else:
            f.write(' & ')
        delim = '&'
        for letter in letters:
            f.write('{1} \\includegraphics[scale=0.25]{{/datagrid/personal/TextSpotter/fonts/genGoogleMin/Latin/{0}/{2}.png}}'.format( letter, delim, font_names[face] ) )
            delim = '&'
        f.write('{1} {0}'.format( 'Cor.\\% ', delim ) )
        f.write('\\\\\\hline\n')
        
        offset = 3
        for det_no in detectors:
            det_mask = results_face[:, 0] == det_no
            det_res = results_face[det_mask, :]
            delim = '&'
            f.write( '& {0}'.format(det_names[det_no]))
            falseCount = 0
            for letter in letters:
                lettr_mask = det_res[:, 6] == letter
                letter_res = det_res[lettr_mask, :]
                blank_image = np.zeros((25,60,3), np.uint8)  + 255
                color = (0, 0, 0)
                angleRad = letter_res[0, 8]
                angleRadGT = letter_res[0, 9]
                
                if np.abs(angleRad - angleRadGT) < (math.pi / 60):
                    color = (0, 0, 0)
                elif np.abs(angleRad - angleRadGT) < (2 * math.pi / 60):
                    color = (0, 128, 255)
                    falseCount += 1
                    letters_counter[letter] += 1
                    det_letter_couter[det_no][letter] += 1
                elif np.abs(angleRad - angleRadGT) > math.pi / 60:
                    color = (0, 0, 255)
                    falseCount += 1
                    letters_counter[letter] += 1
                    det_letter_couter[det_no][letter] += 1  
                                 
                    
                #cv2.rectangle(blank_image, (offset + 30, offset), (blank_image.shape[1] -offset, blank_image.shape[0] -offset), color, thickness=2)
                cv2.line(blank_image, (offset + 30, offset), (offset + 30, blank_image.shape[0] -offset), color, thickness=2)
                cv2.line(blank_image, (offset + 30, blank_image.shape[0] -offset), (blank_image.shape[1] -offset, blank_image.shape[0] -offset), color, thickness=2)
                M = np.zeros((2, 3), np.float)
                M[0, 0] = 1.0
                M[1, 1] = 1.0
                y= math.tan(angleRad - angleRadGT);
                M[0, 1] = -y
                dst = cv2.warpAffine(blank_image, M, (blank_image.shape[0]* 2, blank_image.shape[1] * 2), borderValue=(255, 255, 255))
                
                B = np.argwhere(np.invert(dst))
                (ystart, xstart, t), (ystop, xstop, t) = B.min(0), B.max(0) + 1
                xstop = min((xstop, blank_image.shape[0] + xstart + 5))
                
                Atrim = dst[ystart: blank_image.shape[0], xstart:xstop]
                
                imageName = '/tmp/report/images/{0}.png'.format(counter)
                cv2.imwrite(imageName, Atrim)
                counter += 1         
                f.write('{1} \\includegraphics[height=0.5cm]{2}{0}{3}'.format( imageName, delim, '{', '}' ) )
                delim = '&'
            f.write('& {0:0.1f}'.format((len(letters) - falseCount) / float(len(letters)) * 100))
            f.write('\\\\\n')
        f.write('\\hline\n')
    f.write(' & Correct \\%')
    for letter in letters:
        samples_per_letter = font_faces.shape[0] * detectors.shape[0]
        correct = samples_per_letter - letters_counter[letter]
        f.write(' & {0:0.0f}'.format( correct / float(samples_per_letter) * 100 ))
    f.write('\\\\\n\hline')
    
    for det_no in detectors:
        f.write( '& {0}'.format(det_names[det_no]))
        for letter in letters:
            samples_per_detecor = font_faces.shape[0]
            correct = samples_per_detecor - det_letter_couter[det_no][letter]
            if correct == samples_per_detecor:
                f.write(' & V')
            else:
                f.write(' & {0:0.0f}'.format( correct / float(samples_per_detecor) * 100 )) 
        f.write('\\\\\n')
            
    f.write('\\end{tabular}}\n')
    f.write('\\end{adjustwidth}\n\\vspace{\\fill}\n')
    #\\end{table}\n
    f.write('\\end{document}')
    
    
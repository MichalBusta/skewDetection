'''
Created on Sep 12, 2014

@author: busta
'''

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon
import math

from plotDensity import init_plotting

def arange_boxes(numBoxes, bp, color, ax1):
    
    for i in range(numBoxes):
        box = bp['boxes'][i]
        boxX = []
        boxY = []
        for j in range(5):
            boxX.append(box.get_xdata()[j])
            boxY.append(box.get_ydata()[j])
        boxCoords = zip(boxX,boxY)
        # Alternate between Dark Khaki and Royal Blue
        boxPolygon = Polygon(boxCoords, facecolor=color)
        ax1.add_patch(boxPolygon)
        # Now draw the median lines back over what we just filled in
        med = bp['medians'][i]
        medianX = []
        medianY = []
        for j in range(2):
            medianX.append(med.get_xdata()[j])
            medianY.append(med.get_ydata()[j])
        plt.plot(medianX, medianY, 'k')
        #medians[i] = medianY[0]
        # Finally, overplot the sample averages, with horizontal alignment
        # in the center of each box
        plt.plot([np.average(med.get_xdata())], [np.average(data[i])], color='w', marker='*', markeredgecolor='k')

if __name__ == '__main__':
    
    init_plotting()
    
    fig, ax = plt.subplots(figsize=(10,7))
    plt.rc('text', usetex=True)
    
    data_dir = '/tmp/fixAngleStep'
    resultsAll = np.genfromtxt('{0}/results.csv'.format(data_dir), delimiter=',', skip_header=0)
    detectors = np.unique(resultsAll[:, 0]).astype(np.int)
    detectors = detectors[0:-2]
    
    det_names = {}
    colors = []
    det_names[0] = "Vertical Dominant"
    colors.append("blue")
    det_names[1] = "Vert. Dom. CHull"
    colors.append("cyan")
    det_names[2] = "Longest Edge"
    colors.append("magenta")
    det_names[3] = "Thinnest Profile"
    colors.append("royalblue")
    det_names[4] = "Symmetric Glyph"
    colors.append("green")
    
    
    angles = np.unique(resultsAll[:, 9])
    
    labels = []
    count = 0
    for angle in angles:
        if count % 2 == 0:
            labels.append('{0:0.0f}'.format(angle * 180 / math.pi))
        else:
            labels.append('')
        count += 1
        
    labelsAll = []
    ticks = angles * 180 / math.pi
    ticksAll = []
    offset = 0.0
    for det_no in detectors:
        mask_vertical = resultsAll[:, 0] ==  det_no 
        vert = resultsAll[mask_vertical, :]
        data = []
        labels = []
        count = 0
        for angle in angles:
            angle_mask = vert[:, 9] == angle
            vert_angle = vert[angle_mask, :]
            data.append((vert_angle[:, 8] - vert_angle[:, 9]) * 180 / math.pi)
            ticksAll.append(angle * 180 / math.pi + offset)
            if count % 2 == 0:
                labelsAll.append('{0:0.0f}'.format(angle * 180 / math.pi))
            else:
                labelsAll.append('')
            count += 1
            
            
        bp = plt.boxplot(data, notch=0, sym='b+', whis=1.5, widths=5, positions=(ticks + offset))
        
        numBoxes = angles.shape[0]
        #arange_boxes(numBoxes, bp, colors[det_no], ax1) 
        
        offset += 100
        
    plt.ylim((-60, 60))
        
    plt.xlabel('Generated Skew Angle $\\alpha_{{gen}}$')
    plt.xticks(ticksAll, labelsAll)
    plt.ylabel("$\hat{{\\alpha}} - \\alpha_{{gen}}$")
    
    textOffset = 0
    for det_no in detectors:
        ax.text(0.18 + textOffset, 0.01, det_names[det_no], verticalalignment='bottom', horizontalalignment='right', transform=ax.transAxes, fontsize=10)
        textOffset += 0.2
    
    plt.savefig('/tmp/rotationInv.eps' , format='eps')
    
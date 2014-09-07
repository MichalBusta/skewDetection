'''
Created on Sep 4, 2014

@author: busta
'''

import numpy as np
from numpy.random import randn
import pandas as pd
from scipy import stats
import matplotlib as mpl
import matplotlib.pyplot as plt
import seaborn as sns
from distributions import jointplot
import math

import cv2

from matplotlib.colors import LogNorm

from numpy import genfromtxt

# set global settings
def init_plotting():
    plt.rcParams['figure.figsize'] = (8, 3)
    plt.rcParams['font.size'] = 10
    plt.rcParams['font.family'] = 'Times New Roman'
    plt.rcParams['axes.labelsize'] = plt.rcParams['font.size']
    plt.rcParams['axes.titlesize'] = 1.5*plt.rcParams['font.size']
    plt.rcParams['legend.fontsize'] = plt.rcParams['font.size']
    plt.rcParams['xtick.labelsize'] = plt.rcParams['font.size']
    plt.rcParams['ytick.labelsize'] = plt.rcParams['font.size']
    plt.rcParams['savefig.dpi'] = 2*plt.rcParams['savefig.dpi']
    plt.rcParams['xtick.major.size'] = 3
    plt.rcParams['xtick.minor.size'] = 3
    plt.rcParams['xtick.major.width'] = 1
    plt.rcParams['xtick.minor.width'] = 1
    plt.rcParams['ytick.major.size'] = 3
    plt.rcParams['ytick.minor.size'] = 3
    plt.rcParams['ytick.major.width'] = 1
    plt.rcParams['ytick.minor.width'] = 1
    plt.rcParams['legend.frameon'] = False
    plt.rcParams['legend.loc'] = 'upper right'
    plt.rcParams['axes.linewidth'] = 1
    plt.rcParams['axes.facecolor'] = "{1.0, 1.0, 1.0 }"
    plt.rcParams['axes.edgecolor'] = "black"
    
    plt.rcParams['grid.color'] = "{0.125, 0.125, 0.125 }"
    plt.rcParams['grid.linestyle'] = "dotted"
    plt.rcParams['grid.alpha'] = "0.5"
    plt.rcParams['grid.linewidth'] = "0.5"

    plt.gca().spines['right'].set_color('none')
    plt.gca().spines['top'].set_color('none')
    plt.gca().xaxis.set_ticks_position('bottom')
    plt.gca().yaxis.set_ticks_position('left')
    
def draw_evaluation(data_dir):
    
    detector_results = genfromtxt('{0}/{1}'.format(data_dir, 'results.csv'), delimiter=',', skip_header=0)
    mask_vertical = detector_results[:, 0] == 3 
    vert = detector_results[mask_vertical, :]
    
    #with sns.axes_style("white"):
    #    grid = sns.jointplot(vert[:, 1], vert[:, 4], kind="hex", stat_func = None);
    #plt.show()
    correct = np.abs(vert[:, 1]) <= (3 * math.pi / 180)
    to_hist_corr = vert[correct, 4]
    hist_corr, bin_edges = np.histogram(to_hist_corr, bins=10)
    hist_all,  be = np.histogram(vert[:, 4], bins=bin_edges)
    his = hist_corr / hist_all.astype(np.float)
    
    plt.plot(be[0:10], his)
    plt.rc('text', usetex=True)
    plt.xlabel("P($\delta$)")
    plt.ylabel("Correct Classifications in $\%$")
    #plt.plot(vert[:, 1], vert[:, 4], 'o')
    plt.grid(True)
    plt.subplots_adjust(bottom=0.25)
    plt.savefig('/tmp/VerticalDomPDF.eps')
    plt.close()
    

    fig, ax = plt.subplots()
    fig.set_size_inches(8, 4)
    
    #plot weak detectors
    overview = genfromtxt('{0}/{1}'.format(data_dir, 'overview.csv'), delimiter=',', names=True, dtype=np.float)
    overview2 = genfromtxt('{0}/{1}'.format(data_dir, 'overview.csv'), delimiter=',', skip_header=1, dtype=np.float)
    
    ind = np.arange(len(overview[0]))
    width = 0.25 
    rects1 = plt.bar(ind, overview[0], width, color='#4572A7')
    rects2 = plt.bar(ind + width + 0.1, overview[1], width, color='#42ca2f')
    
    plt.legend( (rects1[0], rects2[0]), ('Correct Classifications', 'Avg. Correct Letters') )
    det_names = []
    for i in range(len(overview.dtype.names)):
        det_names.append(overview.dtype.names[i].replace('_', '\n'))
    plt.xticks(ind, det_names, rotation=17) 
    plt.ylabel("Correct Classification \%")
    
    ax2 = ax.twinx()
    ax2.plot(ind, overview2[3, :], '#89A54E')
    ax2.set_ylabel('Standard Deviation', color='#89A54E')
    ax2.grid(b=False)
    
    plt.subplots_adjust(bottom=0.3)
    plt.savefig('/tmp/weakDetectors.eps')


def draw_detectors_dependence(data_dir):
    
    entries = []
    entries.append(('correlationTable_LongestEdge_VerticalDom.csv', 'Longest Edge', 'Vertical Dominant'))
    entries.append(('correlationTable_ThinProfile_LongestEdge.csv', 'Thinnest Profile', 'Longest Edge'))
    entries.append(('correlationTable_ThinProfile_TopBottomCenter.csv', 'Thinnest Profile', 'Symmetric Glyph'))
    entries.append(('correlationTable_TopBottomCenter_LongestEdge.csv', 'Symmetric Glyph', 'Longest Edge'))
    entries.append(('correlationTable_TopBottomCenter_VerticalDom.csv', 'Symmetric Glyph', 'Vertical Dominant'))
    entries.append(('correlationTable_VerticalDom_VertDomCH.csv', 'Vertical Dominant', 'Vertical Dominant on Convex Hull'))
    
    max_val = 40
    
    for pair_def in entries: 
    
        my_data = genfromtxt('{0}/{1}'.format(data_dir, pair_def[0]), delimiter=',', skip_header=1)
        
        x = my_data[:, 0]
        x = np.abs(x)
        x = x * 180 / math.pi
        x = x.astype(np.int)
        x[x > max_val] = max_val
        y = my_data[:, 1]
        y = np.abs(y)
        y = y * 180 / math.pi
        y = y.astype(np.int)
        y[y > max_val] = max_val
        fig = plt.figure()
        fig.set_size_inches(6, 4)
        ax = plt.gca()
        ax.set_axis_bgcolor('#fffdc6')
        plt.hist2d(x, y, bins=max_val, norm=LogNorm(), cmap="YlOrRd")
        plt.colorbar()
        plt.xlabel(pair_def[1])
        plt.ylabel(pair_def[2])
        plt.subplots_adjust(bottom=0.15)
        #with sns.axes_style("white"):
        #    grid = jointplot(x, y, kind="hex", stat_func = None, xlim=(-0.1, 20), ylim=(-0.1, 20));
        #    grid.set_axis_labels(pair_def[1], pair_def[2])
        
        plt.savefig('/tmp/{0}.eps'.format(pair_def[0]), format='eps')
        #plt.show()

if __name__ == '__main__':
    
    init_plotting()
    
    data_dir = '/datagrid/personal/TextSpotter/SkewDetection/11Run'
    data_dir = '/tmp/11Run'
    draw_evaluation(data_dir)
    draw_detectors_dependence(data_dir)
    
    
        
    
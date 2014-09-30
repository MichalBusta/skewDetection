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
    

def plotPDF(det_data, det_data2, output_file):
    
    fig, ax = plt.subplots(figsize=(6,6))
    correct = np.abs(det_data[:, 1]) <= (3 * math.pi / 180)
    to_hist_corr = det_data[correct, 4]
    bins  = 10
    hist_corr, bin_edges = np.histogram(to_hist_corr, bins=bins)
    hist_all,  be = np.histogram(det_data[:, 4], bins=bin_edges)
    his = hist_corr / hist_all.astype(np.float) * 100
    
    rects1 = plt.plot(be[0:bins], his)
    
    
    correct2 = np.abs(det_data2[:, 1]) <= (3 * math.pi / 180)
    to_hist_corr2 = det_data2[correct2, 4]
    hist_corr2, bin_edges2 = np.histogram(to_hist_corr2, bins=bins)
    hist_all2,  be2 = np.histogram(det_data2[:, 4], bins=bin_edges2)
    his2 = hist_corr2 / hist_all2.astype(np.float) * 100
    rects2 = plt.plot(be2[0:bins], his2, 'g')
    
    plt.rc('text', usetex=True)
    plt.xlabel(r'P($\hat{ \alpha }$)')
    plt.ylabel("Correct Classifications in $\%$")
    
    plt.legend( (rects1[0], rects2[0]), ('Vertical Dominant', 'Vert. Dom. CHull'), bbox_to_anchor=(1.0, 0.2) )
    
    #plt.plot(vert[:, 1], vert[:, 4], 'o')
    plt.grid(False)
    plt.subplots_adjust(bottom=0.25)
    plt.savefig(output_file)
    plt.close()  
    
def print_summary(data_dir):  
    '''
    '''
    detector_results = genfromtxt('{0}/{1}'.format(data_dir, 'results.csv'), delimiter=',', skip_header=0)
    detectors = np.unique(detector_results[:, 0]).astype(np.int)
    scripts = np.unique(detector_results[:, 10]).astype(np.int)
    
    det_names = {}
    detc = 0
    det_names[detc] = "VD"
    detc += 1
    det_names[detc] = "VC"
    detc += 1
    det_names[detc] = "LE"
    detc += 1
    det_names[detc] = "TP"
    detc += 1
    det_names[detc] = "SG"
    detc += 1
    #det_names[detc] = "LB"
    #detc += 1
    det_names[detc] = "NMS"
    detc += 1
    det_names[detc] = "BP"
    
    script_names = {}
    script_names[0] = 'Cyrillic'
    script_names[1] = 'Georgian'
    script_names[2] = 'Greek'
    script_names[3] = 'Latin'
    script_names[4] = 'Runnic'
    
    
    f = open('/tmp/scripts.tex', 'w')
    f.write('\\begin{tabular}')
    f.write('{l|')
    for script in scripts:
        f.write('rr|')
    f.write('rr}\n\\hline\n')
    
    f.write('Script:')
    for script in scripts:
        f.write('& \\multicolumn{{2}}{{l|}}{{{0}}}'.format(script_names[script]) )
    f.write('& \\multicolumn{{2}}{{l}}{{{0}}}'.format('All') )
    f.write('\\\\\n')
    
    f.write('Estim. ')
    for script in scripts:
        f.write('& $<3^{\circ}$[\\%] & $\\sigma [^{\circ}]$')
    f.write('& $<3^{\circ}$[\\%] & $\\sigma [^{\circ}]$')
    f.write('\\\\\\hline\n')    
    
    for det in detectors:
        det_mask = detector_results[:, 0] == det
        det_results_all = detector_results[det_mask, :]
        f.write('{0}'.format(det_names[det]))
        
        for script in scripts:
             
            script_mask = det_results_all[:, 10] == script
            results_sc = det_results_all[script_mask, :]
            
            angles_diff = results_sc[:, 8] - results_sc[:, 9];
            angles_diff_deg = angles_diff * 180 / math.pi
            std_dev = np.std(angles_diff_deg)
            correct_mask = np.abs(angles_diff) < (math.pi / 60)
            correct = np.count_nonzero(correct_mask)
            
            f.write('& {0:0.1f} & {1:0.1f} '.format( correct / float(angles_diff.shape[0]) * 100, std_dev ) )
            
        angles_diff_det = det_results_all[:, 8] - det_results_all[:, 9];
        angles_diff_det_deg = angles_diff_det * 180 / math.pi   
        std_det_dev = np.std(angles_diff_det_deg)
        correct_mask = np.abs(angles_diff_det) < (math.pi / 60)
        correct = np.count_nonzero(correct_mask)
        
        f.write('& {0:0.1f} & {1:0.1f} '.format( correct / float(angles_diff_det.shape[0]) * 100, std_det_dev ) )
            
        f.write('\\\\\n')
        if det == 4:
            f.write('\\hline\n')
            
    f.write('\\end{tabular}')
    
    
    
def draw_evaluation(data_dir):
    
    detector_results = genfromtxt('{0}/{1}'.format(data_dir, 'results.csv'), delimiter=',', skip_header=0)
    mask_vertical = detector_results[:, 0] == 0 
    vert = detector_results[mask_vertical, :]
    
    mask_verticalch = detector_results[:, 0] == 1 
    vertch = detector_results[mask_verticalch, :]
    
    #with sns.axes_style("white"):
    #    grid = sns.jointplot(vert[:, 1], vert[:, 4], kind="hex", stat_func = None);
    #plt.show()
    
    plotPDF(vert, vertch,  '/tmp/VerticalDomPDF.eps')

    fig, ax = plt.subplots()
    fig.set_size_inches(8, 4)
    
    #plot weak detectors
    overview = genfromtxt('{0}/{1}'.format(data_dir, 'overview.csv'), delimiter=',', names=True, dtype=np.float)
    overview2 = genfromtxt('{0}/{1}'.format(data_dir, 'overview.csv'), delimiter=',', skip_header=1, dtype=np.float)
    
    ind = np.arange(len(overview[0]))
    ind[ind.shape[0] - 2] += 0.5 
    ind[ind.shape[0] - 1] += 0.5
    width = 0.25 
    overview0 = overview2[0][::-1]
    overview1 = overview2[1][::-1]
    rects1 = plt.bar(ind, overview0, width, color='#4572A7')
    rects2 = plt.bar(ind + width + 0.1, overview1, width, color='#42ca2f')
    
    plt.legend( (rects1[0], rects2[0]), ('Correct Classifications', 'Avg. Correct Letters'), bbox_to_anchor=(0, 1.14) )
    det_names = []
    for i in range(len(overview.dtype.names)):
        det_names.append(overview.dtype.names[i].replace('_', '\n'))
    det_names = det_names[::-1]
    plt.xticks(ind, det_names, rotation=17) 
    plt.ylabel("Correct Classification \%")
    
    ax2 = ax.twinx()
    overview3 = overview2[3, :][::-1]
    ax2.plot(ind, overview3, '#89A54E')
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
        fig.set_size_inches(5, 4)
        ax = plt.gca()
        #ax.set_axis_bgcolor('#fffdc6')
        plt.hist2d(x, y, bins=35, norm=LogNorm(), cmap="YlOrRd")
        plt.colorbar()
        plt.xlabel( '$|\\hat{{\\alpha}} - \\alpha_{{gen}}| $ {0}'.format(pair_def[1]))
        plt.ylabel('$|\\hat{{\\alpha}} - \\alpha_{{gen}}| $ {0}'.format(pair_def[2]))
        plt.subplots_adjust(bottom=0.15)
        plt.savefig('/tmp/{0}.eps'.format(pair_def[0][:-4]), format='eps')
        
        #with sns.axes_style("white"):
        #    grid = sns.jointplot(x, y, kind="hex", stat_func = None, xlim=(-0.1, 20), ylim=(-0.1, 20));
        #    grid.set_axis_labels(pair_def[1], pair_def[2])
        
        
        #plt.show()

def plotWordsLength():
    
    fig, ax = plt.subplots()
    plt.rc('text', usetex=True)
    plt.rcParams['legend.loc'] = 'upper left'
    
    wordsAll = genfromtxt('/datagrid/personal/TextSpotter/SkewDetection/wordsAll/results.csv', delimiter=',', skip_header=1)
    words2 = genfromtxt('/datagrid/personal/TextSpotter/SkewDetection/words2/results.csv', delimiter=',', skip_header=1)
    words3 = genfromtxt('/datagrid/personal/TextSpotter/SkewDetection/words3/results.csv', delimiter=',', skip_header=1)
    words4 = genfromtxt('/datagrid/personal/TextSpotter/SkewDetection/words4/results.csv', delimiter=',', skip_header=1)
    
    
    f = open('/tmp/words.tex', 'w')
    f.write('\\begin{tabular}')
    f.write('{l|')
    for i in range(4):
        f.write('rr|')
    f.write('rr}\n\\hline\n')
    
    
    for i in range(4):
        f.write('& $<3^{\circ}$[\\%] & $\\sigma [^{\circ}]$')
    f.write('\\\\\\hline\n')
    
    x1 = []
    x2 = []
    x23 = []
    stdDev = []
    labelsX = []
    
    correct2 = np.abs(words2[:, 1]) <= (3 * math.pi / 180)
    x1.append(np.sum(correct2) / float(words2.shape[0]) * 100)
    
    f.write('{0:0.1f} & {1:0.1f}'.format(np.sum(correct2) / float(words2.shape[0]) * 100, np.std(words2[:, 1]) * 180 / math.pi ))
    
    x2.append( (words2.shape[0] - np.sum(words2[:, 5] )) / float(words2.shape[0]) * 100)
    x23.append( np.sum(words2[:, 5]) )
    labelsX.append("2")
    stdDev.append(np.std(words2[:, 1]))
    correct3 = np.abs(words3[:, 1]) <= (3 * math.pi / 180)
    f.write('& {0:0.1f} & {1:0.1f}'.format(np.sum(correct3) / float(words3.shape[0]) * 100, np.std(words3[:, 1] * 180/math.pi)))
    x1.append(np.sum(correct3) / float(words3.shape[0]) * 100)
    x2.append( (words3.shape[0] - np.sum(words3[:, 5] )) / float(words3.shape[0]) * 100)
    x23.append( np.sum(words3[:, 5]) )
    labelsX.append("3")
    stdDev.append(np.std(words3[:, 1]))
    correct4 = np.abs(words4[:, 1]) <= (3 * math.pi / 180)
    x2.append( (words4.shape[0] - np.sum(words4[:, 5] )) / float( words4.shape[0] ) * 100)
    x1.append(np.sum(correct4) / float( words4.shape[0] ) * 100 )
    x23.append( np.sum(words4[:, 5]) )
    f.write('& {0:0.1f} & {1:0.1f}'.format(np.sum(correct4) / float(words4.shape[0]) * 100, np.std(words4[:, 1] * 180/math.pi)))
    labelsX.append("4")
    stdDev.append(np.std(words4[:, 1]))
    correctAll = np.abs(wordsAll[:, 1]) <= (3 * math.pi / 180)
    x1.append(np.sum(correctAll) / float(wordsAll.shape[0] ) * 100 )
    x2.append( ( wordsAll.shape[0] - np.sum(wordsAll[:, 5])) / float(wordsAll.shape[0] ) * 100 )
    x23.append( np.sum(wordsAll[:, 5]) )
    labelsX.append("All")
    f.write('& {0:0.1f} & {1:0.1f}'.format(np.sum(correctAll) / float(wordsAll.shape[0]) * 100, np.std(wordsAll[:, 1] * 180/math.pi)))
    stdDev.append(np.std(wordsAll[:, 1]))
    f.write('\\\\\\hline')
    ind = np.arange(len(x1))
    width = 0.25 
    rects1 = plt.bar(ind, x1, width, color='#4572A7')
    rects2 = plt.bar(ind + width + 0.1, x2, width, color='red')
    
    plt.legend( (rects1[0], rects2[0]), ('Correct Classifications', 'Improved'), bbox_to_anchor=(0, 1.14) )
    plt.ylabel("Correct Classification \%")
    plt.xticks(ind, labelsX) 
    plt.xlabel("Word Length")
    
    ax2 = ax.twinx()
    ax2.plot(np.asarray(ind) + 0.25, stdDev, '#89A54E')
    ax2.set_ylabel('Standard Deviation', color='#89A54E')
    ax2.grid(b=False)
    plt.subplots_adjust(bottom=0.2)
    
    plt.savefig('/tmp/wordClasss.eps' , format='eps')
    print( x23 )
    f.write('\\end{tabular}')
    
    

if __name__ == '__main__':
    
    init_plotting()
    
    data_dir = '/datagrid/personal/TextSpotter/SkewDetection/WinFonts4'
    #data_dir = '/datagrid/personal/TextSpotter/SkewDetection/14Run'
    #data_dir = '/tmp/11Run'
    plotWordsLength()
    #draw_evaluation(data_dir)
    #data_dir = '/tmp/testRes'
    #print_summary(data_dir)
    #draw_detectors_dependence(data_dir)
    
    
        
    
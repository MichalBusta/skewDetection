'''
Created on Nov 14, 2014

@author: busta
'''

import numpy as np
import matplotlib.pyplot as plt

import mpl_toolkits.axisartist.floating_axes as floating_axes

from matplotlib.projections import PolarAxes
from matplotlib.transforms import Affine2D
import  mpl_toolkits.axisartist.angle_helper as angle_helper

from mpl_toolkits.axisartist.grid_finder import FixedLocator, MaxNLocator, DictFormatter


def setup_axes3(fig, rect):
    """
    Sometimes, things like axis_direction need to be adjusted.
    """
    
    # scale degree to radians
    tr_scale = Affine2D().scale(np.pi/180., 1.)

    tr = tr_scale + PolarAxes.PolarTransform()

    grid_locator1 = angle_helper.LocatorHMS(4)
    tick_formatter1 = angle_helper.FormatterHMS()

    grid_locator2 = MaxNLocator(3)

    ra0, ra1 = 0, 180
    cz0, cz1 = 0, 10
    grid_helper = floating_axes.GridHelperCurveLinear(tr,
                                        extremes=(ra0, ra1, cz0, cz1),
                                        #grid_locator1=grid_locator1,
                                        grid_locator2=grid_locator2,
                                        #tick_formatter1=tick_formatter1,
                                        tick_formatter2=None,
                                        )

    ax1 = floating_axes.FloatingSubplot(fig, rect, grid_helper=grid_helper)
    fig.add_subplot(ax1)

    # adjust axis
    ax1.axis["left"].set_axis_direction("bottom")
    ax1.axis["right"].set_axis_direction("top")

    ax1.axis["bottom"].set_visible(False)
    ax1.axis["top"].set_axis_direction("bottom")
    ax1.axis["top"].toggle(ticklabels=True, label=True)
    ax1.axis["top"].major_ticklabels.set_axis_direction("top")
    ax1.axis["top"].label.set_axis_direction("top")

    ax1.axis["left"].label.set_text(r"$P(\alpha)$")
    ax1.axis["top"].label.set_text(r"$\alpha$")


    # create a parasite axes whose transData in RA, cz
    aux_ax = ax1.get_aux_axes(tr)

    aux_ax.patch = ax1.patch # for aux_ax to have a clip path as in ax
    ax1.patch.zorder=0.9 # but this has a side effect that the patch is
                        # drawn twice, and possibly over some other
                        # artists. So, we decrease the zorder a bit to
                        # prevent this.

    return ax1, aux_ax


if __name__ == '__main__':
    N = 20
    theta = np.linspace(0.0, 180, N, endpoint=False)
    radii = 10 * np.random.rand(N)
    width = 180 / 4 * np.random.rand(N)
    
    fig = plt.figure(1, figsize=(8, 4))
    ax, aux_ax3 = setup_axes3(fig, 111)
    #ax = plt.subplot(111, polar=True)
    #ax.set_xbound(0, np.pi)
    bars = aux_ax3.bar(theta, radii, width=width, bottom=0.0)

    # Use custom colors and opacity
    for r, bar in zip(radii, bars):
        bar.set_facecolor(plt.cm.jet(r / 10.))
        bar.set_alpha(0.5)

    plt.show()
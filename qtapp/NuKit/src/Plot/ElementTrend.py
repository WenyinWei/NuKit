#!/usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt
import os
import sys
# from scipy.constants import e, k as kB


# Reactant masses in atomic mass units (u).
# header = {'D': 2.014, 'T': 3.016, '3He': 3.016}

# Energy grid, 1 – 1000 keV, evenly spaced in log-space.
# Egrid = np.logspace(0, 3, 100)


odeRun = np.genfromtxt(os.path.dirname(os.path.dirname( os.path.dirname(os.path.realpath(__file__))))+'/data/oderun.csv', delimiter=',', dtype=str)
neutron = np.genfromtxt(os.path.dirname(os.path.dirname( os.path.dirname(os.path.realpath(__file__))))+'/data/t_neutron.csv', delimiter=',', dtype=str)
# sys.argv[1]
fig, ax = plt.subplots()
lns = []
for i in range(2, np.size(odeRun, 1)):
    if odeRun[0, i] in ['Xe_135','I_135']:
        lns = lns + ax.plot(np.asarray(odeRun[1:, 1], dtype=np.float64), np.asarray(
            odeRun[1:, i], dtype=np.float64), lw=2, label=odeRun[0, i])
    # if odeRun[0, i] in ['U_235']:
    #     lns = lns + ax.plot(np.asarray(odeRun[1:, 1], dtype=np.float64), np.asarray(
    #         odeRun[1:, i], dtype=np.float64)/100000, lw=2, label='U_235*0.1')
# ax.set_xlim(0, 10)
# xticks= np.array([1, 10, 100, 1000])
# ax.set_xticks(xticks)
# ax.set_xticklabels([str(x) for x in xticks])
ax.set_xlabel('t /h')

# label of y axis
ax.set_ylabel('Element Density $mol/m^3$')
ax.set_ylim(bottom=0)
ax.grid(True, which='both', ls='-')
if True:
    ax2 = ax.twinx()
    lns = lns + ax2.plot(np.asarray(neutron[1:, 0], dtype=np.float64), np.asarray(
        neutron[1:, 1], dtype=np.float64), '-r', label='neutron')
    ax2.set_ylabel('Neutron Flux $10^{17}/m^2\cdot s$')
    ax2.set_ylim(bottom=0)


# add legend
labs = [l.get_label() for l in lns]
ax.legend(lns, labs)


plt.savefig(os.path.dirname(os.path.dirname( os.path.dirname(os.path.realpath(
    __file__))))+'/data/specified_species.png')
plt.show()

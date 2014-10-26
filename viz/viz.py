#!/usr/bin/env python
from pylab import *
from matplotlib.ticker import FuncFormatter

snapshots = []
execfile("snapshots.dat")
mn = min(map(min, snapshots))
mx = max(map(max, snapshots))

mx_len = max(map(len, snapshots))
count = 0

rc('font', family='monospace')

def pointer(x, pos):
  return str(hex(x))[:-2]

formatter = FuncFormatter(pointer)

for s in snapshots:
  figure(figsize=(12,8))
  gca().yaxis.set_major_formatter(formatter)
  scatter(range(len(s)), s)
  ylim(mn + (mn-mx)*0.05, mx - (mn-mx)*0.5)
  xlim(0 - mx_len*0.05, mx_len + mx_len*0.05)
  title("nth call to mergesort (n=%d)"%(count+1,))
  xlabel("grouping (by locality)")
  ylabel("memory address")
  savefig("%02d.png"%(count,))
  count += 1


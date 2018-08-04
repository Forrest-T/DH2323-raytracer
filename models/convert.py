#!/usr/bin/python

import sys
import numpy as np
from math import sqrt

def create():
    vertices = open('vertices','r')
    triangles = open('triangles','r')
    vertices = [i.split(' ')[0:3] for i in vertices]
    triangles = [i.split(' ')[0:3] for i in triangles]
    vertices = [[float(a) for a in b] for b in vertices]
    triangles = [[int(a) for a in b] for b in triangles]
    mesh = [vertices[t[0]]+vertices[t[1]]+vertices[t[2]] for t in triangles]
    for i in mesh:
        print(i)

def normals():
    file = sys.argv[1]
    file = open(file,'r')
    lines = [line.split(' ') for line in file]
    lines = [[float(f) for f in s] for s in lines]
    for i in range(len(lines)):
        lines[i] = lines[i] + ([float(i) for i in normal(lines[i])])
        print(lines[i])

def normal(t):
    v0 = np.array(t[0:3])
    v1 = np.array(t[3:6])
    v2 = np.array(t[6:9])
    norm = np.cross(v1-v0,v2-v0)
    length = sqrt(sum([pow(i,2) for i in norm]))
    return norm/length

def flip():
    file = sys.argv[1]
    file = open(file,'r')
    lines = [line.split(' ') for line in file]
    lines = [[float(f) for f in s] for s in lines]
    for i in lines:
        i[1] *= -1
        i[4] *= -1
        i[7] *= -1
        print(*i, sep=' ')

def color(): 
    file = sys.argv[1]
    file = open(file,'r')
    lines = [line.split(' ') for line in file]
    lines = [[float(f) for f in s] for s in lines]
    for i in lines:
        i[9] = 0.0
        i[10] = 0.6
        i[11] = 0.2
        print(*i, sep=' ')

def flipAndColor():
    file = sys.argv[1]
    file = open(file,'r')
    lines = [line.split(' ') for line in file]
    lines = [[float(f) for f in s] for s in lines]
    for i in lines:
        i[1] *= -1
        i[4] *= -1
        i[7] *= -1
        i[9] = 0.0
        i[10] = 0.6
        i[11] = 0.2
        print(*i, sep=' ')

def scale():
    file = sys.argv[1]
    file = open(file,'r')
    lines = [line.split(' ') for line in file]
    lines = [[float(f) for f in s] for s in lines]
    x = [i[0] for i in lines] + [i[3] for i in lines] + [i[6] for i in lines]
    y = [i[1] for i in lines] + [i[4] for i in lines] + [i[7] for i in lines]
    z = [i[2] for i in lines] + [i[5] for i in lines] + [i[8] for i in lines]
    minx = min(x)
    miny = min(y)
    minz = min(z)
    maxx = max(x)
    maxy = max(y)
    maxz = max(z)
    lenx = maxx - minx
    leny = maxy - miny
    lenz = maxz - minz
    offset_x = -.5*(minx + maxx)
    offset_y = -.5*(miny + maxy)
    offset_z = -.5*(minz + maxz)
    scale_factor = 1.8/max([lenx, leny, lenz])
    for i in lines:
        i[0] = (i[0] + offset_x) * scale_factor
        i[3] = (i[3] + offset_x) * scale_factor
        i[6] = (i[6] + offset_x) * scale_factor
        i[1] = (i[1] + offset_y) * scale_factor
        i[4] = (i[4] + offset_y) * scale_factor
        i[7] = (i[7] + offset_y) * scale_factor
        i[2] = (i[2] + offset_z) * scale_factor
        i[5] = (i[5] + offset_z) * scale_factor
        i[8] = (i[8] + offset_z) * scale_factor
        print(*i, sep=' ')

scale()

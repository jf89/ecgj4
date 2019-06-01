#!/usr/bin/python

import struct
from PIL import Image

# texture_file = 'tileset.png'
texture_file = 'codepage437.png'

input = Image.open(texture_file)

width, height = input.size
result = struct.pack('HH', width, height)

for y in range(height, 0, -1):
	row = []
	for x in range(width):
		r, g, b, a = input.getpixel((x, y-1))
		row.extend((r, g, b, a))
	result += bytes(row)

# with open('tileset.bin', 'wb') as f:
with open('codepage437.bin', 'wb') as f:
	f.write(result)

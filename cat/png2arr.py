import png 
import array
import os
names = []

for filename in os.listdir("."):
	if filename.endswith(".png"): 
		name = filename.replace(".png",'')
		names.append(name)
		reader = png.Reader(filename=filename)
		w,h,pixels,metadata = reader.read_flat()
		pixel_byte_width = 4 if metadata['alpha'] else 3
		arr8x8 = ["byte "+name+"[8][8][3] = {"]
		if w == 8 and h == 8:
			# we have it easy, just convert and pass the pixels as [8][8][3]
			for row in range(0, h):
				line = ' {'
				for column in range(0, w):
					pxl_pos = column+row*w
					line = line + '{'
					counter = 0
					for color in pixels[pxl_pos*pixel_byte_width : (pxl_pos+1)*pixel_byte_width]:
						counter = counter+1
						line = line + str(color)
						if counter < pixel_byte_width:
							line = line + ', '
						elif counter == pixel_byte_width and column == w-1:
							line = line + '}'
						else:
							line = line + '}, '
				if row < h-1:
					line = line + '},'
				else:
					line = line + '}'
				arr8x8.append(line)
			arr8x8.append('};')
		#elif w < 8 or h < 8:
			# we have to stretch pixels or let the image take a smaller space
		#else:
			# larger image, average pixels or crop
		for line in arr8x8:
			print line


for name in names:
	print "drawFast("+name+");\ndelay(1000);"
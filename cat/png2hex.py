import png
import os

for filename in os.listdir("."):
    if filename.endswith(".png"):
        reader = png.Reader(filename=filename)
        w, h, pxs, metadata = reader.read_flat()
        px_byte_width = 4 if metadata['alpha'] else 3
        hexline = ''
        if w == 8 and h == 8:
            # we have it easy, just convert and pass the pixels as [8][8]
            for row in range(0, h):
                for column in range(0, w):
                    px = column+row*w
                    clr = pxs[px*px_byte_width: (px+1)*px_byte_width]
                    # print hexes instead of rgb tuplets
                    hexline = hexline + hex(
                        (clr[0] << 16) +    # red
                        (clr[1] << 8) +     # green
                        clr[2])             # blue
                if row < h-1:
                    hexline = hexline + ' '
                elif row == 8 and column == 8:
                    hexline = hexline + '\n'
        # elif w < 8 or h < 8:
            # we have to stretch pixels or let the image take a smaller space
        # else:
            # larger image, average pixels or crop
        print hexline

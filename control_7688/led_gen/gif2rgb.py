import numpy as np
from PIL import Image, ImageSequence
import json
import sys
import os

if len(sys.argv) < 3:
    sys.exit("File name missing!")
else:
    file_path = sys.argv[1]
    output_path = sys.argv[2]
img = Image.open(file_path)
frames = np.array([np.array(frame.copy().convert('RGB').getdata(),dtype=np.uint8).reshape(frame.size[1],frame.size[0],3) for frame in ImageSequence.Iterator(img)])
# frames.shape = (frame_num * frame_height * frame_width * 3(RGB) )

for i in range(frames.shape[0]):
    for j in range(1,frames.shape[1],2):
        frames[i][j] = np.flip(frames[i][j], 0)

frames_list = np.right_shift(frames,2).reshape(frames.shape[0], -1, 3)

for i in range(frames_list.shape[0]):
    for j in range(frames_list.shape[1]):
        for k in range(frames_list.shape[2]):
            if frames_list[i][j][k] > 0:
                frames_list[i][j][k] -= 1

np.save(output_path, frames_list)



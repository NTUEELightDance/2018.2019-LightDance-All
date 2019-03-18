import numpy as np
from PIL import Image, ImageSequence
import json
import sys
import os

N_DANCER = 8
N_WS = 5
MAX_LEN = 2050
WS_LEN = [88, 96, 60, 36, 36]

if os.path.isdir("./gifs"):
	for i in range(N_DANCER):
		gif_list = []
		for j in range(N_WS):
			filename = str(i) + "_" + str(j) + ".gif"
			path = os.path.join("gifs",filename)
			if os.path.isfile(path):
				img = Image.open(path)
				print ("Find " + filename)
				frames = np.array([np.array(frame.copy().convert('RGB').getdata(),dtype=np.uint8).reshape(frame.size[1],frame.size[0],3) for frame in ImageSequence.Iterator(img)])
				if j == 1: # shoulder is vertical
					frames = frames.transpose(0,2,1,3)
				for a in range(frames.shape[0]):
					for b in range(1,frames.shape[1],2):
						frames[a][b] = np.flip(frames[a][b], 0)
				frames_list = np.right_shift(frames,2).reshape(frames.shape[0], -1, 3)
				if i >= 8 and j == 0:
					frames_list = np.concatenate([frames_list,np.zeros([frames.shape[0],88-frames.shape[1],3],dtype=np.int)],axis=1)
				q = np.where(frames_list <= 2)
				w = np.where(frames_list > 2)
				frames_list[p] = 0
				frames_list[w] -= 2
				gif_list.append(frames_list.tolist())
			else:
				print (filename + " not found. Generating...")
				gif_list.append(np.zeros((MAX_LEN,WS_LEN[j],3), dtype=np.int).tolist())
		with open('data_ws'+str(i)+'.json', 'w') as outfile:
			json.dump(gif_list, outfile)
		print("File \"" + "data_ws" + str(i) + ".json" + "\" generated!")

else:
	print ("Folder \"gifs\" not exists!")
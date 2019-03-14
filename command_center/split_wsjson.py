import json

with open('data_ws.json') as f:
	data = json.load(f)
	for i in range(len(data)):
		with open('ws'+str(i)+'.json', 'w') as outfile:
			json.dump(data[i], outfile)
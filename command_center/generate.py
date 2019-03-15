import sys
sys.path.append('../editor')
import translate_new
import json

Data = translate_new.translate('../editor/tron.in')

with open('data.json', 'w') as outfile:
    json.dump(Data, outfile)

print('Generate OK')
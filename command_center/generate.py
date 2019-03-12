import sys
sys.path.append('../editor')
import translate
import json

Data = translate.translate('../editor/tron.in')

with open('data.json', 'w') as outfile:
    json.dump(Data, outfile)

print('Generate OK')
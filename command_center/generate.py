import sys
sys.path.append('../editor')
import translate
import json

Data = translate.translate('../editor/all_bright.in')

with open('data.json', 'w') as outfile:
    json.dump(Data, outfile)

print('Generate OK')
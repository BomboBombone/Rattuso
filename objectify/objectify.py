import sys
import os

content = '//embeds.h\n' \
          '#pragma once\n' \
          '\n'

i = 1
for arg in sys.argv[1:]:
    file = open(arg, 'rb')
    file_bytes = list(file.read(os.path.getsize(arg)))
    file.close()

    content += 'BYTE embedded_image_' + str(i) + '[] = ' + str(file_bytes).replace('[', '{').replace(']', '}') + ';\n\n'

    content += 'size_t embedded_image_' + str(i) + '_size = ' + str(len(file_bytes)) + ';\n'
    i += 1
    
output = open('embeds.h', 'wb')
output.write(bytes(content, 'utf-8'))
output.close()

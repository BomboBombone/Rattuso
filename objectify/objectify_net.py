import sys
import os

args = sys.argv
argc = len(sys.argv)

namespace = 'Program'
classname = 'Embeds'
class_modifier = ' '

to_pop = []
i = 0
args.pop(0)
for arg in args:
    if arg == '-n':
        namespace = args[i+1]
        to_pop.append(i)
        to_pop.append(i+1)
    elif arg == '-c':
        classname = args[i+1]
        to_pop.append(i)
        to_pop.append(i+1)
    elif arg == '-p':
        class_modifier += 'partial'
        to_pop.append(i)
    elif arg == '-s':
        class_modifier += 'static'
        to_pop.append(i)
    i+=1

to_pop.reverse()

for entry in to_pop:
    args.pop(entry)

content = '//embeds.cs\n' \
          'using System;\n' \
          '\n' \
          'namespace ' + namespace + \
          ' {\n' \
          '\tpublic' + class_modifier + \
          ' class ' + classname + \
          ' {\n'

i = 1
for arg in args:
    file = open(arg, 'rb')
    file_bytes = list(file.read(os.path.getsize(arg)))
    file.close()

    content += '\t\tpublic static byte[] embedded_image_' + str(i) + ' = ' + str(file_bytes).replace('[', '{').replace(']', '}') + ';\n\n'
    i += 1

content += '\t}\n}\n'

output = open('embeds.cs', 'wb')
output.write(bytes(content, 'utf-8'))
output.close()

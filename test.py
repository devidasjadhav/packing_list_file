import inflect
import random
import struct
p = inflect.engine()
s = struct.Struct('I 20s B')
f = open("./dummy",'wb+')
for x in range(100):
    r = random.randint(1,101)
    str_num = p.number_to_words(r)
    arr = (r,str_num.encode('utf-8'),len(str_num))
    print(arr)
    arr_bin = s.pack(*arr)
    f.write(arr_bin)
f.close()

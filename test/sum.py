
import time


starttime = time.time()


sum = 0.0
for i in range(1,100000):
    if (i % 2 == 0):
        sum = sum + 1.0 / i
    else:
        sum = sum + 1.0 / (i * i)
    
    i = i + 1

print(sum)
lasttime = time.time()
formatF = "{:.11f}".format(lasttime - starttime )
print(formatF)
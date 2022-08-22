sum = 0.0
for i in range(1,11):
    if (i % 2 == 0):
        sum = sum + 1.0 / i
    else:
        sum = sum + 1.0 / (i * i)
    
    i = i + 1

print(sum)
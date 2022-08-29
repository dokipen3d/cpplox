import time


def isprime(m):

    for j in range(2, m):
        if (m % j == 0):
            return False 
    return True



def primes(n):
    count = 0
    for i in range(2, n):
        if (isprime(i)): 
            count = count + 1
    return count

N = 5000

starttime = time.time()

print(primes(N))

lasttime = time.time()
print("Time: "+str(lasttime - starttime ))
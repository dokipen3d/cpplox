def Fibonacci(n): 
    if n <= 1: 
        return n
    # Second Fibonacci number is 1 
    return Fibonacci(n-1)+Fibonacci(n-2) 
  
# Driver Program 
#for i in range(0,20):
print(Fibonacci(35)) 
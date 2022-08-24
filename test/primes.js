fun isprime(m)
{
    for (var j = 2; j < m; j = (j + 1)) {
        //print j;
        //print m;
        if (m % j == 0) { 
            //print "nope";
            return false; 
        }
    }

    //print "true";

    return true;
}


fun primes(n)
{
    var count = 0;
    //print("in primes");
    //print(n);
    for (var i = 2; i <= n; i = i + 1) {
        //print i;
        if (isprime(i)) { 
            //print i;
            count = count + 1; 
        } 
    }
    //print "finished";
    return count;
}

var before = clock();

var N = 5000;

print(primes(N));

var after = clock();
print after - before;
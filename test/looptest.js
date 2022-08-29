var sum = 0.0;
for (var i = 1.0; i <= 100000; i = i + 1) {
    if (i % 2 == 0) {
        sum = sum + 1.0 / i;
    }
    else {
        sum = sum + (1.0 / (i * i));
    }
}
print sum;
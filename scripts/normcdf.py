# normcdf.py
# implementing the normcdf function in Python

# computes error function to desired accuracy epsilon
def erf(x, epsilon=0.000001):
    # since erf(x) is symmetric, we only focus on x > 0
    y = abs(x)

    # compute the Taylor series about zero
    # use the alternating error term to know when to stop
    term = y
    n = 0.0
    sign = +1.0
    factorial = 1.0
    power = y
    result = term
    while term > epsilon:
        n += 1
        factorial *= n
        sign = -sign
        power *= y * y
        term = power / (factorial * (2 * n + 1))
        result += sign * term

    # normalize and return result
    result *= 1.1283791670955125739  # this is 2/sqrt(pi)
    if x < 0:
        return -result
    return result

# computes normal cumulative distribution function from lower bounds a,
# upper bounds b, mean avg, standard deviation stdev, and desired accuracy
# epsilon
def normcdf(a, b, avg=0.0, stdev=1.0, epsilon=0.000001):
    # get z scores
    z_a = (a - avg) / stdev
    z_b = (b - avg) / stdev
    k = 0.7071067811865475244008  # this is 1/sqrt(2)

    # because we multiply by 0.5, we shouldn't need to scale epsilon
    return 0.5 * (erf(k * z_b, epsilon) - erf(k * z_a, epsilon))

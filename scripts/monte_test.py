# monte_test.py
# testing basic Monte Carlo integration methods
# okay, we'll cheat a bit and do some of the hard parts with numpy and scipy

import numpy as np
from scipy.stats import norm


#### simulates a definite integral with Monte Carlo methods
## parameters:
# f: function f(x) to integrate
# a, b: interval endpoints
# n: number of random samples to take
# alpha: level of confidence for estimate (two-sided)
## returns:
# (res, lo, hi): result and confidence interval
def mc_int(f, a=0.0, b=1.0, n=5000, alpha=0.05):
    # use mean value theorem to estimate integral
    xs = np.random.random(n) * (b - a) + a
    ys = map(f, xs)
    res = np.average(ys) * (b - a)

    # get critical z-score and margin of error
    z = norm.ppf(1.0 - alpha / 2.0)
    margin = z * np.std(ys, ddof=1) / np.sqrt(n)

    # return result and confidence interval
    return res, res - margin, res + margin



#### do lots of random trials to see if this really works
N = 5000                   # number of trials to run
true_val = 1 - np.cos(1)   # true value of the integral

# this should print around the level of confidence (95% by default)
print np.average([2 * np.abs(res - true_val) < hi - lo
                  for res, lo, hi in [mc_int(np.sin) for _ in range(N)]])

# norm.py
# drawing a normal histogram
# I was going to write a nice MATLAB/Octave file to do this, but stupid
# Debian refuses to install the statistics packages
# well, Python is more accessible, anyway

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.mlab as mlab

# general population parameters, in inches
avg_height = 69
stdev_height = 4
sample_size = 1000

# discrete plot
heights = np.random.normal(avg_height, stdev_height, sample_size)
plt.hist(heights, edgecolor='black')
plt.xlabel('Heights (in.)')
plt.ylabel('Frequency (out of %d)' % sample_size)
plt.title('Normally Distributed Heights')
plt.savefig('img/norm1.png', bbox_inches='tight')

# continuous plot
x = np.linspace(avg_height - 3 * stdev_height, avg_height + 3 * stdev_height)
y = mlab.normpdf(x, avg_height, stdev_height)
fig, ax = plt.subplots()
ax.plot(x, y, 'black')
ax.fill(x, y, '#1f77b4')
ax.set_ylim(0)
ax.set_xlabel('Heights (in.)')
ax.set_title('Normally Distributed Heights')
ax.set_ylabel('Relative Frequency')
fig.savefig('img/norm2.png', bbox_inches='tight')

# labeled the continuous plot
ax.axvline(x=avg_height, color="red", linestyle="--", label=r'$\mu$')
ax.axvline(x=avg_height + stdev_height, color="orange", linestyle="--", label=r'$\mu + \sigma$')
ax.legend()
fig.savefig('img/norm3.png', bbox_inches='tight')

plt.show()

import numpy as np
import matplotlib.pyplot as plt

# draw stuff
xs = np.linspace(-0.5, 1.5, 50)
interval = np.linspace(0, 1, 50)
fig, ax = plt.subplots()
ax.plot(xs, np.sin(xs), 'black')
ax.fill_between(interval, np.sin(interval), color='#1f77b4', label=r'$\int _a ^b \sin(x) dx$')
ax.axvline(x=0, color='red', linestyle='--', label='a (x=0)')
ax.axvline(x=1, color='orange', linestyle='--', label='b (x=1)')

# label stuff
ax.legend()
ax.set_ylim([-0.5, 1.1])
ax.set_xlabel('x (rad)')
ax.set_ylabel('sin(x)')
ax.set_title('definite integral of the sine function on [0,1]')

# save/show stuff
fig.savefig('img/sin_integral_plot.png', bbox_inches='tight')
plt.show()

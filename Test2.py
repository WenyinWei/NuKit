#!/usr/bin/env python

# <examples/doc_nistgauss2.py>
import matplotlib.pyplot as plt
import numpy as np

from lmfit.models import ExponentialModel, GaussianModel
N = 80
# dat = np.loadtxt('Gauss2.dat')
x = np.linspace(0, 40, N, endpoint=False)
y1 = np.exp(-x)
y2 = np.exp(-5 * x)
y = y1 + y2
# plt.plot(x, y1, 'b')
# plt.plot(x, y2, 'k--')
# plt.plot(x, y , 'r-')
# plt.show()

exp1 = ExponentialModel(prefix='e1_')
exp2 = ExponentialModel(prefix='e2_')


pars1 = exp1.guess(y, x=x);
pars2 = exp2.guess(y, x=x);
# pars1 = exp_mod.guess(y[:ix1], x=x[:ix1])
# pars2 = gauss1.guess(y[ix1:ix2], x=x[ix1:ix2])
# pars3 = gauss2.guess(y[ix2:ix3], x=x[ix2:ix3])

pars = pars1 + pars2
mod = exp1 + exp2

out = mod.fit(y, pars, x=x)

print(out.fit_report(min_correl=0.5))

plt.plot(x, y, 'b')
plt.plot(x, out.init_fit, 'k--')
plt.plot(x, out.best_fit, 'r-')
# plt.savefig('../doc/_images/models_nistgauss2.png')
plt.show()
# <end examples/doc_nistgauss2.py>
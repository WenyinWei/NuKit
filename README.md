# NuclearKit

Here restores the tools for nuclear science analysis 

本项目起源于中国北京清华大学的学生魏文崟于2019年的核工程课程大作业.

> 编写燃料中核同位素成分随时间(燃耗)变化的计算机程序。可考虑采
> 用不同的数学方法：解析方法和数值方法；采用不同的燃耗方式：定
> 功率燃耗和定通量燃耗。编程时应考虑网络实现和界面的友好程度。

我将该课题分成几个部分:

1. 衰变链的数学刻画

   1. 设定一个起始核素,(即母核)

   2. 它会生成各种各样的子核,要求各种子核数量对时间的变化率

   3. 与解析方法不太合适的是,实际中衰变可能会沿着不同的方向进行,如果继续用解析方法那就太麻烦了.如果我知道了某一种核素自身的衰变常数和母核种类及数量,我就应该也知道它自身的变化规律.(下式中$N_i$为母核数量)

      $$ \frac{dN}{dt} = - \lambda N + \sum_i \lambda_i N_i$$

      $$ \frac{dN}{dt} = - \frac{ln2}{T_{1/2}} N + \sum_i \frac{ln2}{T_{1/2}} N_i$$

      (TODO)　由于 IAEA 搜索页面包含同素异能态,我们的程序内部已经包含有.

      (Possible Bug)当变量极小或极大时,Python可能运算误差会很大,看看decimal能不能解决这一问题.

   4. 运行流程:

      1. 给定初始状态的核素种类和数量
      2. 查询以它们为起点所有可能产生的核素以对应的衰变周期.
      3. 对当前核素的数量进行偏微分方程数值更新.

2. 针对核数据库的爬虫 https://www-nds.iaea.org/relnsd/NdsEnsdf/QueryForm.html
这一部分已经实现了衰变链的获取.

为了做出重同位素随时间的变化,2D的热力图可以考虑

- https://plot.ly/python/2D-Histogram/
- https://help.plot.ly/make-a-2d-histogram-heatmap/
- https://plot.ly/python/histograms/
- https://python-graph-gallery.com/82-marginal-plot-with-seaborn/
- 这个用来绘空间变化应该很好 http://people.bu.edu/andasari/courses/numericalpython/python.html

我发现 odeint 甚至无法指定算法,我决定不使用它而是用更开放的 odespy. 
Remark: odespy 已经过期了,它只适用于python2.7,我现在在用C++程序包odeint
3. 解析方法实现

4. 数值方法实现

5. 不同的燃耗方式:定功率燃耗和定通量燃耗

6. 

https://lmfit.github.io/lmfit-py/builtin_models.html#lmfit.models.ExpressionModel
https://lmfit.github.io/lmfit-py/model.html#composite-models-adding-or-multiplying-models
https://itl.nist.gov/div898/strd/nls/data/gauss2.shtml
https://itl.nist.gov/div898/strd/nls/nls_main.shtml
https://lmfit.github.io/lmfit-py/builtin_models.html#exponentialmodel
http://cars9.uchicago.edu/software/python/lmfit/

1. ***References for C++ odeint***

   - [Solving ordinary differential equations in C++](https://www.codeproject.com/Articles/43607/Solving-ordinary-differential-equations-in-C)
   - [odeint v2 - Solving ordinary differential equations in C++](https://www.codeproject.com/Articles/268589/odeint-v2-Solving-ordinary-differential-equations)
   - [odeint--An advanced C++ framework for numerical integration of
     ordinary differential equations](https://headmyshoulder.github.io/odeint-v2/odeint_talk.pdf)
   - [Homepage of odeint](http://headmyshoulder.github.io/odeint-v2/index.html)
   - [Numerical integration in C++ with Boost odeint](http://boccelliengineering.altervista.org/junk/boost_integration/boost_odeint.html)

2. ***References for C++ Visualization***

   Two way for visualization in C++ are considerated

   - Qwt
   - MathGL

3. ***References for fission product***

   - [Fission product yield](https://wikivisually.com/wiki/Fission_product_yield) 
   - [Fission product yield](https://ipfs.io/ipfs/QmXoypizjW3WknFiJnKLwHCnL72vedxjQkDDP1mXWo6uco/wiki/Fission_product_yield.html)

4. ***References for neutron cross section***

   - [Neutron cross-sections](http://www.kayelaby.npl.co.uk/atomic_and_nuclear_physics/4_7/4_7_2.html)

中子屏蔽中的关键问题
聚变设备中的新型中子屏蔽环流设计
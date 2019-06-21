1、文件夹C++程序中是用WardDuer模型拟合BRDF模型参数的C++工程，使用了LM算法库levmar-2.6和opencv 2.4.13，其中附上了opencv库和LM算法库（编译过）。
     工程中需要用到compiler.h、levmar.h、lm.h、misc.h和Axb.c、lm.c、misc.c。

     其中BRDFfitting.cpp是BRDF参数拟合类的具体函数定义，BRDFfitting.h是对该类的声明。
     该类的公有成员函数StartFitting(int BRDFSerialNumber)和BRDFSample(int BRDFSerialNumber)传入参数为需要拟合的原始数据的文件的序号（序号为1-100之间）。
     该类的私有函数ComputeSamplingAngle()目前是对thetaOut和thetaIn以10:5:80、phiIn以5:5:175采样的，剔除了仰角低于10度和高于80度的数据，因为这些数据误差较大，而且采用每隔5度均匀采样以提高可靠性。
     后续可根据采样的需求更改优化。

     如何使用该工程：调用该类的开始拟合函数StartFitting(int BRDFSerialNumber)，传入文件名称序号即可，默认将该原始文件放到文件夹Param中，所有相关的文件都按照序号存放在这里，如采样后的文件以“brdf_simple.binary”形式存储，拟合结果以“WdDu_parameter.txt”存储。

     之所以使用WardDuer模型是由于经过我们的测试以及视觉分析对比，认为CookTorrance和WardDuer模型表现总体最好，而CookTorrance模型不适用于后面要对接的svBRDF。

2、文件夹Matlab程序中包含Fitting拟合代码和遗传算法使用的原始数据Param10_175和MERL文章给出的结果MERL_Param。
     Fitting拟合代码中是6个模型拟合代码如：Objfun_WardDuer.m等等，6个模型拟合误差函数如：fun_WardDuer.m等等，6个遗传算法计算部分参数初值的函数如：Genetic_algorithm_WardDuer.m等等。
     另外还有一个BRDF材质读取函数：BRDFRead.m以及一个采样角度计算函数：ComputeSamplingAngle.m。       
     Fitting拟合程序文件夹中有个Sheffield文件夹，这是谢菲尔德遗传工具箱，遗传算法计算初值时需要用到其中的函数，因此使用拟合程序时需要把这个文件夹添加到路径当中。
    

3、文件夹Param中存放的是使用的BRDF原始数据、采样后的数据和最终的拟合结果。


4、name.txt是Param中100种序号命名的BRDF文件对应的材质名称。


5、Fitting拟合程序中设置了两种初值计算的方法，一种是根据遗传算法得到一个比较可靠的初值，缺点是计算比较耗时；另一种方法是使用经验推荐的初值，这种方法能比较大地提高拟合效率，但是并不是所有材质所有模型都能很可靠，后面可能还会遇到一些比较复杂的材质等情况，需要根据情况考虑使用。
     经过测试，对于MERL数据库，这6个模型除了Lafortune模型难以用一个根据经验推荐的初值得到稳定的结果外，其他5个模型都能根据Fitting拟合程序中推荐的经验初值得到和遗传算法计算初值一致的拟合结果，这相对于使用遗传算法计算初值能节省很多时间。
     针对这一特点，我们Lafortune模型最好使用遗传算法计算初值，其他5种模型推荐使用经验推荐的初值，如果认为个别材质的结果不理想，再针对个别材质使用遗传算法重新拟合进行结果的比较。
     C++程序中WardDuer模型使用的是经验推荐的初值，经过测试对于MERL数据库的材质，该模型使用遗传算法得到的结果与使用经验初值得到的结果一样。该方法能提高拟合效率同时减少程序的繁杂。
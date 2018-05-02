## linux memory addr


这里面代码我不是原创者，我无意剽窃抄袭原创者的劳动成果，我尊重他们的劳动成功，我感激他们的分享精神。

这是一份学习资料，原作者运行在较老的内核上，我对部分代码稍作了的一点点的修改和重写，使之能够在在`3.10.0-693.21.1` 内核上运行。

对提供`dram.ko`模块，和`sys_reg.ko`模块的作者致敬，对[ilinuxkernel](http://ilinuxkernel.com/)博主致敬。

我的修改如下：

* 重写[dram](./dram)
* 重写[sys_reg](./sys_reg)
* 重构简化[mem_map](mem_map)

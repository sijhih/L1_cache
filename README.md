# L1_cache
本项目是单级 Cache 的建模与性能分析

实现输入交互，用户可选择两种替换策略：LFU/LRU，两种写策略：WBWA/WTNA,并指定trace文件作为输入

实现cache的功能模拟

输出模拟下cache最终的存储内容，记录读缺失、写缺失次数等，计算cache性能

## 文件结构
src：包含源码初始框架和Makefile文件

trace：包含实验所需的所有trace文件，即读/写访存地址流

validation：包含了所有仿真结果验证文件

debug：包含了所有仿真器调试文件

Proj1-1_Specification：实验指导书

## 运行环境
32bit，x86 OS with gcc

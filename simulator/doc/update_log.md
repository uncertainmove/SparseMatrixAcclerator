# BUGS

## 1. P4 Offset处理问题

Offset在Uram中采用左开右闭的存储方式，如：[2, 3)表示点关联1条边，[2, 2)表示点无关联边

问题1：P4通过<font color=red>loff + 1 == roff<font color=red>来判断孤立点，导致孤立点不会被过滤，执行不报错是因为孤立点在P1阶段被提前过滤掉了
解决方案：去掉+1

问题2：P4通过<font color=red>loff / CACHE_LINE == roff / CACHE_LINE<font color=red>来判断点是否需要额外的访存轮次，不会导致逻辑错误，但是会导致额外一轮无效访存
解决方案：使用<font color=red>loff / CACHE_LINE == roff / CACHE_LINE<font color=red>判断
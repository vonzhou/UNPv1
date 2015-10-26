# Source Deduplication
---



## 设计

1. 用双端口file fp走单独的端口

2. 文件分块（定长）传输，服务器端使用redis做索引（file name->fp, fp-> file names） 

3. TCP Nagle问题


![](server02_res.png)

![](server02_redis.png)



TODO 


vonzhou 2015.10.26

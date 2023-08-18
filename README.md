

# SparseMatrixAcclerator

## 框架使用说明

### 框架数据结构使用说明

#### Pipe连接

##### Self_config.h配置

config(name,var) 中Info_name用来标识类名，var则用来声明类成员

具体例子如下：

```c++
config(P7,int P7_Source_Core_Full[CORE_NUM]; 

  int P7_Om2_Send_Push_Flag[CORE_NUM]; 

  int P7_Om2_Send_Update_V_ID[CORE_NUM]; 

  int P7_Om2_Send_Update_V_Value[CORE_NUM]; 

  int P7_Om2_Send_Update_V_Pull_First_Flag[CORE_NUM]; 

  int P7_Om2_Send_Update_V_DValid[CORE_NUM]; 

  int P7_Om2_Iteration_End[CORE_NUM]; 

  int P7_Om2_Iteration_End_DValid[CORE_NUM]; )
```

对于需要对管道读写的变量，则在函数内声明对应的变量，调用管道管理者进行读写即可；

```c++
  static Info_P7 self_info_p7;

  pipes->write(&self_info_p7);
```

#### 组数据结构

使用样例在：

/SparseMatrixAcclerator/simulator/src/Compute_Core/2_RD_Active_Vertex_Offset_container.cpp

##### 队列组数据构建

```cpp
static queue_container<int> v_container[CORE_NUM];
for(int i = 0 ; i<CORE_NUM ; i++){
            v_container[i].combine({&push_flag_buffer[i], &v_id_buffer[i], &v_value_buffer[i],&pull_first_flag_buffer[i]});
}
```

##### 队列组数据push行为

```cpp
if (Front_Active_V_DValid) {
        v_container[Core_ID].push(Front_V_container[Core_ID]);
}
```

##### 队列组数据pop行为

```cpp
v_container[Core_ID].pop();
```



##### 值组数据构建

```cpp
static value_container<int> V_container[CORE_NUM];
...
V_container[Core_ID].combine({Push_Flag, Active_V_ID, Active_V_Value, Active_V_Pull_First_Flag});
```

##### 值组数据赋值行为

```cpp
V_container[Core_ID].assign({-1,0,0,0});
```





### 框架Debug使用说明

##### dcout支持

`dcout << v_buffer_empty[Core_ID] <<endl;`

dcout 支持debug模式下的debug

##### GTest框架下完成测试函数，以2_RD_Active_Vertex_Offset_container.cpp为例；

需要声明debug对象，例如：

`debug_RD_ACTIVE_VERTEX_OFFSET_Container debug_M2;`

为了方便编译，定义了exp_debug宏定义，在测试时不使用用main.cpp的外部变量；

```cpp
#ifdef exp_debug

  int clk = 0;

  int rst_rd = 0;

#else

  extern int clk;

  extern int rst_rd;

#endif
```

debug模式下，rst_rd在调用文件内计算

```cpp
#ifdef exp_debug

  rst_rd = (clk < 100) ? 1 : 0;

#endif
```

clk同理

```cpp
#ifdef exp_debug

  clk++;

#endif
```

debug.h中组数据结构用于debug框架：

```cpp
class debug_RD_ACTIVE_VERTEX_OFFSET_Container {

public:

    queue_container<int> *_v_container;
    value_container<int> *_Front_V_container,*_V_container,*_Active_V_container;

};
```

在模块里接入组结构；

```cpp
  debug_M2._Front_V_container = Front_V_container;
  debug_M2._V_container = V_container;
  debug_M2._Active_V_container = Active_V_container;
  debug_M2._v_container = v_container;
```

在测试函数里容器支持一些数据测试；

`ASSERT_EQ(debug_M2._v_container[i].size(), size_t(0));`



### 框架模块使用说明

#### 流水线

流水线内布线使用pipe进行连接，样例在 /SparseMatrixAcclerator/simulator/src/Compute_Core/core_pipe.cpp

样例中RD_ACTIVE_VERTEX和Read_Active_Vertex_Offset间的连接是通过self_info_p12p2来完成的

声明如下：

```cpp
config(P12P2,int P1_Push_Flag[CORE_NUM];
       int P1_Active_V_ID[CORE_NUM];
       int P1_Active_V_Value[CORE_NUM];
       int P1_Active_V_Pull_First_Flag[CORE_NUM];
       int P1_Active_V_DValid[CORE_NUM];
       int P1_Iteration_End[CORE_NUM];
       int P1_Iteration_End_Dvalid[CORE_NUM];)
```

连接如下：

```cpp
    static Info_P12P2 self_info_p12p2_rd,self_info_p12p2_wr;
    pipes->write(&self_info_p12p2_wr);
	RD_ACTIVE_VERTEX(self_info_p_aie_global_receive.AIE_Active_V_ID,                                          self_info_p_aie_global_receive.AIE_Active_V_Updated,                                      AIE_Active_V_Pull_First_Flag_rd,                                                          self_info_p_aie_global_receive.AIE_Active_V_DValid,
                     AIE_Iteration_End_rd, 
                     AIE_Iteration_End_DValid_rd,
                     P2_Stage_Full_rd,

                     self_info_p12p2_rd,
                     self_info_p1.P1_Global_Iteration_ID);


    Read_Active_Vertex_Offset(self_info_p12p2_wr,
                              P4_Stage_Full_rd,

                              P2_Stage_Full_wr,
                              P2_Push_Flag_wr, 
                              P2_Active_V_ID_wr, 
                              P2_Active_V_Value_wr,                                                                     self_info_offset2uram.P2_Active_V_Offset_Addr,                                           P2_Active_V_Pull_First_Flag_wr,                                                           self_info_offset2uram.P2_Active_V_DValid,
                              P2_Iteration_End_wr, P2_Iteration_End_DValid_wr);
   pipes->read(&self_info_p12p2_rd);
```



#### 网络



##### 片上网络

```
-----接口(管道)------
组数据声明;
节点间连线声明,组数据构建;
节点内部拓扑结构通过组数据连接;
--------------------
for(int i = 0 ; i<CORE_NUM ; i++)
--------------------------------
节点内部执行逻辑函数(管道数据,组数据)|
--------------------------------
--------------------
```



#### 存储



##### 片上缓存

声明片上缓存，初始化参数为BRAM存储大小；

```cpp
BRAM vtx_bram(MAX_VERTEX_NUM / CORE_NUM + 1);
BRAM edge_bram(MAX_VERTEX_NUM / CORE_NUM + 1);
```

片上缓存使用时直接调用即可

```cpp
vtx_bram.BRAM_IP(rst_rd,
                    vtx_bram_Read_Addr, vtx_bram_Read_Addr_Valid,
                    vtx_bram_Wr_Addr, vtx_bram_Wr_Data, vtx_bram_Wr_Addr_Valid,

                    vtx_bram_data, vtx_bram_data_valid);

edge_bram.BRAM_IP(rst_rd,
                   P4_Rd_BRAM_Edge_Addr_rd, P4_Rd_BRAM_Edge_Valid_rd,
                   edge_bram_Wr_Addr, edge_bram_Wr_Data, edge_bram_Wr_Addr_Valid,

                   edge_bram_data, edge_bram_data_valid);
```



看lpddr5的速率，物理长宽
1. 聚焦人类发展国家发展的问题
2. 国家战略，科技突破，社会认同感
3. 坚持问题导向，知道哪些肯定要做，哪些肯定不能做
4. 坚持一体两核多点
5. 之江实验室的两弹一星是什么
6. 我们能为之江实验室做什么

5解析
1. 和国家最强的机构合作
2. 提供计算能力
3. 做厚平台
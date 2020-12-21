# STM32 BMP280 阿里云MQTT

[TOC]

## BMP280 I2C

### 1.HAL库中实现I2C的重要函数

| HAL_I2C_Mem_Read（Write同理） |                     |
| :---------------------------- | ------------------- |
| hi2c                          | i2c的一个实例       |
| DevAddress                    | 设备的slave address |
| MemAddress                    | 设备的寄存器地址    |
| MemAddSize                    | 设备的寄存器大小    |
| pData                         | 设备的寄存器值      |
| Size                          | 数据个数            |
| Timeout                       | 最大等待时间        |



### 2.bmp280寄存器

![image-20200709230855007](C:\Users\Y\AppData\Roaming\Typora\typora-user-images\image-20200709230855007.png)

- ctrl_meas  寄存器

用于使能      oversampling:配置精度值      power mode:是否打开传感器

![image-20200709225330534](C:\Users\Y\AppData\Roaming\Typora\typora-user-images\image-20200709225330534.png)

![image-20200709225401366](C:\Users\Y\AppData\Roaming\Typora\typora-user-images\image-20200709225401366.png)

temperature oversampling配置成001：精度为0.005℃     pressure oversampling不使用          power mode配置成11：普通模式  因此ctrl_meas寄存器配置成00100011(0x23)



- calibration data校准数据

每一个传感器在生产的时候会有一个calibration data，因此在计算时需要用到这些校准参数来获得真实的数值。因为每个芯片由不同的地区生产，它的校验的值是不一样的，所以这个校验的值会被写入到芯片当中。所以说在使用时还需要读取芯片的calibration data。

​	calib00...calib25寄存器（地址0x88...0xA1）：3个温度的calibration data 和 9个压强，都是16位的数据。

![image-20200709225251178](C:\Users\Y\AppData\Roaming\Typora\typora-user-images\image-20200709225251178.png)

- 读温度寄存器

![image-20200709231000715](C:\Users\Y\AppData\Roaming\Typora\typora-user-images\image-20200709231000715.png)

这三个数据的寄存器地址连续，因此可以通过连续读取来获得，起始地址是0xFA

通过手册提供的计算方式，calibration data 和 读取到的温度数据可以计算得到一个最终温度。

![image-20200709233608428](C:\Users\Y\AppData\Roaming\Typora\typora-user-images\image-20200709233608428.png)

![image-20200709233348667](C:\Users\Y\AppData\Roaming\Typora\typora-user-images\image-20200709233348667.png)

## GPRS模块

市面上买的大部分GPRS模块都是串口。

需要串口接收模块发送过来的AT指令，单片机也可以发送AT指令控制模块。

**我们需要打通STM32的串口收发通路**

将串口的TX，RX短接，在cubemx中配置好UART中断

发送数据   HAL_UART_Transmit(&huart2, (uint8_t *)"Hello", 5, 0xFFFF);

重点是：需要在主循环之前手动开启接收中断，接收中断是：

```c
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
```

而此函数会调用：（只需要把相应代码写到此回调函数中即可）

```c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */
  printf("%c", recv_buf);
  HAL_UART_Receive_IT(&huart2, &recv_buf, 1);
}
```

**最需要注意的一点是：**
在回调函数最后需要再次开启中断，否则只能执行一次。

## JSON介绍

本项目中并不需要移植或者导入jansson库。

- 移植cJson -> keil , 可以上github搜源码，部分函数重新实现一下

- jansson -> keil ，这已经被keil收纳在库中，可以直接导入

  首先需要去keil官网下载jansson的包

  ![image-20200710170705792](C:\Users\Y\AppData\Roaming\Typora\typora-user-images\image-20200710170705792.png)

http://json.cn/ 可以直接解析json格式的数据

json_pack()和json_dumps(), json包装时的参数如下
    ![image-20200710182614124](C:\Users\Y\AppData\Roaming\Typora\typora-user-images\image-20200710182614124.png)

```c
//json数据打印测试
void json_test(void)
{
  json_t *root;
  char *str;
  
  root = json_pack("{sisfss}",
          "id", 123,
          "temp", 23.8,
          "method", "post");
  str = json_dumps(root, JSON_ENCODE_ANY);
  if(str != NULL){
    printf("str: %s\r\n", str);
  } else {
    printf("failed to dumps root\r\n");
  }
  
  free(root);//释放内存
  free(str);
}
```

解析json数据 https://github.com/akheron/jansson

注意，解析和生成json的时候要保证有足够的堆空间，如果堆大小不够会处理失败。

![image-20200712173742550](C:\Users\Y\AppData\Roaming\Typora\typora-user-images\image-20200712173742550.png)

## 阿里云MQTT

### 1.阿里云IOT平台

阿里云针对物联网使用的是MQTT协议。MQTT里面封装的数据是json格式的。

MQTT是以Topic订阅的方式去实现的，有一个MQTT的服务器，第三方可以任意的订阅和发送数据。

注册产品后，可以得到产品的一些信息。

![image-20200710201557940](C:\Users\Y\AppData\Roaming\Typora\typora-user-images\image-20200710201557940.png)

{  "ProductKey": "a139Uz564DX",  "DeviceName": "STM32F407ZG_NODE001",  "DeviceSecret": "eb81c3b307ec1267725af71de84e36f9" }

### 2.MQTT.fx

MQTT.fx可以模拟发送json数据给阿里云。

https://help.aliyun.com/document_detail/86706.html

![`image-20200710212831223`](C:\Users\Y\AppData\Roaming\Typora\typora-user-images\image-20200710212831223.png)

要将传感器数据上传到云端，需要用到的Topic

![image-20200710213259890](C:\Users\Y\AppData\Roaming\Typora\typora-user-images\image-20200710213259890.png)

Alink请求数据格式

```json
{
  "id": 123,
  "version": "1.0",
  "params": {
    "configScope": "product",
    "getType": "file"
  },
"method": "thing.config.get"
}
```

在MQTT.fx中模拟

```json
{
  "id": "12345",
  "version": "1.0",
  "params": {
    "CurrentTemperature": 28.4
  },
  "method": "thing.event.property.post"
}
```

订阅，阿里云会返回一个数据

![image-20200710215740881](C:\Users\Y\AppData\Roaming\Typora\typora-user-images\image-20200710215740881.png)

![image-20200710215823261](C:\Users\Y\AppData\Roaming\Typora\typora-user-images\image-20200710215823261.png)

## WIFI

0.ESP8266初始化。检查ESP8266是否存在。

1.ESP8266连接热点。设置为STATION模式，设置热点名与热点密码。

2.ESP8266连接阿里云IOT服务器。使用指定协议(TCP/UDP)连接到服务器，配置IP和端口号。

3.ESP8266通过MQTT登录阿里云，进行发布和订阅。由于ESP8266不自带MQTT，因此需要移植MQTT驱动。



## 微信小程序

https://help.aliyun.com/document_detail/129695.html


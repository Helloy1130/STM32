——————————————
IO连接：
-STM32_F407相关-
-与AD9910-
AD9910_PWR      PDout(3)//0
AD9910_SCLK     PCout(6)
AD9910_CS       PCout(7)
AD9910_SDO      PCout(8)//SDIO
MASTRST    	    PCout(9)
UP_DAT          PAout(8)
PROFILE0   	    PAout(15)
PROFILE1        PCout(10)
PROFILE2        PCout(11)
DRHOLD     	    PCout(12)
DRCTL           PDout(2)
-与FPGA-    
SPI_SCLK        PEout(2)
SPI_CS          PEout(3)
SPI_MISO	    PEout(5)
startSignal(高电平有效) PEout(6)
-与继电器-
输入输出        PAout(4)//低电平输入，高电平输出
采样电阻        PAout(5)//低电平接采样电阻，高电平不接
负载            PAout(6)//低电平空载，高电平带载

-FPGA_黑金Cyclone4相关-
startSignal(去检测上升沿，开始存数据，存完状态机指导发送数据)
SPI_SCLK        	
SPI_CS          	
SPI_MISO		
——————————————
STM32与FPGA的SPI通信：
完整spi数据传输（22组16位数据）
16位一次传输
MSB16位:   开始标志0x1100
16位数据*20
16位：     结束标志0xff00
——————————————

**********************************
采样电阻 15Kohm
输出负载 1Kohm
INA上的增益电阻RG 150ohm ---------> INA增益=1+50K/150=334.33
实测输出电压衰减为0.5794倍
ADS8505测量精度为30.5mV
**********************************
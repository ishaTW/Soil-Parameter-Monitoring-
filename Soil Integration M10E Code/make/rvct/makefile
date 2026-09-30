
#-------------------------------------------------------------------------------
# config tools, tool path
#-------------------------------------------------------------------------------
ARMTOOL_PATH=C:\Progra~1\ARM\RVCT


#-------------------------------------------------------------------------------
# config version
#-------------------------------------------------------------------------------
PLATFORM=APPGS2MD
MEMORY=M64
VERSION=A01


#-------------------------------------------------------------------------------
# config build dir
#-------------------------------------------------------------------------------
BUILD_DIRS=ril customer example

#-------------------------------------------------------------------------------
# config link libs
#-------------------------------------------------------------------------------
LIBS=


all:
	@$(MAKE) help

help:	
	@echo **********************help*******************
	@echo  ==========================================================
	@echo  === Please input the following command to make example ===
	@echo  ==========================================================
#-------------------------------------------------------------------------------
# Basic examples
#-------------------------------------------------------------------------------
	@echo  make rvct custom_app
	@echo  make rvct call
	@echo  make rvct sms
	@echo  make rvct time
	@echo  make rvct timer
	@echo  make rvct simpletcpip
	@echo  make rvct gpio
	@echo  make rvct eint
	@echo  make rvct pwm
	@echo  make rvct clk
	@echo  make rvct adc
	@echo  make rvct iic
	@echo  make rvct spi	
	@echo  make rvct transpass
	@echo  make rvct memory
	@echo  make rvct file
	@echo  make rvct system
	
#-------------------------------------------------------------------------------
# Advance examples
#-------------------------------------------------------------------------------
	@echo  -------------------
	@echo  make rvct multitask
	@echo  make rvct multitask_port
	@echo  make rvct fota_ftp
	@echo  make rvct fota_http
	@echo  make rvct tcpclient
	@echo  make rvct udpclient
	@echo  make rvct tcpserver
	@echo  make rvct udpserver
	

#-------------------------------------------------------------------------------
# Basic examples
#-------------------------------------------------------------------------------
custom_app:
	@echo *******make custom*********
	@echo -D__CUSTOMER_CODE__ >> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile
	
call:
	@echo *******make call example*********
	@echo -D__EXAMPLE_CALL__ >> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile
	
sms:
	@echo *******make sms example*********
	@echo -D__EXAMPLE_SMS__ >> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile

time:
	@echo *******make time*********
	@echo -D__EXAMPLE_TIME__ >> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile

timer:
	@echo *******make timer *********
	@echo -D__EXAMPLE_TIMER__ >> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile

simpletcpip:
	@echo *******make simple tcpip example*********
	@echo -D__EXAMPLE_SIMPLE_TCPIP__>> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile
	
gpio:
	@echo *******make gpio example*********
	@echo -D__EXAMPLE_GPIO__ >> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile
eint:
	@echo *******make eint example*********
	@echo -D__EXAMPLE_EINT__ >> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile

pwm:
	@echo *******make pwm example*********
	@echo -D__EXAMPLE_PWM__ >> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile

clk:
	@echo *******make clk example*********
	@echo -D__EXAMPLE_CLK__ >> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile

adc:
	@echo *******make adc example*********
	@echo -D__EXAMPLE_ADC__ >> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile

iic:
	@echo *******make iic example*********
	@echo -D__EXAMPLE_IIC__ >> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile	
	
spi:	
	@echo *******make spi example*********
	@echo -D__EXAMPLE_SPI__ >> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile

transpass:
	@echo *******make transpass example*********
	@echo -D__EXAMPLE_TRANSPASS__>> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile	

memory:
	@echo *******make memory example*********
	@echo -D__EXAMPLE_MEMORY__ >> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile

file:
	@echo *******make file example*********
	@echo -D__EXAMPLE_FILESYSTEM__ >> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile	

file_sd:
	@echo *******make file example to test SD card*********
	@echo -D__EXAMPLE_FILESYSTEM__ -D__TEST_FOR_MEMORY_CARD__ >> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile

system:
	@echo *******make system *********
	@echo -D__EXAMPLE_SYSTEM__ >> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile

http:
	@echo *******make http test *********
	@echo -D__EXAMPLE_HTTP__ >> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile


#-------------------------------------------------------------------------------
# Advance examples
#-------------------------------------------------------------------------------
multitask:
	@echo *******make multitask example*********
	@echo -D__EXAMPLE_MULTITASK__>> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile	
	
multitask_port:
	@echo *******make multitask_port example*********
	@echo -D__EXAMPLE_MULTITASK_PORT__>> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile	
	
fota_ftp:	
	@echo *******make fota_ftp example*********
	@echo -D__EXAMPLE_FOTA_FTP__ >> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile	
	
fota_http:	
	@echo *******make fota_http example*********
	@echo -D__EXAMPLE_FOTA_HTTP__ >> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile
	
tcpclient:
	@echo *******make tcpclient example*********
	@echo -D__EXAMPLE_TCPCLIENT__>> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile	

udpclient:
	@echo *******make udpclient example*********
	@echo -D__EXAMPLE_UDPCLIENT__>> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile
	
tcpserver:	
	@echo *******make tcpserver example*********
	@echo -D__EXAMPLE_TCPSERVER__ >> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile

udpserver:	
	@echo *******make udpserver example*********
	@echo -D__EXAMPLE_UDPSERVER__ >> .\$(DEFVIATMP)
	@$(MAKE) new -f make\rvct\makefile	


include make\rvct\makefiledef

export ARMTOOL_PATH BUILD_DIRS

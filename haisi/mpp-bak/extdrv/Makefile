# $(ROOT)/mpp/Mafile


#--Compiling 'extdrv'... Configs as follow:
#---- CROSS=arm-hisiv100nptl-linux-
#---- HIARCH=hi3520D, HICHIP=0x3520D100, CVER=.C01, HIDBG=HI_DEBUG
#---- SDK_PATH=/opt/sda5/priv/haisi/Hi3520D_V100R001C01SPC030/release/01.software/board/bakup/Hi3520D_SDK_V1.0.3.0 , PARAM_FILE=../Makefile.param
#---- LINUX_ROOT=/opt/sda5/priv/haisi/Hi3520D_V100R001C01SPC030/release/01.software/board/bakup/Hi3520D_SDK_V1.0.3.0/osdrv/kernel/linux-3.0.y






ifeq ($(PARAM_FILE), ) 
    PARAM_FILE:=../Makefile.param
    include $(PARAM_FILE)
endif

export EXTDRV_KO=$(REL_KO)/extdrv


.PHONY:clean all rel
all:
	@echo -e "\e[0;32;1m--Compiling 'extdrv'... Configs as follow:\e[0;36;1m"
	@echo ---- CROSS=$(CROSS)
	@echo ---- HIARCH=$(HIARCH), HICHIP=$(HICHIP), CVER=$(CVER), HIDBG=$(HIDBG)
	@echo ---- SDK_PATH=$(SDK_PATH) , PARAM_FILE=$(PARAM_FILE)
	@echo ---- LINUX_ROOT=$(LINUX_ROOT)
	@echo ---- EXTDRV_KO=$(EXTDRV_KO)
	@echo -e "\e[0m"
	@mkdir -p $(EXTDRV_KO)
	@for x in `find ./ -maxdepth 2 -mindepth 2 -name "Makefile" `; do\
	   { cd `dirname $$x`; if [ $$? ]; then make || exit 1;  cp *.ko $(EXTDRV_KO); cd ../; fi;}& done; wait
	cp gpio_i2c/i2c_write $(EXTDRV_KO)   

clean:
	@for x in `find ./ -maxdepth 2 -mindepth 2 -name "Makefile" `; do\
	   { cd `dirname $$x`; if [ $$? ]; then make clean; cd ../; fi;}& done; wait
	@if [ -e $(EXTDRV_KO) ];then rm $(EXTDRV_KO) -fr; fi
	@if [ -e $(REL_KO) ];then rmdir --ignore-fail-on-non-empty "$(REL_KO)"; fi


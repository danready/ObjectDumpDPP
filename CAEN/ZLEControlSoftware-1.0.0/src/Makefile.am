bin_PROGRAMS=ZLEControlSoftware
ZLEControlSoftware_SOURCES=ZLEControlSoftware.c ZLECSconfig.c ZLECSplot.c keyb.c
ZLEControlSoftware_LDADD = -lCAENComm -lCAENDigitizer -lm -lpthread
INCLUDES = -I../inc
AM_CFLAGS=  -fPIC
datadir=/etc/ZLEControlSoftware/
dist_data_DATA=../setup/Linux/ZLEControlSoftwareConfig.txt
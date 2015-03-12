export MAKEROOT := $(shell pwd)

TARGET_DIRS := leds \
	keys \
	pwm \
	adc \
	ds18b20 \
	lcds70 \
	projectlib \
	main_routine \

include ./env.mk	

define build_obj
for SubDir in $(TARGET_DIRS); do \
	if ! [ -d $$SubDir ]; then \
	    echo "The $$SubDIr is not exist !"; \
	    exit 11; \
	fi; \
	echo "Building $$SubDir ..."; \
	make -C $$SubDir; \
	if [ $$? -ne 0 ]; then \
	    echo "Building $$SubDir is failed !"; \
	    exit 12; \
	fi;\
done

endef

all:
	@$(call build_obj)
	$(CC) -o S_Hatress_App *.o $(CROSS_COMPILE_LIB_PATH) $(CROSS_COMPILE_LIB)

.PHONY: clean
clean:
	rm S_Hatress_App
	rm -f *.o;


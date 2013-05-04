OBJS = main.o ps2.o stm32f10x_misc.o stm32f10x_it.o stm32f10x_init.o

all: firmware.a

firmware.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

clean:
	rm -f firmware.a $(OBJS)

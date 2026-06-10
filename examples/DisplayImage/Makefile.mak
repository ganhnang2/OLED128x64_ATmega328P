#==========================USER CONFIGURTION============================#
# Cấu hình MCU đang sử dụng, tần số clock cho CPU, BaudRate và công cụ nạp (arduino, usbasp, ...)
# NOTE: 
# 1. Trong Makefile này, tôi đã nạp code trực tiếp vào ATmega328P trên board Arduino
# 2. Tên file của chương trình chính cần chạy phải trùng với tên thư mục hiện tại (Ví dụ: DisplayImage.c thì thư mục chứa nó cũng phải tên DisplayImage)
MCU = atmega328p
F_CPU = 16000000UL
BAUD = 9600UL
PROGRAMMER = arduino	
COM = COM10	
APP_NAME = $(notdir $(CURDIR))

#============================DIRECTORIES================================#
# Lùi cấp 2 thư mục để trở về thư mục gốc ban đầu
SRC_DIR = ../../src
BUILD_DIR = ../../build
TARGET = $(BUILD_DIR)/$(APP_NAME)

#=============================TOOLCHAIN=================================#
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE = avr-size
AVRDUDE = avrdude

#============================COMPILER FLAG==============================#
MCU_FLAGS = -mmcu=$(MCU)
CFLAGS = -c $(MCU_FLAGS)
CFLAGS += -DF_CPU=$(F_CPU) -DBAUD=$(BAUD)
CFLAGS += -Os -g -std=gnu99 -Wall
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums 
CFLAGS += -I$(SRC_DIR)

LDFLAGS = $(MCU_FLAGS)
LDFLAGS += -Wl,-Map,$(TARGET).map,--gc-sections 

#=============================SOURCE FILE===============================#
# Gom file nguồn: Quét file .c, .h ở thư mục ../../src và thư mục hiện tại
# SRCS lưu trữ toàn bộ file .c, .h cùng đường dẫn
# APP_SRCS: Lưu trữ đường dẫn đến chương trình chính .c hiện tại
# SUPPORT_SRCS: Lưu trữ các file được hỗ trợ
APP_SRCS = $(wildcard *.c)
SUPPORT_SRCS = $(wildcard $(SRC_DIR)/*.c)

#====================================OBJECT FILES====================================================#
# Tạo ra các file object ở thư mục build
APP_OBJS = $(BUILD_DIR)/$(APP_NAME).o
SUPPORT_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/src/%.o,$(SUPPORT_SRCS))
OBJS = $(APP_OBJS) $(SUPPORT_OBJS)

#========================================BUILD TARGETS==============================================#
all: $(BUILD_DIR) $(TARGET).elf $(TARGET).bin $(TARGET).hex size

# Tạo ra thư mục build
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/src/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/$(APP_NAME).o: $(APP_NAME).c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $<

# Liên kết thành file .elf
$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

# Tạo ra file .bin (Mã máy nhị phân thuần túy)
# Cú pháp: objcopy [các_cờ_tùy_chọn] <file_đầu_vào> <file_đầu_ra>
$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@
	
# Tạo ra file .hex (Định dạng intel hex)
$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@
	
# Kích thước bộ nhớ
size: $(TARGET).elf
	@echo "------Size------"
	$(AVRSIZE) $<

# Dọn dẹp
clean:
	rm -rf $(BUILD_DIR)
	
# Nạp code
flash: $(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -P $(COM) -b 115200 -U flash:w:$< 

# Khai báo các mục tiêu ảo (tránh lỗi trùng tên file vật lý)
.PHONY: all clean flash size debug

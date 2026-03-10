# Thư viện SSD1306 128x64 OLED

## Tổng quan về SSD1306 128x64 OLED

- OLED (Organic Light Emitting Diode) phát sáng khi có dòng điện chạy qua. Màn hình OLED hoạt động không cần đèn nền (backlight), vì vậy
có thể hiện thị màu đen rất sâu. Nó có kích thước nhỏ gọn và trọng lượng nhẹ hơn so với màn hình tinh thể lỏng (LCD).
- Màn hình OLED 128x64 là loại hiển thị đồ họa dạng ma trận điểm đơn giản. Nó có 128 cột và 64 hàng, tạo thành tổng cộng 128 × 64 = 8192 điểm ảnh (pixel). 
Bằng cách bật/tắt các LED của từng điểm ảnh, chúng ta có thể hiển thị hình ảnh đồ họa với bất kỳ hình dạng nào.
- Những loại giao tiếp cho OLED có thể kể tới như là I2C, SPI, ... Và trong bài này thì tôi sử dụng OLED dùng giao thức I2C để truyền thông

## Thông số của SSD1306 128x64 OLED

- Loại màn hình: OLED (Organic Light Emitting Diode)
- Kích thước màn hình: 128x64 pixels
- Driver điều khiển: SSD1306
- Màu sắc màn hình: Monochrome (White), Yellow, and Blue
- Điện áp hoạt động: 3.3V to 5V
- Giao tiếp: I2C
- Dòng điện hoạt động: ~20mA

## Sơ đồ chân màn hình OLED (Giao tiếp I2C)

- SDA: Truyền dữ liệu giữa master và slave
- SCL: Tạo ra xung clock đến slave. Chỉ Master mới có thể điều khiển đường SCL
- VCC: Nguồn cấp cho màn hình được yêu cầu là 3.3V. Nếu lớn hơn 3.3V, có thể gây hỏng màn hình
- GND

**Địa chỉ I2C của màn hình OLED**
| Bit|	7	| 	6	|	5	|	4	|	3	|	2	|	1	|	0	|
|:---|:---|:---|:---|:---|:---|:---|:---|:---|
| 	| 0	| 1	| 1 | 1 | 1 | 0 | SA0 bit | R/W bit |

- SA0 bit: Để xác định địa chỉ của Slave
	+ SA0 = 0 -> address = 0b00111100 = 0x3C
	+ SA0 = 1 -> address = 0b00111101 = 0x3D
- R/W (Read/Write) bit: Xác định chế độ hoạt động của Master, trong đó: 1 = Read, 0 = Write


## Cấu trúc màn hình OLED

* Màn hình OLED được ánh xạ với GDDRAM của SSD1306

* GDDRAM (Graphics Display Data RAM) là một loại RAM tĩnh dạng ánh xạ bit (bit-mapped static RAM). Nó lưu trữ mẫu bit (bit pattern) sẽ được hiển thị trên màn hình.
	* Kích thước của GDDRAM là 128x64 bits và chia thành 8 pages (PAGE0 ... PAGE7)
	* Khi một byte dữ liệu được ghi vào GDDRAM, tất cả các hàng pixels của cột hiện tại trong một PAGE cụ thể sẽ được lấp đầy. Với data bit D0 nằm ở top row, D7 nẳm ở bottom row
	
## Thuật toán cho việc in dữ liệu ra màn hình

1. Step 1: Thực hiện tạo ra một mảng bufferRAM[] gồm 1024 phần tử (Bản sao màn hình OLED 128x64). Cứ mỗi 128 phần tử trong mảng thì đại diện cho 1 PAGE.
2. Step 2: Clear buffer trước khi vẽ.
3. Step 3: Vẽ pixel vào buffer thay vì gửi I2C. Thay vì gửi pixel lên OLED ngay, ta làm như sau:
	- Với input (x, y) trong đó: x thuộc [0, 63] và y thuộc [0, 127].
	- Tính toán Page chứa pixel: page = x / 8.
	- Thứ tự bit trong một Byte (của một cột trong một PAGE): bit = y % 8.
	- Ta xác định chỉ số bufferRAM[] ứng với vị trí (x, y): index = (page * 128) + x.
	- Thực hiện vẽ một pixel lên vị trí (x, y): bufferRAM[index] |= (1 << bit). 
	-> Như đã nói, index bản chất đại diện cho 8-bit dữ liệu - 8 pixel trong một cột của một PAGE xác định.
4. Step 4: Vẽ ký tự tại vị trí (x, y) vào buffer. Với mỗi ký tự 5x7 cần 5 cột và 7 hàng.
	- Với từng cột, ta lấy dữ liệu (data) 8-bit của một cột.
	- Xét từng hàng (pixel) của cột bằng cách xét các bit trong data, nếu hàng đó sáng thì ta thực hiện vẽ pixel.
5. Step 5: Vẽ chuỗi string vào buffer. 
6. Step 6: Cập nhật buffer vào OLED.

## Lập trình SSD1306 128x64 OLED với AVR ATmega328P

* Trong phần này, tôi đã triển khai 3 nhóm lệnh:
	* Nhóm lệnh cơ bản (Fundamental Function)
	* Nhóm lệnh in lên màn hình (Display Function)
	* Nhóm lệnh cuộn màn hình (Scroll Function)

**NOTE: Những lệnh sau chỉ áp dụng cho OLED 128x64 và chế độ Page Adressing Mode**

### Lệnh cơ bản

```diff
- OLED_Init(): Lệnh khởi tạo SSD1306 OLED128x64
- OLED_Set_Cursor(uint8_t page, uint8_t col): Di chuyển con trỏ đến một vị trí xác định với 2 tham số page - PAGE0..7, col - column 0...127
- OLED_Set_Brightness(uint16_t brightness): Điều chỉnh độ sáng của OLED
- OLED_Display_Mode(uint8_t mode): Lựa chọn chế độ in trên màn hình OLED với 2 chế độ là 0: TOP xuống BOTTOM, 1: BOTTOM lên TOP
- OLED_Sleep(): Đưa OLED vào chế độ Sleep nhưng mà dữ liệu GDRAM vẫn không bị biến mất
- OLED_Clear_Display(): Xóa toàn bộ nội dung trong GDRAM
- OLED_Draw_Bit_Map(const uint8_t* image): Lệnh này nhận vào một mảng ma trận các pixel rồi in lên OLED
```

**NOTE:** TOP ở đây chính là chữ đánh dấu các chân cắm (VCC, GND, SCK, SDA)

### Lệnh in lên màn hình
```diff
- OLED_Print_Character(char ch, uint8_t x, uint8_t y): Helper của hàm OLED_Print_String()
- OLED_Print_String(const char* str, uint8_t x, uint8_t y): Hàm này đẩy chuỗi ký tự cần in vào mảng bufferRAM[]
- OLED_Update_Display(): Thực hiện đẩy bufferRAM[] vào màn hình GDRAM của OLED bằng cách ánh xạ từng ô nhớ của bufferRAM sang pixel
```

**NOTE:** 
- Với x (Cột) thuộc khoảng [0, 127] và y (Hàng) thuộc [0, 63]
- bufferRAM[]: Mảng buffer được sử dụng để lưu trữ các byte dữ liệu của (page, cột) cụ thể

### Lệnh cuộn màn hình

**NOTE**:
1. Tất cả các lệnh cuộn ở đây đều cố định khoảng thời gian giữa 2 lần cuộn là 5 frames và mặc định là sẽ thực hiện cuộn sang phía phải.
2. Cuộn dọc (scroll vertical) ở trong SSD1306 OLED 128x64 là không thể thực hiện được.

```diff
- OLED_Horizontal_Scroll(uint8_t startPage, uint8_t endPage): Các page thuộc khoảng [startPage, endPage] sẽ được cuộn ngang về phía bên phải
- OLED_Scroll_Area(uint8_t fixedRows, uint8_t scrollingRows): Hàm này mục đích nhiệm vụ là thực hiện cuộn những hàng được phép cuộn và cố định những hàng được chỉ định
	- Example: fixedRows = 3, scrollingRows = 6 -> Hàng 0 - 3 được cố định, hàng 4 - 9 được phép cuộn và hàng 10 - 63 cố định
- OLED_Vertical_And_Horizontal_Scroll(uint8_t startPage, uint8_t endPage, uint8_t verticalOffset): Lệnh này thực hiện đồng thời cuộn ngang và  dịch theo phương dọc hình ảnh, trong đó: verticalOffset là độ dịch của trục dọc mỗi lần cuộn ngang
```

## Đóng góp

Source: Nguyễn Hải Nam

Email: namnguyenhai228@gmail.com
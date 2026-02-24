# DIGITAL_CLOCK_STM32
STM32 Digital Clock
Digital Clock with RTC, Alarm, Countdown Timer

*Giới thiệu: project đồng hồ điện tử đa năng sử dụng vi điều khiển STM32F103C8T6, khai thác RTC nội, hiển thị bằng LED 7 đoạn, điều khiển qua nút nhấn, có các chức năng:
Hiển thị thời gian thực
- Chỉnh giờ / phút
- Báo thức (Alarm)
- Đếm ngược (Countdown)
- Thông báo bằng buzzer
Project được xây dựng theo FSM (Finite State Machine), lập trình bare-metal (CMSIS).

* Phần cứng sử dụng
Vi điều khiển: STM32F103C8T6
2 module LED 7 đoạn 4 số ghép nối tiếp. Sử dụng IC ghi dịch 74HC595 để quét LED 7 đoạn
Nút nhấn (MODE, UP, DOWN, OK, COUNTDOWN)
Buzzer active 3.3V

* Hướng dẫn kết nối phần cứng
  
1️⃣ Khối nút nhấn điều khiển

| Chức năng | Chân STM32 (Blue Pill) | Ghi chú                |
| --------- | ---------------------- | ---------------------- |
| MODE      | PB0                    | Cấu hình Input Pull-up |
| COUNTDOWN | PB1                    | Cấu hình Input Pull-up |
| UP        | PB10                   | Cấu hình Input Pull-up |
| DOWN      | PB11                   | Cấu hình Input Pull-up |
| OK        | PB12                   | Cấu hình Input Pull-up |

Một đầu nút nhấn nối GND, đầu còn lại nối vào chân GPIO.

2️⃣ Buzzer
| Thiết bị | Chân STM32 | Ghi chú                   |
| -------- | ---------- | ------------------------- |
| Buzzer   | PA1        | GPIO Output               |

3️⃣ RTC
| Thành phần | Ghi chú                   |
| ---------- | ------------------------- |
| RTC        | Sử dụng RTC nội của STM32 |

Dùng LSE 32.768kHz 

4️⃣ LED 7 đoạn 4 số (4-Bit LED Digital Module – 2 Module nối tiếp)

| Chân STM32 | Kết nối đến              |
| ---------- | ------------------------ |
| PA5        | SCLK của cả 2 module     |
| PA6        | RCLK của cả 2 module     |
| PA7        | DIO của Module LED thứ 1 |

Kết nối nối tiếp (Daisy Chain)
| Từ            | Nối đến        |
| ------------- | -------------- |
| QH (Module 1) | DIO (Module 2) |

 Cả hai module dùng chung SCLK và RCLK. Dữ liệu truyền nối tiếp từ Module 1 sang Module 2 thông qua chân QH
* Chức năng chính
  1. Hiển thị thời gian HH:MM:SS từ RTC nội và cập nhật liên tục
  
  2. Chỉnh thời gian
Nhấn UP / DOWN để chỉnh giờ
Nhấn OK → chuyển sang chỉnh phút
Thời gian RTC vẫn chạy, không bị dừng
Hiển thị dạng:
--.HH.MM.SS

  3. Báo thức (Alarm)
Nhấn SET ALARM
Hiển thị: SA.HH.MM.SS
Chỉnh giờ → phút → giây
Khi đến thời gian cài đặt:
Buzzer kêu
LED 7 đoạn hiển thị SA.HH.MM.SS
  4. Đếm ngược (Countdown)
Nhấn SET COUNTDOWN
Hiển thị: CD.MM.SS
Chỉ đếm ngược phút và giây
Sau khi xác nhận → bắt đầu đếm ngược
Khi về 0:
Buzzer kêu
Nhấn MODE bất kỳ lúc nào để hủy countdown

* FSM – Finite State Machine
Project được thiết kế theo FSM để:
Dễ quản lý trạng thái
Dễ mở rộng
Tránh lỗi logic khi xử lý nút nhấn
Các trạng thái chính:
STATE_DISPLAY

STATE_SET_TIME

STATE_SET_ALARM

STATE_SET_COUNTDOWN

STATE_CD_RUNNING

STATE_RING

* Timer & kỹ thuật
TIM2 interrupt:
Quét LED 7 đoạn
Tạo tick thời gian mềm
Countdown sử dụng tick tích lũy, không phụ thuộc clock chính xác
Nút nhấn có debounce bằng phần mềm

* Phần mềm & công cụ
STM32CubeIDE
CMSIS 
Ngôn ngữ: C
Debug: ST-Link

*DEMO: https://youtu.be/Bd1gl-mIB_g?si=sSsYV1YKNl9h8DhI

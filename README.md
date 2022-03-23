# README for TV-Q

# DESCRIPTION

This product plays video on a 3.5 inch display. The videos are loaded from an external micro SD card. Storing your original video on micro SD and insert it to this product, you can play the video on this product
This is a product created by [Kaoru Sasahara](https://github.com/ksasahara1) and [Masahiko Tsuyama](https://github.com/MTsuyama-git) at Mar 2022.

# Requirements
- [Raspberry Pi OS Lite 11.2](https://www.raspberrypi.com/software/)
  - [US](https://downloads.raspberrypi.org/raspios_lite_armhf/images/raspios_lite_armhf-2022-01-28/)
  - [JP](http://ftp.jaist.ac.jp/pub/raspberrypi/raspios_lite_armhf/images/raspios_lite_armhf-2022-01-28/)
- [FFmpeg 5.0](https://ffmpeg.org)


# REQUIPMENTS
- Raspberry Pi zero (unmarked/W/WH)
- Elecrow 3.5 Inch 480x320 TFT Display with Touch Screen for Raspberry Pi[wiki](https://www.elecrow.com/wiki/index.php?title=3.5_Inch_480x320_TFT_Display_with_Touch_Screen_for_Raspberry_Pi)
  - Availabe at:
    - [AliExpress](https://a.aliexpress.com/_mKufbde)
    - [Amazon](https://www.amazon.co.jp/dp/B01LXM8NL4/ref=cm_sw_r_tw_dp_3DXJ5ABX25AQSMX2GXAG)

- SD card x2
  - For Raspberry Pi OS
  - For Video Storage
- microSD card reader
  Micro USB Type B to micro SD

- microSD slot extender

- microUSB connector
  [shop](https://akizukidenshi.com/catalog/g/gK-10972/)

# HOW TO SET UP Raspberry Pi
Please refer to ...

# READY TO PLAY
You have 2 options to show the video:
- Use encoded video.
- Use resized video
 
- Video setting (our recommendation)
type: H264(.mp4) or QuickTime(.mov)
size: 240px:160px
*The file system of the video SDcard must be FAT32 or exFAT.

# HOW TO ENCODE
## CUI Program
./bin/video_decoder /path/to/video destination
mv destination /path/to/mnt
## GUI Program
Coming Soon…


# THE CASE
See the stl


# Also you need to use…
- Grub Screws x4 (D:3mm L:10mm) 
- screws x12 (D:2mm D:5mm)
- Clear Acrylic Plate x1 (82mm x 56.5mm)
- Small Drill x1 (to make holes for Acrylic Plate)




# LICENSE
Coming soon.



# COPYRIGHT
Copyright (c) 2022- The TV-Q Project
Copyright (c) 2022 Kaoru Sasahara, Masahiko Tsuyama
All rights reserved.

Thank you for reading.
Donate us or give us a job.




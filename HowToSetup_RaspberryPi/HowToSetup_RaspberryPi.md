# HOW TO SETUP Raspberry Pi

Installing the OS
Choose "Raspberry Pi OS Lite(32 bit)" from Raspberry Pi Imager.



Firstly, setup for Wi-Fi on raspi-config.

# SHOW LCD SCREEN 
<https://raspida.com/waveshare-install-lcd35>

$ sudo apt-get install git libjpeg9 libjpeg9-dev

$ git clone https://github.com/waveshare/LCD-show.git

$ chmod +x ./LCD-show/LCD35-show

$ cd LCD-show

$ ./LCD35-show



# microSD AUTOMATION MOUNT by FSTAB
$ vi /etc/fstab

add this description.

/dev/sda1	/mnt	exfat defaults,ro,nofail	0	0

/dev/sda1	/mnt_vfat	vfat defaults,ro,nofail	0	0


then Raspberry Pi can read the SD card.

# CREATE RUN SCRIPT
$ vi TV-Q.sh
```
#!/bin/bash

while :
do

# for common video files
for i in $(find /mnt /mnt_vfat -maxdepth 1 -name \*.mov -or -name \*.MOV -or -name \*.mp4 -type f); do

ffmpeg -re -i $i -vf "scale=480:320" -pix_fmt bgra -f fbdev /dev/fb0

done

# for tjpeg files
for i in $(find /mnt /mnt_vfat -maxdepth 1 -name \*.tjpeg -type f); do
/home/pi/TV-Q/bin/playJpeg $i
done

done
```

# CRONTAB
$ crontab -e
```
@reboot /bin/bash -l /home/pi/TV-Q.sh 
```
$ sudo reboot


# ADD A PERMISSION
$chmod 755 TV-Q.sh


# READ-ONLY SYSTEM by OVERLAYS
<https://qiita.com/ma2shita/items/45818f0872472ecacac1>

$ sudo raspi-config nonint enable_overlayfs

$ sudo reboot

$ sudo raspi-config nonint get_overlay_now && echo "enabled" || echo "disabled"

If "enabled", the read-only system is working. (disabled means not READ-ONLY.)


 



# Now complete! Have a nice video!


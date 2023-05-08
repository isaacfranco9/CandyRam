NOW=$(date "+%d-%m-%y_%k:%M:%S")
touch $NOW.jpg
libcamera-jpeg -n -o temp.jpg
mv temp.jpg $NOW.jpg
#echo $NOW
/usr/bin/convert /home/ramsandrockets/testing/$NOW.jpg -pointsize 72 -fill black -annotate +50+1900 $(date "+ %D_%T") /home/ramsandrockets/testing/$NOW.jpg

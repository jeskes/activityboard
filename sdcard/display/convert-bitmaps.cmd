@echo off

for /R %%f in (*.png *.jpg *.bmp) do (
    echo converting: %%f
    ffmpeg -v 0 -i "%%f" -f rawvideo -pix_fmt rgb565be "%%~dpnf.raw" -y
)

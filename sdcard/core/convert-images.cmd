@echo off

for /R %%f in (*.png *.jpg *.bmp) do (
    echo converting: %%f
    ffmpeg -v 0 -i "%%f" -f rawvideo -pix_fmt rgb565le -sws_flags full_chroma_int+accurate_rnd+lanczos "%%~dpnf.raw" -y
)

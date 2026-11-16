@echo off

for /R %%f in (*.bmp) do (
    echo converting: %%f
	ffmpeg -v 0 -i "%%f" -vf "scale=320:240" -pix_fmt yuvj420p -q:v 4 -map_metadata -1 "%%~dpnf.jpg" -y
    rem ffmpeg -v 0 -i "%%f" -f rawvideo -pix_fmt rgb565le -sws_flags full_chroma_int+accurate_rnd+lanczos "%%~dpnf.raw" -y
	rem ffmpeg -i input.png -vf "scale=480:320" -pix_fmt yuvj420p -q:v 4 -map_metadata -1 output.jpg
	rem ffmpeg -i input.png -vf "scale=480:320:force_original_aspect_ratio=decrease,pad=480:320:(ow-iw)/2:(oh-ih)/2:black" -pix_fmt yuvj420p -q:v 4 -map_metadata -1 output.jpg
)

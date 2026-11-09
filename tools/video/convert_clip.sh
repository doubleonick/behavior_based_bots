#!/usr/bin/env bash
# convert_clip.sh - turn an iPhone .MOV of Val into a small web-ready MP4.
#
#   tools/video/convert_clip.sh IN.MOV OUT.mp4 [START END] [TURN]
#
#   START END  seconds to keep (e.g. 13.5 18.0); leave both out to keep it all
#   TURN       cw  = a quarter turn clockwise (top of the shot ends up on the right)
#              ccw = a quarter turn anticlockwise
#              none (default)
#
# What it does:
#   - the phone's own rotation is applied automatically
#   - HDR (the iPhone's HLG colour) is converted to ordinary colour, so it doesn't
#     look grey and washed out in browsers
#   - 1280 pixels wide, H.264, no sound, starts playing quickly on the web
#
# Needs ffmpeg with the zscale filter (Ubuntu's ffmpeg package has it).
set -euo pipefail

in="$1"
out="$2"
start="${3:-}"
end="${4:-}"
turn="${5:-none}"

range=()
if [ -n "$start" ] && [ -n "$end" ]; then
  range=(-ss "$start" -to "$end")
fi

tonemap="zscale=t=linear:npl=100,format=gbrpf32le,zscale=p=bt709,tonemap=tonemap=hable:desat=0,zscale=t=bt709:m=bt709:r=tv,format=yuv420p"
case "$turn" in
  cw)   rotate=",transpose=1" ;;
  ccw)  rotate=",transpose=2" ;;
  none) rotate="" ;;
  *)    echo "TURN must be cw, ccw or none" >&2; exit 1 ;;
esac

ffmpeg -hide_banner -loglevel error -y "${range[@]}" -i "$in" -map 0:v:0 -an \
  -vf "${tonemap}${rotate},scale=1280:-2" \
  -c:v libx264 -preset slow -crf 23 -movflags +faststart \
  -color_primaries bt709 -color_trc bt709 -colorspace bt709 "$out"
echo "wrote $out"

#!/bin/bash
#nathan jahnke <njahnke@gmail.com>

#this script helps you work around the fact that the compiler chokes on .qrc files referencing files totaling more than about 1 meg
#paste the output into the RESOURCES += section of your .pro file

cd helpers

if [ -f ffmpeg.exe ] && [ -f mp4box.exe ]; then
	exeext=".exe"
fi

for bin in ffmpeg$exeext mp4box$exeext nnedi3_weights.bin ; do
	rm -f $bin.*

	split -b 1000000 $bin $bin.

	i=1
	for part in `ls $bin.*` ; do
		mv $part $bin.$i

		echo '<RCC><qresource prefix="/"><file>'$bin.$i'</file></qresource></RCC>' > $bin.$i.qrc
		echo "helpers/$bin.$i.qrc " >> qrc_list.in

		i=$[ $i + 1 ]
	done
done

grep -v 'nnedi3_weights' qrc_list.in | tr '\n' ' ' > qrc_list
rm -f qrc_list.in

exit 0

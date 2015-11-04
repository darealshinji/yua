#!/bin/bash
#nathan jahnke <njahnke@gmail.com>

#this script helps you work around the fact that the compiler chokes on .qrc files referencing files totaling more than about 1 meg
#paste the output into the RESOURCES += section of your .pro file

dir=helpers/tmp

cd $dir

for bin in ffmpeg mp4box nnedi3_weights.bin ; do
	rm -f $bin.*
	
	split -b 1000000 $bin $bin.
	
	i=1
	for part in `ls $bin.*` ; do
		mv $part $bin.$i
		
		echo '<RCC><qresource prefix="/"><file>'$bin.$i'</file></qresource></RCC>' > $bin.$i.qrc
		echo "$dir/$bin.$i.qrc \\" >> qrc_list
		
		i=$[ $i + 1 ]
	done
done

exit 0

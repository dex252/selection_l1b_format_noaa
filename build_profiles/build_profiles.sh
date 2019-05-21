#!/bin/bash
# build_profiles.sh

echo "hostname = "`hostname`
echo "whoami = "`whoami`
echo "uname -a = "`uname -a`

SOURCEDIR="$1"
TRACK_N="$2"
#число потоков____________________
PROCESS_NUM=$3
#_________________________________
JSD_DIR_DATA="$HOME/full_System_new/data"
JSD_DIR_EX="$HOME/full_System_new"
MO_DIR_EX="$HOME/IASI_1DVar35"
DEST_DIR="/satellite/product/TH_profiles"
NAME="build_profiles"
BASE_DIR="$HOME/build_profiles"

if [ $# -ne 2 ] ; then
    echo "Syntax: build_profiles.sh <ATOVS files dir>"
exit 1
fi

# Save script directory
#BASE_DIR=$PWD

# Anti re-enter
#a=` ps ax | grep $NAME | wc -l`
#if [ $a -eq 0 ] ; then
    echo "Start processing ..."
#else
#    echo "Script already run!!!"
#    exit 2;
#fi

# Process input directory
#for AMAlst in `ls $SOURCEDIR/*.gz`
for AMAlst in `ls $SOURCEDIR/*`
do
    # Form out file name from input file name
    FN=`basename "$AMAlst"`
    FNAMA=`echo "$FN" | sed s'/.*AMAX.//'`
    if [ "$FN" = "$FNAMA" ] ; then
	continue
    fi;

    # Check file *.HIRX.*
    if [ -f `echo "$SOURCEDIR"/'NSS.HIRX.'"$FNAMA"` ] ; then
	# File *.HIRX.* exist
	# Check file *.MHSX.*/*.AMBX.*
	if [ -f `echo "$SOURCEDIR"/'NSS.MHSX.'"$FNAMA"` -o -f `echo "$SOURCEDIR"/'NSS.AMBX.'"$FNAMA"` ] ; then
	    # Exist all (3) source files, check exist profiles
	    SIDS=${FNAMA:0:2}
	    if [ "$SIDS" = 'NK' ] ; then
		SIDC='15'
	    else if [ "$SIDS" = 'NL' ] ; then
		SIDC='16'
	    else if [ "$SIDS" = 'NM' ] ; then
		SIDC='17'
	    else if [ "$SIDS" = 'NN' ] ; then
		SIDC='18'
	    else if [ "$SIDS" = 'NP' ] ; then
		SIDC='19'
	    fi fi fi fi fi

	    if [ -f `echo "$DEST_DIR"/${SOURCEDIR:(-18)}/${SOURCEDIR:(-10)}'_'${FNAMA:11:4}'_NOAA'"$SIDC"'.joined_satellite_data.profiles.gz'` ] ; then
		echo 'File alredy exist: '"$DEST_DIR"/${SOURCEDIR:(-18)}/${SOURCEDIR:(-10)}'_'${FNAMA:11:4}'_NOAA'"$SIDC"'.joined_satellite_data.profiles.gz'
#	    else
		echo 'Process file: '"$DEST_DIR"/${SOURCEDIR:(-18)}/${SOURCEDIR:(-10)}'_'${FNAMA:11:4}'_NOAA'"$SIDC"'.joined_satellite_data.profiles.gz'
		# Copying and decompressed source files
#		cp "$SOURCEDIR"/'NSS.HIRX.'"$FNAMA" "$JSD_DIR_DATA"
#		gunzip "$JSD_DIR_DATA"/'NSS.HIRX.'"$FNAMA"
#		cp "$SOURCEDIR"/'NSS.AMAX.'"$FNAMA" "$JSD_DIR_DATA"
#		gunzip "$JSD_DIR_DATA"/'NSS.AMAX.'"$FNAMA"
#		if [[ "$SIDS" = "NK" || "$SIDS" = "NL" || "$SIDS" = "NM" ]]; then
#		    cp "$SOURCEDIR"/'NSS.AMBX.'"$FNAMA" "$JSD_DIR_DATA"
#		    gunzip "$JSD_DIR_DATA"/'NSS.AMBX.'"$FNAMA"
#		else
#		    cp "$SOURCEDIR"/'NSS.MHSX.'"$FNAMA" "$JSD_DIR_DATA"
#		    gunzip "$JSD_DIR_DATA"/'NSS.MHSX.'"$FNAMA"
#		fi
	    fi
	else
	    echo 'File '"$SOURCEDIR"/'NSS.AMBX.'"$FNAMA"' or '"$SOURCEDIR"/'NSS.MHSX.'"$FNAMA"' is absent'
	    continue
	fi
    else
	echo 'File '"$SOURCEDIR"/'NSS.HIRX.'"$FNAMA"' is absent'
	continue
    fi
done

# Run processing ATOVS files, generate *.joined_satellite_data
echo "Run processing ATOVS files: cd $JSD_DIR_EX; ./run.sh $JSD_DIR_DATA"
cd "$JSD_DIR_EX"
./run.sh "$JSD_DIR_DATA"

# Run process NOAA[15..19] data
#for n in {1..5}
#do
#    "$BASE_DIR/sat_proc.sh" "NOAA$((n+14))" "$MO_DIR_EX"_"$((n+14))" "$SOURCEDIR" "$DEST_DIR/$TRACK_N/" &
#done
#
#wait

#Распараллеливание___________________________________________________________________________________________________________________________________________________________________________

cd "$JSD_DIR_DATA"

#создаем папку для списка файлов, если она не существует
mkdir -p parallel

max=0

#пробегаем по всем файлам в дериктории с маской '*.*.*' и записываем полные пути файлов в списки с номерами процессов
for i in $(find $JSD_DIR_DATA -depth -name '*.*.*')
do 
max=$(($max+1))
num=$(($max % ${PROCESS_NUM}))
if [ "$num" -eq 0 ]; then 
num=${PROCESS_NUM} 
fi
echo "$i" >> ${JSD_DIR_DATA}/parallel/p_$(($num))
#echo "$max $num $i"
done

#запускаем скрипт на n потоках, {1} - номер потока {2} - дериктория с данными {3} - список файлов для потока {4} - папка IASI__1Dvar35 {5} - треки в iRods
for (( n=1; n <= ${PROCESS_NUM} ; n++ ))
do
#echo "This is: $n"
"$BASE_DIR/sat_proc_21_05_19.sh" "${n}" "$JSD_DIR_DATA" "${JSD_DIR_DATA}/parallel/p_$(($n))" "$MO_DIR_EX" "$DEST_DIR/$TRACK_N/"&
done

wait

#___________________________________________________________________________________________________________________________________________________________________________________________

# enable trigger "create.profile.gml" in dsystem
echo "[value:TCtrack]$DEST_DIR/$TRACK_N"

#rm "$JSD_DIR_DATA"/*

echo "Processing done. Results in:"
echo "$DEST_DIR"/"$TRACK_N"

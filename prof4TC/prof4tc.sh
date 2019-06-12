#!/bin/bash

#	Build profiles for TC in file TC_track.txt
#	Syntax: "./prof4tc.sh TC_track.txt"

SOURCEFILE=$1
PROC_NUM=$2 #число потоков
HOME=$HOME/irus17
IHOME=/satellite/home/slava/irus17
ATOVSPROCESSDIR=${HOME}/full_System_new/data
COORDPATH=${HOME}/full_System_new/coordinates
COORDFILE=${COORDPATH}/MTSATproj.py
BUILDPROF=${HOME}/build_profiles
IATOVSPATH=/satellite/data/ATOVS
RAOBDIR=/home/dima/GETRAOB
RAOBINIFILE=auto_get_raob.ini
IDEST_DIR=/satellite/product/TH_profiles
RADIUS=10.0						# radius of TC region in degree
# for selection ->
TRACKFILE=${HOME}/selection_l1b_format_noaa/TEMP/track
ATOVSPROCESSDIR2=${HOME}/selection_l1b_format_noaa/TEMP/data
CHANGEL1B=${HOME}/selection_l1b_format_noaa/changel1b

# Check arguments
if [ $# -ne 1 ]
then
    echo "Syntax: ./prof4tc.sh <IRODS_path_filename_TC_track.txt>"
    exit 1
fi

cd ${HOME}/prof4TC

#***** iRODS init *****
iinit "$(cat ${HOME}/.irods/.irdpwd)"
iget -fv $SOURCEFILE
iget -fv $SOURCEFILE $TRACKFILE
SOURCEFILE=`basename $SOURCEFILE`
echo Processed $SOURCEFILE
TRACKFILE=${TRACKFILE}/${SOURCEFILE}

TRACK_N=`cat $SOURCEFILE | head -n1`					# 1st string
TRACK_N=`echo -n "$TRACK_N" | sed 's/ /_/g'`
TRACK_BEGIN=`cat $SOURCEFILE | head -n2 | tail -n1`			# 2nd string
TRACK_END=`cat $SOURCEFILE | sed '/^$/d' $SOURCEFILE | tail -n -1`	# last string without empty strings

FILELIST="${TRACK_N}.list"
TRACKLIST=ATOVS_list

BEGIN_DATE_DIR=`echo ${TRACK_BEGIN} | awk '{print $2}' | sed 's/-/\//g'`	# yyyy/mm/dd
BEGIN_TIME_TXT=`echo ${TRACK_BEGIN} | awk '{print $3}'`			# hh:mm:ss
BEGIN_TIME=${BEGIN_TIME_TXT:0:2}${BEGIN_TIME_TXT:3:2}			# hhmm

END_DATE_DIR=`echo ${TRACK_END} | awk '{print $2}' | sed 's/-/\//g'`	# yyyy/mm/dd
END_TIME_TXT=`echo ${TRACK_END} | awk '{print $3}'`			# hh:mm:ss
END_TIME=${END_TIME_TXT:0:2}${END_TIME_TXT:3:2}				# hhmm

BEGIN_DAYS=`date +%j -d$BEGIN_DATE_DIR`
END_DAYS=`date +%j -d$END_DATE_DIR`

# Determinate max and min lat and lon of coordinates of TCtrack
MAX_LON=0.0
MAX_LAT=0.0
MIN_LON=180.0
MIN_LAT=80.0

# Create *.tmp file without first string
cat $SOURCEFILE | sed '/^$/d' $SOURCEFILE | tail -n+2 > ${SOURCEFILE}.tmp
while read line; do
    LON=`echo $line |awk '{print $4}'`
    LAT=`echo $line |awk '{print $5}'`

    if [[ `echo "$LON < $MIN_LON" | bc` == 1 ]] ; then
	MIN_LON="$LON"
    fi
    if [[ `echo "$LAT < $MIN_LAT" | bc` == 1 ]] ; then
	MIN_LAT="$LAT"
    fi
    if [[ `echo "$LON > $MAX_LON" | bc` == 1 ]] ; then
	MAX_LON="$LON"
    fi
    if [[ `echo "$LAT > $MAX_LAT" | bc` == 1 ]] ; then
	MAX_LAT="$LAT"
    fi
done < ${SOURCEFILE}.tmp

# Delete *.tmp file
rm -f ${SOURCEFILE}.tmp

MIN_LON=`echo "$MIN_LON - $RADIUS" | bc`
MIN_LAT=`echo "$MIN_LAT - $RADIUS" | bc`
if [[ `echo "$MIN_LAT < 0.0" | bc` == 1 ]] ; then
    MIN_LAT=0.0
fi
MAX_LON=`echo "$MAX_LON + $RADIUS" | bc`
MAX_LAT=`echo "$MAX_LAT + $RADIUS" | bc`


#	--------------------
#	Begin Day Processing
#	--------------------
echo 'Search ATOVS data files for '$TRACK_N' ...'
echo "ils ${IATOVSPATH}/${BEGIN_DATE_DIR} >${FILELIST}"
ils ${IATOVSPATH}/${BEGIN_DATE_DIR} >${FILELIST}

# Check for exist ATOVS data directory
if ! [ -s ${FILELIST} ]; then
    echo 'ATOVS data collection -C '${IATOVSPATH}/${BEGIN_DATE_DIR}' is not exist'
    echo 'Please, download ATOVS data from NOAA CLASS'
    exit 2
fi

# Check for exist ATOVS data files
if  [ `cat ${FILELIST} | wc -l` -eq "1" ]; then
    echo 'There are no ATOVS data files in -C '${IATOVSPATH}/${BEGIN_DATE_DIR}
    echo 'Please, download ATOVS data from NOAA CLASS'
    exit 3
fi

if [ $BEGIN_DATE_DIR != $END_DATE_DIR ] ; then
    END_T="2400"
else
    END_T=${END_TIME}
fi

cat ${FILELIST} | sed '/^$/d' ${FILELIST} | sed '1d' | sed 's/ //g' | while read A
do
    STIME=${A:20:4}
    ETIME=${A:26:4}

    if [ "$ETIME" -gt "$BEGIN_TIME" ] && [ "$END_T" -gt "$STIME" ]  ; then
	iget -fv ${IATOVSPATH}/${BEGIN_DATE_DIR}/${A} ${ATOVSPROCESSDIR2}
    fi
    if [ "$ETIME" -lt "$BEGIN_TIME" ] && [ "$ETIME" -lt "$STIME" ] && [ "$END_T" -gt "$STIME" ]  ; then
	iget -fv ${IATOVSPATH}/${BEGIN_DATE_DIR}/${A} ${ATOVSPROCESSDIR2}
    fi
done;

DAYS=$[$END_DAYS - $BEGIN_DAYS]

#	---------------------
#	Whole days Processing
#	---------------------
if [ "$DAYS" -gt 1 ] ; then
    for (( I=1; I<$DAYS; I++ ))
    do
	# make date for dir name
	BEGIN_SECONDS=`date +%s -d$BEGIN_DATE_DIR`
	BEGIN_SECONDS=$(( BEGIN_SECONDS + I * 86400 ))
	DATE_DIR=`date +%F -d @$BEGIN_SECONDS | sed 's/-/\//g'`		# yyyy/mm/dd

	ils ${IATOVSPATH}/${DATE_DIR} >${FILELIST}
	# Check for exist ATOVS data directory
	if  ! [ -s ${FILELIST} ]; then
	    echo 'ATOVS data collection -C '${IATOVSPATH}/${DATE_DIR} 'is not exist'
	    echo 'Please, download ATOVS data from NOAA CLASS'
	    exit 2
	fi

	# Check for exist ATOVS data files
	if  [ `cat ${FILELIST} | wc -l` -eq "1" ]; then
	    echo 'There are no ATOVS data files in -C '${IATOVSPATH}/${DATE_DIR}
	    echo 'Please, download ATOVS data from NOAA CLASS'
	    exit 3
	fi

	ils ${IATOVSPATH}/${DATE_DIR} | sed '/^$/d' | sed '1d' | sed 's/^[ ]*//' >${FILELIST}
	while read line; do
	    iget -fv ${IATOVSPATH}/${DATE_DIR}/"$line" ${ATOVSPROCESSDIR2}
	done < ${FILELIST}
    done
fi

#	------------------
#	End Day Processing
#	------------------
if [ "$DAYS" -gt 0 ] ; then
    ils ${IATOVSPATH}/${END_DATE_DIR} >${FILELIST}

    # Check for exist ATOVS data directory
    if  ! [ -s ${FILELIST} ]; then
	echo 'ATOVS data collection -C '${IATOVSPATH}/${END_DATE_DIR} 'is not exist'
        echo 'Please, download ATOVS data from NOAA CLASS'
	exit 2
    fi

    # Check for exist ATOVS data files
    if  [ `cat ${FILELIST} | wc -l` -eq "1" ]; then
	echo 'There are no ATOVS data files in -C '${IATOVSPATH}/${END_DATE_DIR}
        echo 'Please, download ATOVS data from NOAA CLASS'
	exit 3
    fi

    cat ${FILELIST} | sed '/^$/d' ${FILELIST} | sed '1d' | sed 's/ //g' | while read A
    do
#	ETIME=`echo ${A} | tail -c 17 | head -c 4`
#	STIME=`echo ${A} | tail -c 23 | head -c 4`

	STIME=${A:20:4}
	ETIME=${A:26:4}

	if [ "$ETIME" -gt "$END_TIME" ] && [ "$END_TIME" -gt "$STIME" ]  ; then
	    iget -fv ${IATOVSPATH}/${END_DATE_DIR}/${A} ${ATOVSPROCESSDIR2}
	fi
	if [ "$END_TIME" -lt "$ETIME" ] && [ "$END_TIME" -lt "$STIME" ]  ; then
	    iget -fv ${IATOVSPATH}/${END_DATE_DIR}/${A} ${ATOVSPROCESSDIR2}
	fi
	if [ "$STIME" -lt "$END_TIME" ] && [ "$ETIME" -lt "$STIME" ] ; then
	    iget -fv ${IATOVSPATH}/${END_DATE_DIR}/${A} ${ATOVSPROCESSDIR2}
	fi
    done;
fi

rm -f "$FILELIST"

#delete sourcefile
rm -f ${SOURCEFILE}

for fileGZ in `ls $ATOVSPROCESSDIR2/*.gz`
    do
    gunzip $fileGZ
    done;

${CHANGEL1B} $TRACKFILE;

rm -f ${TRACKFILE};

# Make file coordinate.py
echo 'Create coordinate file '${COORDFILE}
echo '#!/usr/bin/python'   > ${COORDFILE}
echo 'minLat = '$MIN_LAT  >> ${COORDFILE}
echo 'maxLat = '$MAX_LAT  >> ${COORDFILE}
echo 'minLon = '$MIN_LON  >> ${COORDFILE}
echo 'maxLon = '$MAX_LON  >> ${COORDFILE}
echo 'numPixelsInLon = 4' >> ${COORDFILE}
echo 'numPixelsInLat = 4' >> ${COORDFILE}


cd ${BUILDPROF}
echo "Starting build profiles: cd ${BUILDPROF}; ./build_profiles.sh ${ATOVSPROCESSDIR} ${TRACK_N}"
./build_profiles.sh ${ATOVSPROCESSDIR} ${TRACK_N} ${PROC_NUM}

# enable trigger "create.profile.gml" in dsystem
echo "result:$IDEST_DIR/$TRACK_N"

# *** Get RAOB data for TC track from dima@icecube.satellite.dvo.ru
cd ${HOME}/prof4TC

# Make ini-file for auto_get_raob.php
echo 'Create '${RAOBINIFILE}' file'
echo '[General]' >${RAOBINIFILE}
echo 'bdate='${BEGIN_DATE_DIR////} >>${RAOBINIFILE}	# yyyymmdd
echo 'edate='${END_DATE_DIR////} >>${RAOBINIFILE}	# yyyymmdd
echo 'blat='$MIN_LAT >>${RAOBINIFILE}
echo 'blon='$MIN_LON >>${RAOBINIFILE}
echo 'elat='$MAX_LAT >>${RAOBINIFILE}
echo 'elon='$MAX_LON >>${RAOBINIFILE}
echo '' >>${RAOBINIFILE}
echo '[Connect]' >>${RAOBINIFILE}
echo 'host=escave.satellite.dvo.ru' >>${RAOBINIFILE}
echo 'port=5432' >>${RAOBINIFILE}
echo 'dbname=raob' >>${RAOBINIFILE}
echo 'user=titan' >>${RAOBINIFILE}
echo 'password=lonewolf' >>${RAOBINIFILE}

# Copy auto_get_rab.ini to dima@icecube.satellite.dvo.ru:~/GETRAOB
scp "${RAOBINIFILE}" dima@icecube.satellite.dvo.ru:${RAOBDIR}
rm -f ${RAOBINIFILE}

# Run php to get RAOB data
ssh dima@icecube.satellite.dvo.ru 'cd '${RAOBDIR}';\
 php -c '${RAOBDIR}' '${RAOBDIR}'/auto_get_raob.php 1>get_raob.out 2>get_raob.err;\
 mv -fv '${RAOBDIR}'/raob_data.txt '${RAOBDIR}/${TRACK_N}'_raob.txt;\
 gzip "'${RAOBDIR}/${TRACK_N}'_raob.txt";\
 /opt/irods/bin/iput -fv "'${RAOBDIR}/${TRACK_N}'_raob.txt.gz" "'${IDEST_DIR}/${TRACK_N}'";\
 rm -f "'${RAOBDIR}/${TRACK_N}'_raob.txt.gz"'

#echo 'Done'

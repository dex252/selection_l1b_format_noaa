#!/bin/bash

RHOME=$HOME/irus17
HOME=$HOME/irus17/Projects/selection_l1b_format_noaa
IATOVSPATH=satellite/data/ATOVS
SOURCEFILE=${HOME}/TEMP/Dates.txt
ATOVSPROCESSDIR=${HOME}/TEMP/data
FILELIST="atovs.list" 


echo 'Search ATOVS data files'
echo ${RHOME}
#***** iRODS init *****
iinit "$(cat ${RHOME}/.irods/.irdpwd)"

while read line; do 
                DATE=`echo $line |awk '{print $1}'`
                echo "ils ${IATOVSPATH}/${DATE} >${FILELIST}"
                ils ${IATOVSPATH}/${DATE} >${FILELIST}
done < ${SOURCEFILE}

# Check for exist ATOVS data directory
if ! [ -s ${FILELIST} ]; then
    echo 'ATOVS data collection -C '${IATOVSPATH}/${DATE}' is not exist'
    echo 'Please, download ATOVS data from NOAA CLASS'
    exit 2
fi

# Check for exist ATOVS data files
if  [ `cat ${FILELIST} | wc -l` -eq "1" ]; then 
    echo 'There are no ATOVS data files in -C '${IATOVSPATH}/${DATE}
    echo 'Please, download ATOVS data from NOAA CLASS'
    exit 3
fi

iget -fv ${IATOVSPATH}/${FILELIST} ${ATOVSPROCESSDIR}

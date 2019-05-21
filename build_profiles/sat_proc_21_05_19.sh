#!/bin/bash
#set_proc_21_05_19.sh

PROCESS_N=$1
SOURCEDIR="$2"
LIST=$3
MO_DIR_EX="$4"
DEST_DIR="$5"

iinit "$(cat ${HOME}/.irods/.irdpwd)"

echo "Process... ${PROCESS_N} in ${LIST}"

while read var; do
#проверка к какому типу спутника относится файл (15-19)
    if [[ $var == *"NK."* ]]; then 
    MO_DIR_EX="${MO_DIR_EX}_15"
    else if [[ $var == *"NL."* ]] ; then
	MO_DIR_EX="${MO_DIR_EX}_16"
    else if [[ $var == *"NM."* ]] ; then
	MO_DIR_EX="${MO_DIR_EX}_17"
    else if [[ $var == *"NN."* ]] ; then
	MO_DIR_EX="${MO_DIR_EX}_18"
    else if [[ $var == *"NP."* ]] ; then
	MO_DIR_EX="${MO_DIR_EX}_19"
    fi fi fi fi fi
#скопировано с предыдущего процесса sat_proc.sh
    echo Process "$var" in "$MO_DIR_EX"
    cp "$var" "${MO_DIR_EX}"
    cd "${MO_DIR_EX}"
    ./mo35prof ${var##*/}
    gzip "${var##*/}.profiles"
    imkdir -p "${DEST_DIR}"
#    cp "${var##*/}.profiles.gz" "${DEST_DIR}"
    iput -fv "${var##*/}.profiles.gz" "${DEST_DIR}"
    gzip "${var##*/}"
#    cp "${var##*/}.gz" "${DEST_DIR}"
    iput -fv "${var##*/}.gz" "${DEST_DIR}"
    rm "${var##*/}.profiles.gz"
    rm "${var##*/}.gz"

done < ${LIST}

echo "End process...${PROCESS_N} "

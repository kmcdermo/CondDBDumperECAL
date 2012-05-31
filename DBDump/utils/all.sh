#!/bin/bash

myself="`readlink -f "$0"`"
mydir="`dirname "$myself"`"

runlist="$mydir/nur.list"
file_prompt="/afs/cern.ch/user/e/ecalmon/www/prompt/prompt.root"
dir_www="/afs/cern.ch/user/e/ecalmon/www/prompt"

[[ -n $1 ]] && {
    file_prompt="$1"
    dir_prompt="gp_`echo $file_prompt | sed -e 's/\.root$//' | sed -e 's@\../@@g' | sed -e 's@\./@@g' | sed -e 's@/@_@g'`"
}
echo "input file: $file_prompt"
echo "output dir: $dir_prompt"

last_run=`tail -1 $runlist | awk '{ print $1}'`
[[ -z $last_run ]] && last_run=185000
echo "last run found: $last_run"
((last_run += 1))

$mydir/rr.py -r $last_run | tac | grep -v "#" >> $runlist
$mydir/boundaries.py $runlist

(
export SCRAM_ARCH="slc5_amd64_gcc462"
dir_cmssw_rel=$mydir
cd $dir_cmssw_rel && eval `scram run -sh` && cd - > /dev/null
#rm -r $dir_prompt
$mydir/dumproot.py $file_prompt
#fs sa -dir $dir_prompt -acl webserver:afs read
)
dir_prompt="gp_`echo $file_prompt | sed -e 's@/@_@g' | sed -e 's/\.root$//'`"

#cp /afs/cern.ch/user/e/ecalmon/www/prompt/index_dev.html /afs/cern.ch/user/e/ecalmon/www/prompt/gp_prompt/index.html
#cp /afs/cern.ch/user/e/ecalmon/www/prompt/style.css /afs/cern.ch/user/e/ecalmon/www/prompt/gp_prompt/style.css

$mydir/validationplots.sh $dir_prompt
mv $dir_prompt/*png $dir_www
rm -r $dir_prompt
echo "making animated gif"
convert -delay 20 -loop 0 $dir_www/history*.png $dir_www/anim.gif
[[ $2"X" == "fehX" ]] && feh $dir_prompt/*png

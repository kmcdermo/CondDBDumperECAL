#!/bin/bash

myself="`readlink -f "$0"`"
mydir="`dirname "$myself"`"

dir=$1
gp_dir="$mydir"
gp_map="$gp_dir/plot_dump_maps.gp"
gp_lm="$gp_dir/plot_LM.gp"
gp_histories="$gp_dir/plot_dump_history_sigma.gp"

gp_slope_histories="$gp_dir/plot_dump_slope_history_sigma.gp"
gp_slope_lm="$gp_dir/plot_slope_LM.gp"

now=`date +"%F %H:%M:%S %Z"`

#~ecalmon/public/bin contains a gnuplot version more recent than lxplus one:
PATH="/afs/cern.ch/user/e/ecalmon/public/bin:$PATH"

[[ "X"$1 == "X" ]] && {
    echo "Usage: $0 <directory>"
    exit 1
}

title()
{
    [ $1 =  1 ] && echo '{/=16 -3.0 < {/Symbol h} < -2.7}'
    [ $1 =  2 ] && echo '{/=16 -2.7 < {/Symbol h} < -2.4}'
    [ $1 =  3 ] && echo '{/=16 -2.4 < {/Symbol h} < -2.1}'
    [ $1 =  4 ] && echo '{/=16 -2.1 < {/Symbol h} < -1.8}'
    [ $1 =  5 ] && echo '{/=16 -1.8 < {/Symbol h} < -1.5}'
    [ $1 =  6 ] && echo '{/=16 -1.5 < {/Symbol h} < -1.2}'
    [ $1 =  7 ] && echo '{/=16 -1.2 < {/Symbol h} < -0.9}'
    [ $1 =  8 ] && echo '{/=16 -0.9 < {/Symbol h} < -0.6}'
    [ $1 =  9 ] && echo '{/=16 -0.6 < {/Symbol h} < -0.3}'
    [ $1 = 10 ] && echo '{/=16 -0.3 < {/Symbol h} < 0}'
    [ $1 = 11 ] && echo '{/=16 0 < {/Symbol h} < 0.3}'
    [ $1 = 12 ] && echo '{/=16 0.3 < {/Symbol h} < 0.6}'
    [ $1 = 13 ] && echo '{/=16 0.6 < {/Symbol h} < 0.9}'
    [ $1 = 14 ] && echo '{/=16 0.9 < {/Symbol h} < 1.2}'
    [ $1 = 15 ] && echo '{/=16 1.2 < {/Symbol h} < 1.5}'
    [ $1 = 16 ] && echo '{/=16 1.5 < {/Symbol h} < 1.8}'
    [ $1 = 17 ] && echo '{/=16 1.8 < {/Symbol h} < 2.1}'
    [ $1 = 18 ] && echo '{/=16 2.1 < {/Symbol h} < 2.4}'
    [ $1 = 19 ] && echo '{/=16 2.4 < {/Symbol h} < 2.7}'
    [ $1 = 20 ] && echo '{/=16 2.7 < {/Symbol h} < 3.0}'
    #echo 'unknown'
}

check_file()
{
    pref="$1"
    [[ ! -a "$dir/${pref}_min_max.dat" ]] && paste "$dir/${pref}_min.dat" "$dir/${pref}_max.dat" | awk '{print $1, $2, $3, $9}' > "$dir/${pref}_min_max.dat"
}

check_file EBh2
check_file EEh2_nZ
check_file EEh2_pZ

echo -n "."
sed -e "s@%DIR%@$dir@g" $gp_map | sed -e "s@#%PNG%@@g" | sed -e "s@%NOW%@$now@g" > tmp_.gp
gnuplot tmp_.gp

echo -n "."
sed -e "s@%DIR%@$dir@g" $gp_lm | sed -e "s@#%PNG%@@g" | sed -e "s@%NOW%@$now@g" > tmp_.gp
gnuplot tmp_.gp

echo -n "."
sed -e "s@%DIR%@$dir@g" $gp_slope_lm | sed -e "s@#%PNG%@@g" | sed -e "s@%NOW%@$now@g" > tmp_.gp
gnuplot tmp_.gp

for i in "All" "EB-" "EB+" "EE-" "EE+";
do
    sed -e "s@%DIR%@$dir@g" $gp_histories | sed -e "s@%REGION%@$i@g" | sed -e "s@#%PNG%@@g" | sed -e "s@%TITLE%@{/=16 $i}@g" | sed -e "s@%NOW%@$now@g" > tmp_.gp
    gnuplot -p tmp_.gp
    echo -n "."
    sed -e "s@%DIR%@$dir@g" $gp_slope_histories | sed -e "s@%REGION%@$i@g" | sed -e "s@#%PNG%@@g" | sed -e "s@%TITLE%@{/=16 $i}@g" | sed -e "s@%NOW%@$now@g" > tmp_.gp
    gnuplot -p tmp_.gp
    echo -n "."
done

for i in `seq 1 92`;
do
    sed -e "s@%DIR%@$dir@g" $gp_histories | sed -e "s@%REGION%@LM`printf %02d $i`@g" | sed -e "s@#%PNG%@@g" | sed -e "s@%TITLE%@{/=16 LM`printf %02d $i`}@g" | sed -e "s@%NOW%@$now@g" > tmp_.gp
    gnuplot -p tmp_.gp
    echo -n "."
    sed -e "s@%DIR%@$dir@g" $gp_slope_histories | sed -e "s@%REGION%@LM`printf %02d $i`@g" | sed -e "s@#%PNG%@@g" | sed -e "s@%TITLE%@{/=16 LM`printf %02d $i`}@g" | sed -e "s@%NOW%@$now@g" > tmp_.gp
    gnuplot -p tmp_.gp
    echo -n "."
done

for i in `seq 1 20`;
do
    sed -e "s@%DIR%@$dir@g" $gp_histories | sed -e "s@%REGION%@eta`printf %02d $i`@g" | sed -e "s@#%PNG%@@g" | sed -e "s@%TITLE%@`title $i`@" | sed -e "s@%NOW%@$now@g" > tmp_.gp
    gnuplot -p tmp_.gp
    echo -n "."
    sed -e "s@%DIR%@$dir@g" $gp_slope_histories | sed -e "s@%REGION%@eta`printf %02d $i`@g" | sed -e "s@#%PNG%@@g" | sed -e "s@%TITLE%@`title $i`@" | sed -e "s@%NOW%@$now@g" > tmp_.gp
    gnuplot -p tmp_.gp
    echo -n "."
done

echo

#! /bin/bash
FILES=./log/*
string=()
dates=()
commands=()
keywords=()
paths=()
for f in $FILES
do
    while read -r LINE
    do
        if [ "$(cut -d':' -f4 <<<"$LINE")" = " wc " ] || [ "$(cut -d':' -f4 <<<"$LINE")" = " maxcount " ] || [ "$(cut -d':' -f4 <<<"$LINE")" = " mincount " ]
        then
            continue
        fi
        string=("${string[@]}" "$LINE")
        dates+=("$(cut -d':' -f1-3 <<<"$LINE")")
        commands+=("$(cut -d':' -f4 <<<"$LINE")")
        keywords+=("$(cut -d':' -f5 <<<"$LINE")")
        paths+=("$(cut -d':' -f6- <<<"$LINE")")
    done < $f
done
difkeywordssearched=()
for ((i=0;i<${#commands[@]}; i++))
do
    flag=1
    for ((j=0;j<${#difkeywordssearched[@]}; j++))
    do
        if [ "${keywords[$i]}" = "${difkeywordssearched[$j]}" ]
        then
            flag=0
        fi
    done
    if [ "$flag" = 1 ]
    then
        difkeywordssearched+=("${keywords[$i]}")
    fi
done
echo "Total number of keywords searched:"${#difkeywordssearched[@]}
minkeyword=""
maxkeyword=""
min=100000000
max=0
for ((i=0;i<${#difkeywordssearched[@]}; i++))
do
    difpaths=()
    for ((j=0;j<${#keywords[@]}; j++))
    do
        if [ "${difkeywordssearched[$i]}" = "${keywords[$j]}" ]
        then
            var=6
            while [ ! -z "$(cut -d':' -f$var <<<"${string[$j]}")" ]
            do
                flag=1
                for ((l=0;l<${#difpaths[@]}; l++))
                do
                    if [ "${difpaths[$l]}" = "$(cut -d':' -f$var <<<"${string[$j]}")" ]
                    then
                        flag=0
                    fi
                done
                if [ "$flag" = 1 ]
                then
                    difpaths+=("$(cut -d':' -f$var <<<"${string[$j]}")")
                fi
                ((var++))
            done
        fi 
    done
    if [ "${#difpaths[@]}" = 0 ]
    then
        continue
    fi
    if [ $min -gt ${#difpaths[@]} ]
    then
        min=${#difpaths[@]}
        minkeyword=${difkeywordssearched[$i]}
    fi
    if [ ${#difpaths[@]} -gt $max ]
    then
        max=${#difpaths[@]}
        maxkeyword=${difkeywordssearched[$i]}
    fi
done
echo "Keyword most frequently found: $maxkeyword [totalNumFilesFound: $max]"
echo "Keyword least frequently found: $minkeyword [totalNumFilesFound: $min]"
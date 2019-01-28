#!/bin/bash

POSITIONAL=()
rootdirectory=""
text_file=""
w=0
p=0
index=0
numoflines=0
while [[ $# -gt 0 ]]    #checking if input is correct
do
    key="$1"
    if [ $index == 0 ]; then
        rootdirectory=$key
        if [ ! -d "$rootdirectory" ]; then
            echo "Directory not found, exiting..."
            exit 1
        fi
        if [ "$(ls -A $rootdirectory)" ]; then
            echo "Warning: $rootdirectory is full, purging"
            rm -r $rootdirectory/*
        fi
    fi
    if [ $index == 1 ]; then
        text_file=$key
        if [ ! -f "$text_file" ]; then
            echo "File not found, exiting..."
            exit 1
        fi
        numoflines=$(wc -l < $text_file)
        if [ 10000 -gt $numoflines ]; then
            echo "$text_file is too small file, exiting..."
            exit 1
        fi
    fi
    if [ $index == 2 ]; then
        w=$key
        if [ $w == 0 ] || [ $w == 1 ]; then
            echo "Very small websites number exiting..."
            exit 1
        fi
    fi
    if [ $index == 3 ]; then
        p=$key
        if [ $p == 0 ];then
            echo "Very small pages number exiting..."
            exit 1
        fi
    fi
    shift
    index=$((index+1))
done
set -- "${POSITIONAL[@]}"
declare -A allpages
numofrows=$w
numofcolumns=$p
for ((i=0;i<numofrows;i++)) do  #initialise the array where all pages number will be located
    for ((j=0;j<numofcolumns;j++)) do
        allpages[$i,$j]=-1
    done
done
for ((i=0;i<numofrows;i++));    #give the pages a unique random number between 0-10000
do
    for ((j=0;j<numofcolumns;j++));
    do
        rand_pagenumber=0
        flag=-1
        while [ $flag == -1 ]; do
            rand_pagenumber=$((RANDOM % 10000))
            for ((l=0;l<numofcolumns;l++)); do
                if [ ${allpages[$i,$l]} == -1 ]; then
                    flag=$l
                    break
                fi
                if [ $rand_pagenumber == ${allpages[$i,$l]} ]; then
                    break
                fi
            done
        done
        allpages[$i,$flag]=$rand_pagenumber
    done
done
declare -A inlinks  
for ((i=0;i<numofrows;i++)) do  #initialise array which indicates if pages have incoming links
    for ((j=0;j<numofcolumns;j++)) do
        inlinks[$i,$j]=-1
    done
done

numberofpages=$((w*p))
for ((i=0;i<numofrows;i++));
do
    echo "Creating web site $i"
    webname=$rootdirectory/"site"$i
    mkdir $webname
    for ((j=0;j<numofcolumns;j++));
    do
        k=$((1+RANDOM%$((numoflines-2000))))    #start
        m=$((1000+RANDOM%1000)) #number of lines
        declare -A randintpages
        f=$(((p/2)+1))
        if [ $f == $p ]; then
            f=$((f-1))
        fi
        for ((index=0;index<f;index++));
        do
            randintpages[$index]=-1     
        done
        for ((index=0;index<f;index++));
        do
            rand_pagenumber=0
            flag=-1
            while [ $flag == -1 ]; do
                rand_pagenumber=$((RANDOM % $p))    #give the pages a random index which indicates a unique page at allpages array
                for ((l=0;l<f;l++)); do
                    if [ $rand_pagenumber == $j ]; then #if it is the same page ignore it and try again
                        break
                    fi
                    if [ ${randintpages[$l]} == -1 ]; then  #if all are ok with the random number place it to table
                        flag=$l
                        break
                    fi
                    if [ $rand_pagenumber == ${randintpages[$l]} ]; then    #if we have chosen again this number try again
                        break
                    fi
                done
            done
            randintpages[$flag]=$rand_pagenumber
            inlinks[$i,$rand_pagenumber]=1      #pass the link to the link array
        done
        declare -A randoutpages
        q=$(((w/2)+1))
        for ((index=0;index<q;index++));
        do
            randoutpages[$index]=-1
        done
        for ((index=0;index<q;index++));
        do
            rand_pagenumber=0
            flag=-1
            while [ $flag == -1 ]; do     
                rand_pagenumber=$((RANDOM % $numberofpages ))
                for ((l=0;l<q;l++)); do
                    if [ $rand_pagenumber -ge $((i*p)) ] && [ $rand_pagenumber -lt $(((i+1)*p)) ]; then #make sure it is from another website
                        break
                    fi
                    if [ ${randoutpages[$l]} == -1 ]; then
                        flag=$l
                        break
                    fi
                    if [ $rand_pagenumber == ${randoutpages[$l]} ]; then
                        break
                    fi
                done
            done
            randoutpages[$flag]=$rand_pagenumber
            x=$((rand_pagenumber / numofcolumns))               #x and y identify the location of the page at the inlink array
            y=$((rand_pagenumber % numofcolumns))
            inlink[$x,$y]=1
        done
        pagename=$webname/"page"$i"_"${allpages[$i,$j]}.html
        echo "Creating page $pagename with $m lines starting at line $k"    #intialise page
        touch $pagename
        echo "<!DOCTYPE html>" >> $pagename
        echo -e "\t<body>" >> $pagename
        index=0
        counter=0
        totallinks=$((f+q))
        while read line; do
            if [ $index -ge $k ]; then
                if [ $index -gt $((k+m)) ]; then
                    break
                fi
                echo $line"<br>" >> $pagename   #pass a regural line to the page
                if [ $((counter%(m/(f+q)))) == 0 ] && [ $counter != 0 ]; then
                    flag=0
                    linkname=""
                    ran=$((RANDOM%totallinks))
                    websitenum=-1
                    pagenum=-1
                    if [ $ran -lt $f ]; then                #decide if a inside or outside link will be used and construct it
                        if [ ${randintpages[$ran]} != -1 ]; then
                            websitenum=$i
                            pagenum=${allpages[$websitenum,${randintpages[$ran]}]}
                            randintpages[$ran]=-1
                            flag=1
                        fi
                    else 
                        if [ ${randoutpages[$((ran-f))]} != -1 ]; then
                            websitenum=$((randoutpages[$((ran-f))]/p))
                            jindex=$((randoutpages[$((ran-f))]%p))
                            pagenum=${allpages[$websitenum,$jindex]}
                            randoutpages[$((ran-f))]=-1
                            flag=1
                        fi
                    fi
                    if [ $flag == 0 ]; then
                        for ((c=0;c<totallinks;c++));
                        do
                            if [ $c -lt $f ]; then
                                if [ ${randintpages[$c]} != -1 ]; then
                                    websitenum=$i
                                    pagenum=${allpages[$websitenum,${randintpages[$c]}]}
                                    randintpages[$c]=-1
                                    break
                                fi
                            else 
                                if [ ${randoutpages[$((c-f))]} != -1 ]; then
                                    websitenum=$((randoutpages[$((c-f))]/p))
                                    jindex=$((randoutpages[$((c-f))]%p))
                                    pagenum=${allpages[$websitenum,$jindex]}
                                    randoutpages[$((c-f))]=-1
                                    break
                                fi
                            fi 
                        done
                    fi
                    linkname="/site"$websitenum/        #pass the link to the page
                    linkname=$linkname"page"$websitenum"_"$pagenum".html"
                    printname=$rootdirectory/"site"$websitenum/"page"$websitenum"_"$pagenum".html"
                    echo "Addint link to "$printname
                    echo -e "<a href=\"$linkname\">$linkname</a><br>" >> $pagename
                fi
                counter=$((counter+1))
            fi
            index=$((index+1))
        done < text_file.txt
        echo -e "\t</body>" >> $pagename        #the final touches to finish the creation of the page
        echo "</html>" >> $pagename
    done
done
flag=0
for ((i=0;i<numofrows;i++)) do          #check if all pages have incoming links and print the result
    for ((j=0;j<numofcolumns;j++)) do
        if [ ${inlinks[$i,$j]} == -1 ]; then
            flag=1
            break
        fi
    done
done

if [ $flag == 1 ]; then
    echo "Some pages do not have inlink"
fi
if [ $flag == 0 ]; then
    echo "All pages have a inlink"
fi
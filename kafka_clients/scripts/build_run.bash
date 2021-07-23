#!/bin/bash
usage()
{
    echo "##################################################################################"
    echo "   ./build.bash  -b <build folder> -t <test executable> -e  <main executable>    "
    echo "##################################################################################"
}

usage

while getopts t:e:b: flag
do
    case "${flag}" in
        b)
            build_dir=${OPTARG}
           
        ;;
        t) 
           test_file=${OPTARG} 
        ;;
        e) 
           run_file=${OPTARG}
        ;;
    esac
done

if [ ${#build_dir} -gt 0 ]; then
    cd $build_dir 
    make
fi

if [ ${#test_file} -gt 0 ]; then
    sudo chmod 777 $test_file
    $test_file
fi

if [ ${#run_file} -gt 0 ]; then
    sudo chmod 777 $run_file
    $run_file
fi


 

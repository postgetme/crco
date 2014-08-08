#!/bin/bash

WIDGET_ROOT_DIR=widget
WIDGETS=(menu time list desktop yesno password progress screen_protect messagebox)

rm -rf $WIDGET_ROOT_DIR
mkdir $WIDGET_ROOT_DIR
cd $WIDGET_ROOT_DIR

for ((i = 0; i < ${#WIDGETS[*]}; i++))
do
    widget=${WIDGETS[i]}
    mkdir $widget
    cd $widget
    touch ${widget}.h
    touch ${widget}.c
    
    Widget=`echo ${widget} | awk '{print toupper($0)}'`
    
    echo "#ifndef WIDGET_${Widget}_H_H" >> ${widget}.h
    echo "#define WIDGET_${Widget}_H_H" >> ${widget}.h
    echo >> ${widget}.h
    echo "extern ">> ${widget}.h
    echo "extern ">> ${widget}.h
    echo "extern ">> ${widget}.h
    echo >> ${widget}.h
    echo "#endif" >> ${widget}.h
    echo >> ${widget}.h
    cd ..
done
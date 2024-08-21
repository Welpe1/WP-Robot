#!/bin/bash  
  
echo "filename"  
read file  
  
if [ ! -f "$file" ]; then  
    echo "ERR:$file don't exist!"  
    exit 1  
fi  
  
scp "$file" root@192.168.42.2:/root

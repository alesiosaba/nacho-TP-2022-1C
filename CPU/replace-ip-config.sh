#!/bin/bash

echo "\n\t\t¡Dale que se aprueba SO!\n"

read -p "Ingrese nueva IP_ESCUCHA: " IP_ESCUCHA
read -p "Ingrese nueva IP_MEMORIA: " IP_MEMORIA

grep -RiIl 'IP_ESCUCHA' | xargs sed -i 's|\(IP_ESCUCHA\s*=\).*|\1'$IP_ESCUCHA'|'
grep -RiIl 'IP_MEMORIA' | xargs sed -i 's|\(IP_MEMORIA\s*=\).*|\1'$IP_MEMORIA'|'

echo "\nLos .config han sido modificados correctamente\n"
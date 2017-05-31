#!/bin/bash
#if number of argumens eq 1
if [ $# -gt 0 ]; then
    case "$1" in
        #status returns if interfaces are up / down
        status )
            if [ $# -gt 1 ]; then
                echo "Status of selected interface(s)"
                shift #replace position of first argument
                    while [ ${#} -gt 0 ]
                        do
                          echo $1 : $(cat /sys/class/net/$1/operstate)
                          shift #again replace position of argument
                        done
            else
                echo "Status of all interfaces"
                for iface in $(ls /sys/class/net); do
                    echo $iface : $(cat /sys/class/net/$iface/operstate)
                done
            fi ;;

        list )

            echo "List of all interfaces"
            echo $(ls /sys/class/net) ;;

        addr_hw )
            if [ $# -gt 1 ]; then
            echo "MAC adresses of selected interface(s)"
            shift
            while [ ${#} -gt 0 ]
                        do
                          echo $1 : $(cat /sys/class/net/$1/address)
                          shift #again replace position of argument
                        done

            else

            echo "Interface physical addresses"
            for iface in $(ls /sys/class/net); do
                echo $iface : $(cat /sys/class/net/$iface/address)
            done
            fi;;
        addr_ip )


            if [ $# -gt 1 ]; then
            echo "IP addresses of selected all interfaces"
            shift
            while [ ${#} -gt 0 ]
                        do
                          echo $1 : $(ifconfig $1| grep 'inet')
                          shift #again replace position of argument
                        done

            else

            echo "IP addresses of all interfaces"

            for iface in $(ls /sys/class/net); do
                echo $iface : $(ifconfig $iface| grep 'inet')
            done

            fi;;
        * )
            echo "No such command defined in bash script";;



    esac

fi
#! /bin/bash
VARIANT_NUMBER="334886"
PORTS_OF_PROC=$(sudo lsof -i -a -p `pgrep ${VARIANT_NUMBER} ` -e /run/user/1000/gvfs | grep -oP '(?:\*:)[0-9]*' | grep -o '[0-9]*' | tr '\n' " ") 
PORTS_OF_PROC=${PORTS_OF_PROC::-1}
CPU_ITER=1
CPU_TIME_ITER=0
IO_ITER=2
IO_TIME_ITER=0
NET_ITER=3
NET_TIME_ITER=0
THREADS_ITER=21
SLP_TIME=1

if pgrep -x ${VARIANT_NUMBER} > /dev/null
then
	echo $PORTS_OF_PROC
	PORTS_FOR_DUMP=$(sed 's/ / or port /g' <<< $PORTS_OF_PROC)
	if [ -z "$PORTS_OF_PROC" ]
	then
		sudo tcpdump -i any -X -c10 -q
	else
		sudo tcpdump -i any port $PORTS_FOR_DUMP -X -c10 -q
	fi



	# CPU usage
	val=$(( $SLP_TIME * $CPU_ITER)) 
	echo "CPU USAGE TEST STARTED!  IT WILL END IN $val SEC"  
	echo "time,cpu,mem" > cpu.csv;
	for i in `seq 1 $CPU_ITER`
	do 
		echo $(echo "$CPU_TIME_ITER";
		top -b -n 1 |
		grep 334886 | 
		grep -oP '(?:\s)[0-9]*\.[0-9]*'   
		) | tr ' ' ',' >>  cpu.csv; 
		sleep $SLP_TIME;
		((CPU_TIME_ITER = CPU_TIME_ITER + 1))
	done

	# IO usage
	val=$(( $SLP_TIME * $IO_ITER)) 
	echo "IO USAGE TEST STARTED!  IT WILL END IN $val SEC"  
		sudo pidstat -d -p `pgrep 334886` 1 $IO_ITER > temp.csv
		sed -i '1,3 d' temp.csv
		sed -i '$ d' temp.csv
		echo "date,disk_rd,disk_wr"> io.csv; 
		awk -v time_iter="$IO_TIME_ITER" -v OFS=, '{print time_iter++ , $5, $6}' temp.csv >> io.csv
		rm temp.csv
		((IO_TIME_ITER = IO_TIME_ITER + 1))


	# NET usage
	val=$(( $SLP_TIME * $NET_ITER)) 
	echo "NET USAGE TEST STARTED!  IT WILL END IN $val SEC"  

	echo "time,RX,TX" > net.csv
	timeout $NET_ITER  bmon -p lo -o ascii -b > in.txt
	awk 'NR % 2 == 0' in.txt > out.txt
	rm in.txt
	awk  '{ system(" echo $(python converter.py "$2") >> in.txt "); }' out.txt 
	awk -v time_iter="$NET_TIME_ITER" -v OFS=, '{print time_iter++,$0}' in.txt >> net.csv
	rm in.txt
	rm out.txt


	# THREADS usage
	val=$(( $SLP_TIME * $THREADS_ITER)) 
	echo "THREADS USAGE TEST STARTED!  IT WILL END IN $val SEC"  

	rm proc.csv
	ps -T -p `pgrep 334886` S | awk '{print $2}' >> proc.csv
	for ((i=0; i< $THREADS_ITER; i++)); 
	do 
		ps -T -p `pgrep 334886` S | awk '{print  $4}'  >> in.csv
		python proc.py
		rm in.csv
		sleep 0.5 
	done
else
	echo "Please check that your program running!"

fi

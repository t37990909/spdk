#!/usr/bin/env bash

NVMF_PORT=7174
NVMF_IP_PREFIX="192.168.100."
NVMF_IP_LEAST_ADDR=8
NVMF_FIRST_TARGET_IP=$NVMF_IP_PREFIX$NVMF_IP_LEAST_ADDR

nvmf_nic_bdfs=""

function load_ib_rdma_modules()
{
	if [ `uname` != Linux ]; then
		exit 0
	fi

	modprobe ib_addr ib_mad ib_sa || true # part of core since 4.7
	modprobe ib_cm
	modprobe ib_core
	modprobe ib_ucm
	modprobe ib_umad
	modprobe ib_uverbs
	modprobe iw_cm
	modprobe rdma_cm
	modprobe rdma_ucm
}

function detect_mellanox_nics()
{
	nvmf_nic_bdfs=`lspci | grep Ethernet | grep Mellanox | awk -F ' ' '{print "0000:"$1}'`
	mlx_core_driver="mlx4_core"
	mlx_ib_driver="mlx4_ib"
	mlx_en_driver="mlx4_en"

	if [ -z "$nvmf_nic_bdfs" ]; then
		exit 0
	fi

	# for nvmf target loopback test, suppose we only have one type of card.
	for nvmf_nic_bdf in $nvmf_nic_bdfs
	do
		result=`find /sys -name $nvmf_nic_bdf | grep driver | awk -F / '{ print $6 }'`
		if [ "$result" == "mlx5_core" ]; then
			mlx_core_driver="mlx5_core"
			mlx_ib_driver="mlx5_ib"
			mlx_en_driver=""
		fi
		break;
	done

	# Uninstall/install driver to make a clean test environment
	if lsmod | grep -q $mlx_ib_driver; then
		rmmod $mlx_ib_driver
	fi

	if [ -n "$mlx_en_driver" ]; then
		if lsmod | grep -q $mlx_en_driver; then
			rmmod $mlx_en_driver
		fi
	fi

	if lsmod | grep -q $mlx_core_driver; then
		rmmod $mlx_core_driver
	fi

	modprobe $mlx_core_driver
	modprobe $mlx_ib_driver
	if [ -n "$mlx_en_driver" ]; then
		modprobe $mlx_en_driver
	fi

	trap - SIGINT SIGTERM EXIT
}

function detect_rdma_nics()
{
	# could be add other nics, so wrap it
	detect_mellanox_nics
}

function allocate_nic_ips()
{
	LEAST_ADDR=$NVMF_IP_LEAST_ADDR
	for bdf in $1; do
		dir=`find /sys -name $bdf | grep "/sys/devices"`
		if [ -e $dir ]; then
			if [ -e $dir"/net" ]; then
				nic_name=`ls $dir"/net"`
				# just get the first nic
				nic_name=`echo $nic_name | awk '{ print $1; }'`
				ifconfig $nic_name $NVMF_IP_PREFIX$LEAST_ADDR netmask 255.255.255.0 up
				LEAST_ADDR=$[$LEAST_ADDR + 1]
			fi
		fi
	done
	# check whether the IP is configured
	result=`ifconfig | grep $NVMF_IP_PREFIX`
	if [ -z "$result" ]; then
		echo "no NIC for nvmf test"
		exit 0
	fi
}

function nvmfcleanup()
{
	rmmod nvme-rdma
	rmmod nvme || true
}

function rdma_device_init()
{
	load_ib_rdma_modules
	detect_rdma_nics
	allocate_nic_ips $nvmf_nic_bdfs
}


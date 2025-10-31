#!/usr/bin/env bash

# Copyright (C) 2025 Red Hat, Inc. All rights reserved.
#
# This copyrighted material is made available to anyone wishing to use,
# modify, copy, or redistribute it subject to the terms and conditions
# of the GNU General Public License v.2.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

# Check pvmove behaviour when its device are kept open


. lib/inittest --skip-with-lvmlockd

_create_lv()
{
	lvremove -f $vg
	lvcreate -Zn -L8 -n $lv1 $vg "$dev1"
	rm -f debug.log_DEBUG*
}

_keep_open()
{
	export LVM_TEST_TAG="kill_me_$PREFIX"
	local d="$DM_DEV_DIR/mapper/$vg-$1"

	for i in {30..0}; do
		test "$i" -eq 0 && die "Failed to wait and open: $d!"
		# try to keep device open for a while
		exec 2>/dev/null 3<"$d" && break
		sleep .1 || return 0
	done

	echo "Keeping open: $d."
	sleep ${2-10} || true
	exec 3<&-
}

_check_msg()
{
	# skipping this check for test with lvmpolld
	# we would need to check 'lvmpolld' messages to get this message
	test -e LOCAL_LVMPOLLD && return

	grep "$@"
}

aux target_at_least dm-mirror 1 2 0 || skip

aux prepare_vg 3

aux delay_dev "$dev3" 0 2 "$(get first_extent_sector "$dev3"):"

# do not waste 'testing' time on 'retry deactivation' loops
aux lvmconf 'activation/retry_deactivation = 0' \
	    'activation/raid_region_size = 16'

# fallback to mirror throttling
# this does not work too well with fast CPUs
test -f HAVE_DM_DELAY || { aux throttle_dm_mirror || skip ; }

########################################################
# pvmove operation finishes, while 1 mirror leg is open
########################################################

_create_lv
_keep_open pvmove0_mimage_0 &

# pvmove fails in such case
not pvmove -i0 --atomic "$dev1" "$dev3" -vvvv |& tee out

aux kill_tagged_processes
wait

_check_msg "ABORTING: Failed" out

lvs -ao+seg_pe_ranges $vg
# but LVs were already moved
check lv_on $vg $lv1 "$dev3"
lvs -a $vg

# orphan LV should be visible with error segment and removable
check lv_field $vg/pvmove0_mimage_0 layout "error"
check lv_field $vg/pvmove0_mimage_0 role "public"
lvremove -f $vg/pvmove0_mimage_0


##########################################
# abort pvmove while 1 mirror leg is open
##########################################

_create_lv
_keep_open pvmove0_mimage_1 &

# with background mode - it's forking polling
pvmove -b -i1 --atomic -vvvv "$dev1" "$dev3"
aux wait_pvmove_lv_ready "$vg-pvmove0"

not pvmove -i0 --abort -vvvv |& tee out

aux kill_tagged_processes
wait
# FIXME: here we are waiting to let the 'original'
# 'pvmove -b' to catch the knowledge about aborted pvmove
# So 'pvmove --abort' itself does NOT abort potentially number
# of monitoring processes
# Temporarily resolve the issue with following sleep
# that last longer then  1s interval used with '-b'
sleep 1.5
not pgrep -u root -lx lvm || {
	ps aux
	die "Some 'lvm' process keeps running!"
	# Bad luck, if admin runs parallel lvm2 command with our testsuite
}

_check_msg "ABORTING: Failed" out

# hopefully we managed to abort before pvmove finished
check lv_on $vg $lv1 "$dev1"

check lv_field $vg/pvmove0_mimage_1 layout "error"
check lv_field $vg/pvmove0_mimage_1 role "public"
lvremove -f $vg/pvmove0_mimage_1


#############################################
# keep pvmove0 open while it tries to finish
#############################################

_create_lv
_keep_open pvmove0 &

not pvmove -i0 --atomic "$dev1" "$dev3" |& tee out

aux kill_tagged_processes
wait

_check_msg "ABORTING: Unable to deactivate" out

check lv_field $vg/pvmove0 layout "error"
check lv_field $vg/pvmove0 role "public"
lvremove -f $vg/pvmove0


################################################
# keep all pvmove volumes open
################################################

_create_lv
_keep_open pvmove0_mimage_0 &
_keep_open pvmove0_mimage_1 &
_keep_open pvmove0 &

not pvmove -i0 --atomic -vvvv "$dev1" "$dev3" |& tee out

aux kill_tagged_processes
wait

_check_msg "ABORTING: Unable to deactivate" out

lvremove -f $vg/pvmove0_mimage_0
lvremove -f $vg/pvmove0_mimage_1
lvremove -f $vg/pvmove0


# Restore throttling
aux restore_dm_mirror

vgremove -ff $vg

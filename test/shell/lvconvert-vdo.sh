#!/usr/bin/env bash

# Copyright (C) 2018 Red Hat, Inc. All rights reserved.
#
# This copyrighted material is made available to anyone wishing to use,
# modify, copy, or redistribute it subject to the terms and conditions
# of the GNU General Public License v.2.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


. lib/inittest --skip-with-lvmpolld --skip-with-lvmlockd

#
# Main
#
aux prepare_vg 2 6400

# Conversion to vdo-pool
lvcreate -L5G -n $lv1 $vg

if not aux have_vdo 6 2 0 ; then
# For unsupported VDO let's check lvconvert fails
	not lvconvert --yes --type vdo-pool $vg/$lv1 |& tee out
	vgremove -ff $vg
	exit
fi

if aux have_thin 1 0 0 && aux have_cache 1 3 0 ; then

# Prepare some lvs for testing conversion
lvcreate --vdo -V5G -L5G -n vdo $vg/vpool

lvcreate -T -V20 -L10 -n thin $vg/tpool

lvcreate --type cache-pool -L10 $vg/cpool

lvcreate -L10 -n $lv2 $vg
lvcreate -s -L10 -n snap $vg/$lv2

#
# Unsupported conversion:
#  thin-pool, thin metadata
#  cache-pool, cache-pool data, cache-pool metadata
#  vdo, vdo-pool, vdo-pool vdata
#
for i in $vg/tpool $vg/tpool_tmeta $vg/thin \
	$vg/cpool $vg/cpool_cmeta $vg/cpool_cdata \
	$vg/lvol0_pmspare \
	$vg/vpool $vg/vpool_vdata $vg/vdo \
	$vg/snap
do
	not lvconvert -t --type vdo-pool $i 2>out
	grep "not permitted" out
	not lvconvert -t --vdopool $i 2>out
	grep "not permitted" out
done

fi
lvremove -f $vg


lvcreate -L5G -n $lv1 $vg

# Check there is big prompting warning
not lvconvert --type vdo-pool $vg/$lv1 |& tee out
grep "WARNING" out

# Check --vdosettings is also applied to converted vdo-pool
lvconvert -y --type vdo-pool --vdosettings 'ack_threads=5' $vg/$lv1
check lv_field $vg/$lv1 vdo_ack_threads "5"
lvremove -f $vg

#
lvcreate -L5G -n $lv1 $vg
lvconvert -y --vdopool $vg/$lv1
lvremove -f $vg


lvcreate -L5G -n $lv1 $vg
lvconvert -y --vdopool $vg/$lv1 -n $lv2
check lv_field $vg/$lv1 segtype vdo-pool
check lv_field $vg/${lv1}_vdata segtype linear -a
check lv_field $vg/$lv2 segtype vdo
lvremove -f $vg


lvcreate -L5G -n $lv1 $vg
lvconvert -y --type vdo-pool $vg/$lv1 -n $lv2 -V10G
lvremove -f $vg


lvcreate -L5G -n $lv1 $vg
lvconvert -y --vdopool $vg/$lv1 -n $lv2 -V10G --compression n --deduplication n
check lv_field $vg/$lv1 size "5.00g"
check lv_field $vg/${lv1}_vdata size "5.00g" -a
check lv_field $vg/$lv2 size "10.00g"
lvremove -f $vg

vgremove -ff $vg

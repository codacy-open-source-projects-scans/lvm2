/*
 * Copyright (C) 2014 Red Hat, Inc. All rights reserved.
 *
 * This file is part of LVM2.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU Lesser General Public License v.2.1.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 * This file defines reserved names for field values.
 *
 * This is used for registering reserved names with reporting code that
 * uses the exact value defined whenever the reserved name is hit, for
 * example during selection criteria processing.
 *
 * TYPE_RESERVED_VALUE defines reserved value that is not bound to any field,
 * but rather it's bound to a certain type. This can be used as a reserved
 * value for all fields of that type then. When naming type reserved value,
 * please follow this naming scheme:
 *   <report type name in lowercase>_<reserved_value_name>
 *
 * FIELD_RESERVED_VALUE defines reserved value bound to a single field.
 * When naming reserved value for the field, please follow this naming scheme:
 *   <field_name>_<reserved_value_name>
 *
 * FIELD_BINARY_RESERVED_VALUE is similar to FIELD_RESERVED_VALUE but it
 * is specifically designed for definition of reserved names for fields
 * with binary values where the reserved names given denote value 1.
 * The first reserved_name given is also used for reporting,
 * others are synonyms which are recognized in addition.
 *
 */

/*
 * TYPE_RESERVED_VALUE(type, flags, reserved_value_id, description, value, reserved_name, ...)
 * FIELD_RESERVED_VALUE(field_id, flags, reserved_value_id, description, value, reserved_name, ...)
 * FIELD_BINARY_RESERVED_VALUE(field_id, reserved_value_id, description, reserved_name for 1, ...)
 */

/* *INDENT-OFF* */

/*
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * IMPORTANT NOTE ABOUT ADDING A NEW VALUE FOR REPORTING
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 * When adding a new string value to report, try to keep it
 * self-descriptive so when it's printed even without the header,
 * we can still deduce what it is actually reporting.
 *
 * If you need more than one descriptive string to mean the same value,
 * please define them as reserved values in values.h.
 *
 * The first reserved value is the one that is printed in reports (unless
 * it's a binary value and we have report/binary_values_as_numeric=1 config
 * option used OR --binary command line option is used OR we're using an
 * output format which must always print binary values in numeric way,
 * like json_std output format.
 *
 * All the other (2nd and further) listed reserved names are synonyms which
 * may be also used in selection (-S|--select).
 *
 * Also, always use proper *_disp functions to display each type of value
 * properly. For example, in case of binary values, you should use
 * _binary_disp so that we can always switch between numerical (0/1/-1) and
 * string representation while reporting the value.
 */

/* Per-type reserved values usable for all fields of certain type. */
TYPE_RESERVED_VALUE(NUM, NOFLAG, num_undef_64, "Reserved value for undefined numeric value.", UINT64_C(-1), "-1", "unknown", "undefined", "undef")

/* Reserved values for PV fields */
FIELD_RESERVED_BINARY_VALUE(pv_allocatable, pv_allocatable, "", "allocatable")
FIELD_RESERVED_BINARY_VALUE(pv_exported, pv_exported, "", "exported")
FIELD_RESERVED_BINARY_VALUE(pv_missing, pv_missing, "", "missing")
FIELD_RESERVED_BINARY_VALUE(pv_in_use, pv_in_use, "", "used", "in use")
FIELD_RESERVED_BINARY_VALUE(pv_duplicate, pv_duplicate, "", "duplicate")

/* Reserved values for VG fields */
FIELD_RESERVED_BINARY_VALUE(vg_extendable, vg_extendable, "", "extendable")
FIELD_RESERVED_BINARY_VALUE(vg_exported, vg_exported, "", "exported")
FIELD_RESERVED_BINARY_VALUE(vg_partial, vg_partial, "", "partial")
FIELD_RESERVED_BINARY_VALUE(vg_clustered, vg_clustered, "", "clustered")
FIELD_RESERVED_BINARY_VALUE(vg_shared, vg_shared, "", "shared")
FIELD_RESERVED_VALUE(NAMED, vg_permissions, vg_permissions_rw, "", "writeable", "writeable", "rw", "read-write")
FIELD_RESERVED_VALUE(NAMED, vg_permissions, vg_permissions_r, "", "read-only", "read-only", "r", "ro")
FIELD_RESERVED_VALUE(NOFLAG, vg_mda_copies, vg_mda_copies_unmanaged, "", &GET_TYPE_RESERVED_VALUE(num_undef_64), "unmanaged")

/* Reserved values for LV fields */
FIELD_RESERVED_BINARY_VALUE(lv_initial_image_sync, lv_initial_image_sync, "", "initial image sync", "sync")
FIELD_RESERVED_BINARY_VALUE(lv_image_synced, lv_image_synced, "", "image synced", "synced")
FIELD_RESERVED_BINARY_VALUE(lv_merging, lv_merging, "", "merging")
FIELD_RESERVED_BINARY_VALUE(lv_converting, lv_converting, "", "converting")
FIELD_RESERVED_BINARY_VALUE(lv_allocation_locked, lv_allocation_locked, "", "allocation locked", "locked")
FIELD_RESERVED_BINARY_VALUE(lv_fixed_minor, lv_fixed_minor, "", "fixed minor", "fixed")
FIELD_RESERVED_BINARY_VALUE(lv_active, lv_active, "", "active")
FIELD_RESERVED_BINARY_VALUE(lv_active_locally, lv_active_locally, "", "active locally", "active", "locally")
FIELD_RESERVED_BINARY_VALUE(lv_active_remotely, lv_active_remotely, "", "active remotely", "active", "remotely")
FIELD_RESERVED_BINARY_VALUE(lv_active_exclusively, lv_active_exclusively, "", "active exclusively", "active", "exclusively")
FIELD_RESERVED_BINARY_VALUE(lv_merge_failed, lv_merge_failed, "", "merge failed", "failed")
FIELD_RESERVED_BINARY_VALUE(lv_snapshot_invalid, lv_snapshot_invalid, "", "snapshot invalid", "invalid")
FIELD_RESERVED_BINARY_VALUE(lv_suspended, lv_suspended, "", "suspended")
FIELD_RESERVED_BINARY_VALUE(lv_live_table, lv_live_table, "", "live table present", "live table", "live")
FIELD_RESERVED_BINARY_VALUE(lv_inactive_table, lv_inactive_table, "", "inactive table present", "inactive table", "inactive")
FIELD_RESERVED_BINARY_VALUE(lv_device_open, lv_device_open, "", "open")
FIELD_RESERVED_BINARY_VALUE(lv_skip_activation, lv_skip_activation, "", "skip activation", "skip")
FIELD_RESERVED_BINARY_VALUE(zero, zero, "", "zero")
FIELD_RESERVED_BINARY_VALUE(lv_check_needed, lv_check_needed, "", "check needed", "needed")
FIELD_RESERVED_BINARY_VALUE(vdo_compression, vdo_compression, "", "enabled" )
FIELD_RESERVED_BINARY_VALUE(vdo_deduplication, vdo_deduplication, "", "enabled" )
FIELD_RESERVED_BINARY_VALUE(vdo_use_metadata_hints, vdo_use_metadata_hints, "", "enabled" )
FIELD_RESERVED_BINARY_VALUE(vdo_use_sparse_index, vdo_use_sparse_index, "", "enabled" )
FIELD_RESERVED_VALUE(NAMED, lv_permissions, lv_permissions_rw, "", "writeable", "writeable", "rw", "read-write")
FIELD_RESERVED_VALUE(NAMED, lv_permissions, lv_permissions_r, "", "read-only", "read-only", "r", "ro")
FIELD_RESERVED_VALUE(NAMED, lv_permissions, lv_permissions_r_override, "", "read-only-override", "read-only-override", "ro-override", "r-override", "R")
FIELD_RESERVED_VALUE(NOFLAG, lv_read_ahead, lv_read_ahead_auto, "", &_siz_max, "auto")
FIELD_RESERVED_VALUE(NAMED, lv_when_full, lv_when_full_error, "", "error", "error", "error when full", "error if no space")
FIELD_RESERVED_VALUE(NAMED, lv_when_full, lv_when_full_queue, "", "queue", "queue", "queue when full", "queue if no space")
FIELD_RESERVED_VALUE(NOFLAG, lv_when_full, lv_when_full_undef, "", "", "", "undefined")
FIELD_RESERVED_VALUE(NAMED | RANGE | FUZZY | DYNAMIC, lv_time, lv_time_fuzzy, "", _lv_time_handler, NULL)
FIELD_RESERVED_VALUE(NAMED | RANGE | FUZZY | DYNAMIC, lv_time_removed, lv_time_removed_fuzzy, "", _lv_time_handler, NULL)

/* Reserved values for SEG fields */
FIELD_RESERVED_VALUE(NOFLAG, cache_policy, cache_policy_undef, "", "", "", "undefined")
FIELD_RESERVED_VALUE(NOFLAG, seg_monitor, seg_monitor_undef, "", "", "", "undefined")
FIELD_RESERVED_VALUE(NOFLAG, lv_health_status, health_undef, "", "", "", "undefined")
FIELD_RESERVED_VALUE(NOFLAG, kernel_discards, seg_kernel_discards_undef, "", "", "", "undefined")
FIELD_RESERVED_VALUE(NOFLAG, vdo_write_policy, vdo_write_policy_undef, "", "", "", "undefined")
/* TODO the following 2 need STR_LIST support for reserved values
FIELD_RESERVED_VALUE(cache_settings, cache_settings_default, "", "default", "default")
FIELD_RESERVED_VALUE(cache_settings, cache_settings_undef, "", "undefined", "undefined") */

/* *INDENT-ON* */

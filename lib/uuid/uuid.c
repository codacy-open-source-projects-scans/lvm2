/*
 * Copyright (C) 2001-2004 Sistina Software, Inc. All rights reserved.
 * Copyright (C) 2004-2006 Red Hat, Inc. All rights reserved.
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

#include "lib/misc/lib.h"
#include "lib/uuid/uuid.h"
#include "lib/misc/lvm-wrappers.h"

#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>

static const char _c[] =
    "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!#";

static int _built_inverse;
static char _inverse_c[256];

int lvid_create(union lvid *lvid, struct id *vgid)
{
	memcpy(lvid->id, vgid, sizeof(*lvid->id));
	return id_create(&lvid->id[1]);
}

int lvid_in_restricted_range(union lvid *lvid)
{
	int i;

	for (i = 0; i < ID_LEN - 3; i++)
		if (lvid->id[1].uuid[i] != '0')
			return 0;

	for (i = ID_LEN - 3; i < ID_LEN; i++)
		if (!isdigit(lvid->id[1].uuid[i]))
			return 0;

	return 1;
}


int id_create(struct id *id)
{
	unsigned i;
	size_t len = sizeof(id->uuid);

	memset(id->uuid, 0, len);
	if (!read_urandom(&id->uuid, len)) {
		return 0;
	}

	/*
	 * Skip out the last 2 chars in randomized creation for LVM1
	 * backwards compatibility.
	 */
	for (i = 0; i < len; i++)
		id->uuid[i] = _c[id->uuid[i] % (sizeof(_c) - 3)];

	return 1;
}

/*
 * The only validity check we have is that
 * the uuid just contains characters from
 * '_c'.  A checksum would have been nice :(
 */
static void _build_inverse(void)
{
	const char *ptr;

	if (_built_inverse)
		return;

	_built_inverse = 1;
	memset(_inverse_c, 0, sizeof(_inverse_c));

	for (ptr = _c; *ptr; ptr++)
		_inverse_c[(int) *ptr] = (char) 0x1;
}

static int _id_valid(struct id *id, int e)
{
	int i;

	_build_inverse();

	for (i = 0; i < ID_LEN; i++)
		if (!_inverse_c[id->uuid[i]]) {
			if (e)
				log_error("UUID contains invalid character '%c'", id->uuid[i]);
			return 0;
		}

	return 1;
}

int id_valid(struct id *id)
{
	return _id_valid(id, 1);
}


int id_equal(const struct id *lhs, const struct id *rhs)
{
	return !memcmp(lhs->uuid, rhs->uuid, sizeof(lhs->uuid));
}

#define GROUPS (ID_LEN / 4)

int id_write_format(const struct id *id, char *buffer, size_t size)
{
	int i, tot;

	static const unsigned _group_size[] = { 6, 4, 4, 4, 4, 4, 6 };

	assert(ID_LEN == 32);

	if (id->uuid[0] == '#') {
		dm_strncpy(buffer, (char*)id->uuid, size);
		return 1;
	}

	/* split into groups separated by dashes */
	if (size < (32 + 6 + 1)) {
		if (size > 0)
			buffer[0] = '\0';
		log_error("Couldn't write uuid, buffer too small.");
		return 0;
	}

	for (i = 0, tot = 0; i < 7; i++) {
		memcpy(buffer, id->uuid + tot, _group_size[i]);
		buffer += _group_size[i];
		tot += _group_size[i];
		*buffer++ = '-';
	}

	*--buffer = '\0';
	return 1;
}

static int _id_read_format(struct id *id, const char *buffer, int e)
{
	int out = 0;

	/* just strip out any dashes */
	while (*buffer) {

		if (*buffer == '-') {
			buffer++;
			continue;
		}

		if (out >= ID_LEN) {
			if (e)
				log_error("Too many characters to be uuid.");
			return 0;
		}

		id->uuid[out++] = *buffer++;
	}

	if (out != ID_LEN) {
		if (e)
			log_error("Couldn't read uuid: incorrect number of "
				  "characters.");
		return 0;
	}

	return _id_valid(id, e);
}

int id_read_format(struct id *id, const char *buffer)
{
	return _id_read_format(id, buffer, 1);
}

int id_read_format_try(struct id *id, const char *buffer)
{
	return _id_read_format(id, buffer, 0);
}

char *id_format_and_copy(struct dm_pool *mem, const struct id *id)
{
	char *repstr = NULL;

	if (!(repstr = dm_pool_alloc(mem, 40))) {
		log_error("dm_pool_alloc failed");
		return NULL;
	}

	if (!id_write_format(id, repstr, 40))
		return_NULL;

	return repstr;
}

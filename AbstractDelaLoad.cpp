 /*
  * Unitex
  *
  * Copyright (C) 2001-2009 Universit� Paris-Est Marne-la-Vall�e <unitex@univ-mlv.fr>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 2.1 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  *
  * You should have received a copy of the GNU Lesser General Public
  * License along with this library; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
  *
  */

#include "Unicode.h"
#include "DELA.h"
#include "AbstractDelaLoad.h"


struct INF_codes* load_abstract_INF_file(char* name,struct INF_free_info* p_inf_free_info)
{
	struct INF_codes* res = NULL;
	res = load_INF_file(name);
	if (res != NULL)
		p_inf_free_info->must_be_free = 1;
	return res;
}

void free_abstract_INF(struct INF_codes* INF,struct INF_free_info* p_inf_free_info)
{
	if (INF != NULL)
		if (p_inf_free_info->must_be_free != 0)
			free_INF_codes(INF);
}

unsigned char* load_abstract_BIN_file(char* name,struct BIN_free_info* p_bin_free_info)
{
	unsigned char* tab = NULL;
	tab = load_BIN_file(name);
	if (tab != NULL)
		p_bin_free_info->must_be_free = 1;
	return tab;
}

void free_abstract_BIN(unsigned char* BIN,struct BIN_free_info* p_bin_free_info)
{
	if (BIN != NULL)
		if (p_bin_free_info->must_be_free != 0)
			free(BIN);
}

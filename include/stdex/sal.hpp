﻿/*
	SPDX-License-Identifier: MIT
	Copyright © 2022-2023 Amebis
*/

#pragma once

#ifdef _WIN32
#include <sal.h>
#endif

#ifndef _In_
#define _In_
#endif
#ifndef _In_bytecount_
#define _In_bytecount_(p)
#endif
#ifndef _In_count_
#define _In_count_(p)
#endif
#ifndef _In_opt_
#define _In_opt_
#endif
#ifndef _In_opt_count_
#define _In_opt_count_(p)
#endif
#ifndef _In_opt_z_count_
#define _In_opt_z_count_(p)
#endif
#ifndef _In_z_
#define _In_z_
#endif
#ifndef _In_z_count_
#define _In_z_count_(p)
#endif
#ifndef _In_reads_or_z_
#define _In_reads_or_z_(p)
#endif
#ifndef _In_reads_or_z_opt_
#define _In_reads_or_z_opt_(p)
#endif

#ifndef _Inout_
#define _Inout_
#endif

#ifndef _Use_decl_annotations_
#define _Use_decl_annotations_
#endif

#ifndef _Out_
#define _Out_
#endif
#ifndef _Out_opt_
#define _Out_opt_
#endif
#ifndef _Out_writes_z_
#define _Out_writes_z_(p)
#endif

#ifndef _Success_
#define _Success_(p)
#endif

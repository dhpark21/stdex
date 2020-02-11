/*
    Copyright 2016-2018 Amebis

    This file is part of stdex.

    stdex is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    stdex is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with stdex. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <assert.h>
#include <Windows.h>
#include <vector>


///
/// Classes without virtual table
///
#define STDEX_NOVTABLE __declspec(novtable)

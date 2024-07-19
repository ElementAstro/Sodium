/*
 * stepguiders.hpp
 *
 * Copyright (C) 2023-2024 Max Qian <lightapt.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#if defined(GUIDE_INDI)
#define STEPGUIDER_SXAO_INDI
#define STEPGUIDER_SBIGAO_INDI
#endif

#ifndef STEPGUIDERS_H_INCLUDED
#define STEPGUIDERS_H_INCLUDED

#define STEPGUIDER_SIMULATOR

#include "stepguider.hpp"
#include "stepguider_sbigao_indi.hpp"
#include "stepguider_sxao_indi.hpp"


#endif /* STEPGUIDERS_H_INCLUDED */

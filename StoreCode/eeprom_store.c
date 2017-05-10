/*
Created on: May 9, 2017
Copyright (C) Mohammad Mazarei Zeus @ Sisoog.com
Email : mohammad.mazarei@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <eeprom_store.h>
#include <util/delay.h>

uint32_t EEMEM g_u32eeprom[EE_Total_Rec];
void EE_Format(void)
{
	uint32_t Temp = 0xFFFFFFFF;
	for(int i=0;i<EE_Total_Rec;i++)
	{
		eeprom_write_dword(&g_u32eeprom[i],Temp);
		_delay_ms(1);
	}
}

int8_t EE_FindFreeIndex(void)
{
	for(int i=0;i<EE_Total_Rec;i++)
	{
		uint32_t Temp = eeprom_read_dword(&g_u32eeprom[i]);
		if(Temp==0xFFFFFFFF)
			return i;
	}
	return -1;
}


void EE_StoreCode(uint32_t Code)
{
	int8_t Index = EE_FindFreeIndex();
	if(Index!=-1)
	{
		eeprom_write_dword(&g_u32eeprom[Index],Code);
	}
}

bool EE_ISValidCode(uint32_t Code)
{
	for(int i=0;i<EE_Total_Rec;i++)
	{
		uint32_t Temp = eeprom_read_dword(&g_u32eeprom[i]);
		if(Temp==Code)
			return true;
	}
	return false;
}

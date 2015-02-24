/** @file

  Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

  **/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/ArmLib.h>
#include <Library/PrePiLib.h>
#include <Library/PcdLib.h>

// DDR attributes
#define DDR_ATTRIBUTES_CACHED                ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK
#define DDR_ATTRIBUTES_UNCACHED              ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED

VOID
InitCache(
	IN  UINT32  MemoryBase,
	IN  UINT32  MemoryLength
)
{
	UINT32                        CacheAttributes;
	ARM_MEMORY_REGION_DESCRIPTOR  MemoryTable[32];
	VOID                          *TranslationTableBase;
	UINTN                         TranslationTableSize;
	UINTN                         Index = 0;

	if (FeaturePcdGet(PcdCacheEnable) == TRUE) 
	{
		CacheAttributes = DDR_ATTRIBUTES_CACHED;
	}
	else 
	{
		CacheAttributes = DDR_ATTRIBUTES_UNCACHED;
	}

	//All memory segment must align to 1MB(0x00100000)

	//MSM_IOMAP
	MemoryTable[Index].PhysicalBase   = 0x00100000;
	MemoryTable[Index].VirtualBase    = 0x00100000;
	MemoryTable[Index].Length         = 0x27f00000;
	MemoryTable[Index].Attributes     = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

	//LPASS LPASS_CSR_BASE
	MemoryTable[++Index].PhysicalBase = 0x28000000;
	MemoryTable[Index].VirtualBase    = 0x28000000;
	MemoryTable[Index].Length         = 0x00B00000;
	MemoryTable[Index].Attributes     = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

	//IMEM //MUST NX?
	MemoryTable[++Index].PhysicalBase = 0x2A000000;   //0x2A03F000 align to 1MB is 0x2A000000
	MemoryTable[Index].VirtualBase    = 0x2A000000;   //0x2A03F000 align to 1MB is 0x2A000000
	MemoryTable[Index].Length         = 0x00100000;   //0x00001000 true size
	MemoryTable[Index].Attributes     = ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED;

	//0x2E000000 ~ 0x2A030000              SBL2
	MemoryTable[++Index].PhysicalBase = 0x2e000000;
	MemoryTable[Index].VirtualBase    = 0x2e000000;
	MemoryTable[Index].Length         = 0x00100000;   //0x00001000
	MemoryTable[Index].Attributes     = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

	// SMEM
	MemoryTable[++Index].PhysicalBase = 0x80000000;
	MemoryTable[Index].VirtualBase    = 0x80000000;
	MemoryTable[Index].Length         = 0x00200000;
	MemoryTable[Index].Attributes     = (ARM_MEMORY_REGION_ATTRIBUTES)CacheAttributes;

	// ATAGE
	MemoryTable[++Index].PhysicalBase = 0x80200000;   //in order to align with 1MB
	MemoryTable[Index].VirtualBase    = 0x80200000;
	MemoryTable[Index].Length         = 0x00100000;
	MemoryTable[Index].Attributes     = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

	// PcdEmbeddedFdBaseAddress + PcdEmbeddedFdSize   ( PcdFlashFvMainBase + PcdFlashFvMainSize)( 0x00200000)
	MemoryTable[++Index].PhysicalBase = 0x80300000;
	MemoryTable[Index].VirtualBase    = 0x80300000;
	MemoryTable[Index].Length         = 0x00200000;
	MemoryTable[Index].Attributes     = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

	// Free
	MemoryTable[++Index].PhysicalBase = 0x80500000;
	MemoryTable[Index].VirtualBase    = 0x80500000;
	MemoryTable[Index].Length         = 0x01000000;
	MemoryTable[Index].Attributes     = (ARM_MEMORY_REGION_ATTRIBUTES)CacheAttributes;

	// PcdCpuVectorBaseAddress
	MemoryTable[++Index].PhysicalBase = 0x81500000;
	MemoryTable[Index].VirtualBase    = 0x81500000;
	MemoryTable[Index].Length         = 0x00100000;
	MemoryTable[Index].Attributes     = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;

	// PcdPrePiStackBase + PcdPrePiStackSize
	MemoryTable[++Index].PhysicalBase = 0x81600000;
	MemoryTable[Index].VirtualBase    = 0x81600000;
	MemoryTable[Index].Length         = 0x00100000;
	MemoryTable[Index].Attributes     = (ARM_MEMORY_REGION_ATTRIBUTES)CacheAttributes;

	// uefi memory (hob base)
	MemoryTable[++Index].PhysicalBase = 0x81700000;
	MemoryTable[Index].VirtualBase    = 0x81700000;
	MemoryTable[Index].Length         = 0x02F00000;
	MemoryTable[Index].Attributes     = (ARM_MEMORY_REGION_ATTRIBUTES)CacheAttributes;

	//Free 
	MemoryTable[++Index].PhysicalBase = 0x84600000;
	MemoryTable[Index].VirtualBase    = 0x84600000;
	MemoryTable[Index].Length         = 0x04900000;
	MemoryTable[Index].Attributes     = (ARM_MEMORY_REGION_ATTRIBUTES)CacheAttributes;

	// LK
	MemoryTable[++Index].PhysicalBase = 0x88f00000;
	MemoryTable[Index].VirtualBase    = 0x88f00000;
	MemoryTable[Index].Length         = 0x00100000;
	MemoryTable[Index].Attributes     = ARM_MEMORY_REGION_ATTRIBUTE_WRITE_THROUGH;

	//FB 
	MemoryTable[++Index].PhysicalBase = 0x89000000;
	MemoryTable[Index].VirtualBase    = 0x89000000;
	MemoryTable[Index].Length         = 0x00300000;
	MemoryTable[Index].Attributes     = ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;
	
	//0x8ff00000 ~ 0x90000000 0x00100000   SBL3
	
	//Free 
	MemoryTable[++Index].PhysicalBase = 0x89300000;
	MemoryTable[Index].VirtualBase    = 0x89300000;
	MemoryTable[Index].Length         = MemoryBase + MemoryLength - 0x89300000;
	MemoryTable[Index].Attributes     = (ARM_MEMORY_REGION_ATTRIBUTES)CacheAttributes;

	// End of Table
	MemoryTable[++Index].PhysicalBase = 0;
	MemoryTable[Index].VirtualBase    = 0;
	MemoryTable[Index].Length         = 0;
	MemoryTable[Index].Attributes     = (ARM_MEMORY_REGION_ATTRIBUTES)0;


	ArmConfigureMmu(MemoryTable, &TranslationTableBase, &TranslationTableSize);

	BuildMemoryAllocationHob((EFI_PHYSICAL_ADDRESS)(UINTN)TranslationTableBase, TranslationTableSize, EfiBootServicesData);
}

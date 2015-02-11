/** @file
  C Entry point for the SEC. First C code after the reset vector.

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
#include <Library/PrePiLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/ArmLib.h>
#include <Library/PeCoffGetEntryPointLib.h>
#include <Library/DebugAgentLib.h>

#include <Ppi/GuidedSectionExtraction.h>
#include <Guid/LzmaDecompress.h>
#include "LzmaDecompress.h"


extern VOID TimerInit(VOID);
extern void msm_clocks_init(VOID);
extern RETURN_STATUS EFIAPI SerialPortInitialize(VOID);

VOID PadConfiguration(VOID);
VOID InitCache(IN  UINT32  MemoryBase, IN  UINT32  MemoryLength);

EFI_STATUS EFIAPI ExtractGuidedSectionLibConstructor(VOID);
EFI_STATUS EFIAPI LzmaDecompressLibConstructor(VOID);


VOID ClockInit(VOID)
{
	msm_clocks_init();
}

//VOID TimerInit(VOID)
//{
//
//}


VOID
CEntryPoint(
	IN  VOID  *StackBase,
	IN  UINTN StackSize
)
{
	VOID * HobBase;
	VOID * MemoryBase;
	UINTN  MemorySize;

	// TODO:
	// ConfigPMU();

	// TODO:
	// Set up Pin muxing.
	PadConfiguration();

	// Set up system clocking,now only uart.
	ClockInit();

	// 
	// Init Uart
	SerialPortInitialize();

	DEBUG((EFI_D_ERROR, "UEFI:Sec:CEntryPoint Uart Enabled\n"));

	//SMEM Init
	//the VALUE should come from SMEM,but we now set fixed value for dev ;)

	MemoryBase = (VOID*)FixedPcdGet32(PcdMemoryBase); //0x80000000
	MemorySize = FixedPcdGet32(PcdMemorySize);        //0x40000000

	//MemoryBase = (VOID*)0x80000000; //0x80000000
	//MemorySize = 0x40000000;        //0x40000000
	
	//FBPT need PMU,before BuildFBPT,we need config PMU.
	//BuildFBPT(MemoryBase);

	// Enable program flow prediction, if supported.
	ArmEnableBranchPrediction();

	/*
	Memory Layout

	0x80200000 ~ 0x80300000 Linux Atags(from lk)
	0x80300000 ~ 0x80500000 Our .FD file(lk load .FD file to this address)
	0x81500000 ~ 0x81500020 PcdCpuVectorBaseAddress + 32
	0x81600000 ~ 0x81640000 PcdPrePiStackBase + PcdPrePiStackSize
	0x81700000 ~ 0x84600000 MemoryBegin(HobBase)
	0x845c0000 ~ 0x84600000 StackBase + size


	0x88f00000 ~ 0x******** lk
	0x89000000 ~ 0x******** FrameBuffer

	*/

	HobBase = (VOID *)((UINT32)MemoryBase + 0x01700000); // = 0x81700000


	//MemorySize
	CreateHobList(HobBase, 0x02F00000, HobBase, (VOID *)((UINT32)HobBase + 0x02F00000 - StackSize));

	DEBUG((EFI_D_ERROR, "UEFI:Sec:CEntryPoint:CreateHobList ok!\n"));



#define attr0x3c07  EFI_RESOURCE_ATTRIBUTE_PRESENT |\
					EFI_RESOURCE_ATTRIBUTE_INITIALIZED              |\
					EFI_RESOURCE_ATTRIBUTE_TESTED                   |\
					EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE              |\
					EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE        |\
					EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE  |\
					EFI_RESOURCE_ATTRIBUTE_WRITE_BACK_CACHEABLE     

	// Initialize CPU cache
	InitCache ((UINT32)MemoryBase, (UINT32)MemorySize);


	BuildResourceDescriptorHob(EFI_RESOURCE_MEMORY_MAPPED_IO, EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE, 0x00100000LL, 0x1D100000LL);
	BuildResourceDescriptorHob(EFI_RESOURCE_MEMORY_MAPPED_IO, EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE, 0x28000000LL, 0x00B00000LL);
	BuildResourceDescriptorHob(EFI_RESOURCE_MEMORY_MAPPED_IO, EFI_RESOURCE_ATTRIBUTE_INITIALIZED, 0x2A03F000LL, 0x00001000LL);

	// SMEM
	BuildResourceDescriptorHob(EFI_RESOURCE_SYSTEM_MEMORY, EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE, 0x80000000, 0x00200000);
	BuildMemoryAllocationHob((EFI_PHYSICAL_ADDRESS)(UINTN)0x80000000, 0x00200000, EfiBootServicesData);

	// Linux Atags(from lk)
	BuildResourceDescriptorHob(EFI_RESOURCE_SYSTEM_MEMORY, EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE, 0x80200000, 0x00100000);
	BuildMemoryAllocationHob((EFI_PHYSICAL_ADDRESS)(UINTN)0x80200000, 0x00100000, EfiBootServicesData);

	// .FD 
	BuildResourceDescriptorHob(EFI_RESOURCE_SYSTEM_MEMORY, EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE, 0x80300000, 0x00200000);
	//BuildMemoryAllocationHob((EFI_PHYSICAL_ADDRESS)(UINTN)0x80300000, 0x00200000, EfiBootServicesData);
	{
		// Add memory allocation hob for relocated FD
		BuildMemoryAllocationHob(FixedPcdGet32(PcdEmbeddedFdBaseAddress), FixedPcdGet32(PcdEmbeddedFdSize), EfiBootServicesData);

		// Add the FVs to the hob list
		BuildFvHob(PcdGet32(PcdFlashFvMainBase), PcdGet32(PcdFlashFvMainSize));

	}

	// Free
	BuildResourceDescriptorHob(EFI_RESOURCE_SYSTEM_MEMORY, attr0x3c07, 0x80500000, 0x01000000);
	//BuildMemoryAllocationHob((EFI_PHYSICAL_ADDRESS)(UINTN)0x80500000, 0x01000000, EfiBootServicesData);

	// PcdCpuVectorBaseAddress
	BuildResourceDescriptorHob(EFI_RESOURCE_SYSTEM_MEMORY, EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE, 0x81500000, 0x00100000);
	BuildMemoryAllocationHob((EFI_PHYSICAL_ADDRESS)(UINTN)0x81500000, 0x00100000, EfiBootServicesData);

	// PrePiStack
	BuildResourceDescriptorHob(EFI_RESOURCE_SYSTEM_MEMORY, attr0x3c07, 0x81600000, 0x00040000);
	BuildMemoryAllocationHob((EFI_PHYSICAL_ADDRESS)(UINTN)0x81600000, 0x00040000, EfiBootServicesData);

	// Free
	BuildResourceDescriptorHob(EFI_RESOURCE_SYSTEM_MEMORY, attr0x3c07, 0x81640000, 0x000c0000);
	//BuildMemoryAllocationHob((EFI_PHYSICAL_ADDRESS)(UINTN)0x81640000, 0x000c0000, EfiBootServicesData);

	// HobBase
	//BuildResourceDescriptorHob(EFI_RESOURCE_SYSTEM_MEMORY, attr0x3c07, 0x81700000, 0x02F00000);
	//BuildMemoryAllocationHob((EFI_PHYSICAL_ADDRESS)(UINTN)0x81700000, 0x02F00000, EfiBootServicesData);

	// Free
	BuildResourceDescriptorHob(EFI_RESOURCE_SYSTEM_MEMORY, attr0x3c07, 0x84600000, 0x04900000);
	//BuildMemoryAllocationHob((EFI_PHYSICAL_ADDRESS)(UINTN)0x84600000, 0x04900000, EfiBootServicesData);

	// LK
	BuildResourceDescriptorHob(EFI_RESOURCE_SYSTEM_MEMORY, EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE, 0x88f00000, 0x00100000);
	BuildMemoryAllocationHob((EFI_PHYSICAL_ADDRESS)(UINTN)0x88f00000, 0x00100000, EfiBootServicesData);

	// Free
	BuildResourceDescriptorHob(EFI_RESOURCE_SYSTEM_MEMORY, attr0x3c07, 0x89000000, (UINT32)MemoryBase + (UINT32)MemorySize - 0x89000000);
	//BuildMemoryAllocationHob((EFI_PHYSICAL_ADDRESS)(UINTN)0x89000000, (UINT32)MemoryBase + (UINT32)MemorySize - 0x89000000, EfiBootServicesData);

	// Give the DXE Core access to our DEBUG and ASSERT infrastructure so this will work prior
	// to the DXE version being loaded. Thus we close the debugging gap between phases.
	AddDxeCoreReportStatusCodeCallback ();

	// Start up a free running timer so that the timer lib will work
	//TimerInit ();

	// SEC phase needs to run library constructors by hand.
	ExtractGuidedSectionLibConstructor();
	LzmaDecompressLibConstructor();

	// Build HOBs to pass up our version of stuff the DXE Core needs to save space
	BuildPeCoffLoaderHob();
	BuildExtractSectionHob(
		&gLzmaCustomDecompressGuid,
		LzmaGuidedSectionGetInfo,
		LzmaGuidedSectionExtraction
		);

	// Assume the FV that contains the SEC (our code) also contains a compressed FV.
	DecompressFirstFv();

	DEBUG((EFI_D_ERROR, "UEFI:Sec:CEntryPoint:start LoadDxeCoreFromFv\n"));

	// Load the DXE Core and transfer control to it
	LoadDxeCoreFromFv(NULL, 0);

	// DXE Core should always load and never return
	ASSERT(FALSE);
}


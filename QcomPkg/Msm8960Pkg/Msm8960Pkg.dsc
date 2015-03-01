#/** @file
#
# Copyright (c) 2015, BinSys. All rights reserved.<BR>
#
#**/

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = Msm8960Pkg
  PLATFORM_GUID                  = 91fa6c28-33df-46ac-aee6-292d6811ea31
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/QcomPkg/Msm8960Pkg
  SUPPORTED_ARCHITECTURES        = ARM
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = QcomPkg/Msm8960Pkg/Msm8960Pkg.fdf


[LibraryClasses.common]
!if $(TARGET) == RELEASE
  DebugLib                         |    MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
  UncachedMemoryAllocationLib      |    ArmPkg/Library/UncachedMemoryAllocationLib/UncachedMemoryAllocationLib.inf
!else
  DebugLib                         |    MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
  UncachedMemoryAllocationLib      |    ArmPkg/Library/UncachedMemoryAllocationLib/UncachedMemoryAllocationLib.inf
# UncachedMemoryAllocationLib      |    ArmPkg/Library/DebugUncachedMemoryAllocationLib/DebugUncachedMemoryAllocationLib.inf
!endif
  PcdLib                           |    MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  BaseLib                          |    MdePkg/Library/BaseLib/BaseLib.inf
  CpuLib                           |    MdePkg/Library/BaseCpuLib/BaseCpuLib.inf
  IoLib                            |    MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf 
  BaseMemoryLib                    |    MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
  PrintLib                         |    MdePkg/Library/BasePrintLib/BasePrintLib.inf
  UefiDecompressLib                |    MdePkg/Library/BaseUefiDecompressLib/BaseUefiDecompressLib.inf
  DebugPrintErrorLevelLib          |    MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  PeCoffGetEntryPointLib           |    MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
  PeCoffLib                        |    MdePkg/Library/BasePeCoffLib/BasePeCoffLib.inf
  
  CacheMaintenanceLib              |    ArmPkg/Library/ArmCacheMaintenanceLib/ArmCacheMaintenanceLib.inf
  ArmLib                           |    ArmPkg/Library/ArmLib/ArmV7/ArmV7Lib.inf
  
  UefiLib                          |    MdePkg/Library/UefiLib/UefiLib.inf
  UefiDriverEntryPoint             |    MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
  UefiBootServicesTableLib         |    MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  DevicePathLib                    |    MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  UefiRuntimeServicesTableLib      |    MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  
  PeCoffExtraActionLib             |    ArmPkg/Library/DebugPeCoffExtraActionLib/DebugPeCoffExtraActionLib.inf
  
  
  
  SerialPortLib                    |    QcomPkg/Library/MsmSerialPortLib/MsmSerialPortLib.inf
  MsmSharedLib                     |    QcomPkg/Library/MsmSharedLib/MsmSharedLib.inf
  MsmClockLib                      |    QcomPkg/Library/MsmClockLib/MsmClockLib.inf
  MsmSSBILib                       |    QcomPkg/Library/MsmSSBILib/MsmSSBILib.inf
  RealTimeClockLib                 |    QcomPkg/Library/MsmPM8921RealTimeClockLib/MsmPM8921RealTimeClockLib.inf
  
  
  TimerLib                         |    QcomPkg/Msm8960Pkg/Library/MsmTargetTimerLib/MsmTargetTimerLib.inf
  MsmTargetLib                     |    QcomPkg/Msm8960Pkg/Library/MsmTargetLib/MsmTargetLib.inf
  MsmTargetGpioLib                 |    QcomPkg/Msm8960Pkg/Library/MsmTargetGpioLib/MsmTargetGpioLib.inf
  EfiResetSystemLib                |    QcomPkg/Msm8960Pkg/Library/MsmTargetResetSystemLib/MsmTargetResetSystemLib.inf
  MsmTargetClockLib                |    QcomPkg/Msm8960Pkg/Library/MsmTargetClockLib/MsmTargetClockLib.inf
  MsmTargetMmcLib                  |    QcomPkg/Msm8960Pkg/Library/MsmTargetMmcLib/MsmTargetMmcLib.inf
  MsmTargetI2C                     |    QcomPkg/Msm8960Pkg/Library/MsmTargetI2C/MsmTargetI2C.inf
  
  
[LibraryClasses.common.SEC]
  ArmLib                           |    ArmPkg/Library/ArmLib/ArmV7/ArmV7LibPrePi.inf
  
  PerformanceLib                   |    MdeModulePkg/Library/PeiPerformanceLib/PeiPerformanceLib.inf

  PrePiLib                         |    EmbeddedPkg/Library/PrePiLib/PrePiLib.inf
  MemoryAllocationLib              |    EmbeddedPkg/Library/PrePiMemoryAllocationLib/PrePiMemoryAllocationLib.inf
  HobLib                           |    EmbeddedPkg/Library/PrePiHobLib/PrePiHobLib.inf
  PrePiHobListPointerLib           |    EmbeddedPkg/Library/PrePiHobListPointerLib/PrePiHobListPointerLib.inf
  ExtractGuidedSectionLib          |    EmbeddedPkg/Library/PrePiExtractGuidedSectionLib/PrePiExtractGuidedSectionLib.inf
  
  LzmaDecompressLib                |    IntelFrameworkModulePkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf
  

  
[LibraryClasses.common.PEI_CORE]

[LibraryClasses.common.DXE_CORE]
  PerformanceLib                   |    MdeModulePkg/Library/DxeCorePerformanceLib/DxeCorePerformanceLib.inf
  MemoryAllocationLib              |    MdeModulePkg/Library/DxeCoreMemoryAllocationLib/DxeCoreMemoryAllocationLib.inf
  DebugAgentLib                    |    MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf
  CpuExceptionHandlerLib           |    MdeModulePkg/Library/CpuExceptionHandlerLibNull/CpuExceptionHandlerLibNull.inf
  
  DxeCoreEntryPoint                |    MdePkg/Library/DxeCoreEntryPoint/DxeCoreEntryPoint.inf
  HobLib                           |    MdePkg/Library/DxeCoreHobLib/DxeCoreHobLib.inf
  ExtractGuidedSectionLib          |    MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf
  DxeServicesLib                   |    MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  
  PeCoffLib                        |    EmbeddedPkg/Library/DxeHobPeCoffLib/DxeHobPeCoffLib.inf
  
  ReportStatusCodeLib              |    IntelFrameworkModulePkg/Library/DxeReportStatusCodeLibFramework/DxeReportStatusCodeLib.inf



[LibraryClasses.common.DXE_DRIVER]

  
  DefaultExceptionHandlerLib       |    ArmPkg/Library/DefaultExceptionHandlerLib/DefaultExceptionHandlerLib.inf
  ArmDisassemblerLib               |    ArmPkg/Library/ArmDisassemblerLib/ArmDisassemblerLib.inf
  BdsLib                           |    ArmPkg/Library/BdsLib/BdsLib.inf
  ArmBdsHelperLib                  |    ArmPkg/Library/ArmBdsHelperLib/ArmBdsHelperLib.inf
  ArmSmcLib                        |    ArmPkg/Library/ArmSmcLib/ArmSmcLib.inf
  
  DxeServicesLib                   |    MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  DxeServicesTableLib              |    MdePkg/Library/DxeServicesTableLib/DxeServicesTableLib.inf
  HobLib                           |    MdePkg/Library/DxeHobLib/DxeHobLib.inf
  MemoryAllocationLib              |    MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
 
  SerialPortExtLib                 |    EmbeddedPkg/Library/TemplateSerialPortExtLib/TemplateSerialPortExtLib.inf
  FdtLib                           |    EmbeddedPkg/Library/FdtLib/FdtLib.inf

  ReportStatusCodeLib              |    IntelFrameworkModulePkg/Library/DxeReportStatusCodeLibFramework/DxeReportStatusCodeLib.inf
  
  SecurityManagementLib            |    MdeModulePkg/Library/DxeSecurityManagementLib/DxeSecurityManagementLib.inf
  PerformanceLib                   |    MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
  NetLib                           |    MdeModulePkg/Library/DxeNetLib/DxeNetLib.inf
  HiiLib                           |    MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  UefiHiiServicesLib               |    MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  
  MsmPM8921Lib                     |    QcomPkg/Library/MsmPM8921Lib/MsmPM8921Lib.inf
  

[LibraryClasses.common.DXE_RUNTIME_DRIVER]
  ReportStatusCodeLib              |    IntelFrameworkModulePkg/Library/DxeReportStatusCodeLibFramework/DxeReportStatusCodeLib.inf
  
  MemoryAllocationLib              |    MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  UefiRuntimeLib                   |    MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf
  HobLib                           |    MdePkg/Library/DxeHobLib/DxeHobLib.inf
  
  CapsuleLib                       |    MdeModulePkg/Library/DxeCapsuleLibNull/DxeCapsuleLibNull.inf
  
  

[LibraryClasses.common.UEFI_APPLICATION]

[LibraryClasses.common.UEFI_DRIVER]
  ReportStatusCodeLib              |    IntelFrameworkModulePkg/Library/DxeReportStatusCodeLibFramework/DxeReportStatusCodeLib.inf
  #UefiDecompressLib               |    IntelFrameworkModulePkg/Library/BaseUefiTianoCustomDecompressLib/BaseUefiTianoCustomDecompressLib.inf
  
  #ExtractGuidedSectionLib         |    MdePkg/Library/DxeExtractGuidedSectionLib/DxeExtractGuidedSectionLib.inf
  #DxeServicesLib                  |    MdePkg/Library/DxeServicesLib/DxeServicesLib.inf
  MemoryAllocationLib              |    MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  
  #PerformanceLib                  |    MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
  HiiLib                           |    MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  UefiHiiServicesLib               |    MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
  



  
[LibraryClasses.ARM]
  #
  # It is not possible to prevent the ARM compiler for generic intrinsic functions.
  # This library provides the instrinsic functions generate by a given compiler.
  # [LibraryClasses.ARM] and NULL mean link this library into all ARM images.
  #
  NULL|ArmPkg/Library/CompilerIntrinsicsLib/CompilerIntrinsicsLib.inf

  # Add support for GCC stack protector
  NULL|MdePkg/Library/BaseStackCheckLib/BaseStackCheckLib.inf

[BuildOptions]
  XCODE:*_*_ARM_PLATFORM_FLAGS == -arch armv7

  GCC:*_*_ARM_PLATFORM_FLAGS == -march=armv7-a

  RVCT:*_*_ARM_PLATFORM_FLAGS == --cpu Cortex-A8

################################################################################
#
# Pcd Section - list of all EDK II PCD Entries defined by this Platform
#
################################################################################

[PcdsFeatureFlag.common]
  gEmbeddedTokenSpaceGuid.PcdPrePiProduceMemoryTypeInformationHob  |TRUE
  gEmbeddedTokenSpaceGuid.PcdCacheEnable                           |TRUE
  
  # Use the Vector Table location in CpuDxe. We will not copy the Vector Table at PcdCpuVectorBaseAddress
  gArmTokenSpaceGuid.PcdRelocateVectorTable                        |TRUE
  gArmTokenSpaceGuid.PcdCpuDxeProduceDebugSupport                  |TRUE
  #gArmTokenSpaceGuid.PcdDebuggerExceptionSupport                   |TRUE
  
  ## If TRUE, Graphics Output Protocol will be installed on virtual handle created by ConsplitterDxe.
  #  It could be set FALSE to save size.
  #gEfiMdeModulePkgTokenSpaceGuid.PcdConOutGopSupport|TRUE
  gEfiMdePkgTokenSpaceGuid.PcdUgaConsumeSupport|FALSE

[PcdsFixedAtBuild.common]

# DEBUG_ASSERT_ENABLED       0x01
# DEBUG_PRINT_ENABLED        0x02
# DEBUG_CODE_ENABLED         0x04
# CLEAR_MEMORY_ENABLED       0x08
# ASSERT_BREAKPOINT_ENABLED  0x10
# ASSERT_DEADLOOP_ENABLED    0x20
!if $(TARGET) == RELEASE
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask                |0x21
!else
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask                |0x2f
!endif

#  DEBUG_INIT      0x00000001  // Initialization
#  DEBUG_WARN      0x00000002  // Warnings
#  DEBUG_LOAD      0x00000004  // Load events
#  DEBUG_FS        0x00000008  // EFI File system
#  DEBUG_POOL      0x00000010  // Alloc & Free's
#  DEBUG_PAGE      0x00000020  // Alloc & Free's
#  DEBUG_INFO      0x00000040  // Verbose
#  DEBUG_DISPATCH  0x00000080  // PEI/DXE Dispatchers
#  DEBUG_VARIABLE  0x00000100  // Variable
#  DEBUG_BM        0x00000400  // Boot Manager
#  DEBUG_BLKIO     0x00001000  // BlkIo Driver
#  DEBUG_NET       0x00004000  // SNI Driver
#  DEBUG_UNDI      0x00010000  // UNDI Driver
#  DEBUG_LOADFILE  0x00020000  // UNDI Driver
#  DEBUG_EVENT     0x00080000  // Event messages
#  DEBUG_ERROR     0x80000000  // Error
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel             |0x800B55CF

  gEfiMdePkgTokenSpaceGuid.PcdUartDefaultParity                |0x00

# REPORT_STATUS_CODE_PROPERTY_PROGRESS_CODE_ENABLED            0x00000001
# REPORT_STATUS_CODE_PROPERTY_ERROR_CODE_ENABLED               0x00000002
# REPORT_STATUS_CODE_PROPERTY_DEBUG_CODE_ENABLED               0x00000004

  gEfiMdePkgTokenSpaceGuid.PcdReportStatusCodePropertyMask     |0x07
  
  
  #gArmPlatformTokenSpaceGuid.PcdSecGlobalVariableSize         |0x00002000
  #gArmPlatformTokenSpaceGuid.PcdSystemMemoryUefiRegionSize    |0x02F00000

  gEmbeddedTokenSpaceGuid.PcdEmbeddedAutomaticBootCommand      |""
  gEmbeddedTokenSpaceGuid.PcdEmbeddedDefaultTextColor          |0x07
  gEmbeddedTokenSpaceGuid.PcdEmbeddedMemVariableStoreSize      |0x00010000
  
#
# Optional feature to help prevent EFI memory map fragments
# Turned on and off via: PcdPrePiProduceMemoryTypeInformationHob
# Values are in EFI Pages (4K). DXE Core will make sure that
# at least this much of each type of memory can be allocated
# from a single memory range. This way you only end up with
# maximum of two fragements for each type in the memory map
# (the memory used, and the free memory that was prereserved
# but not used).
#
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiACPIReclaimMemory    |0
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiACPIMemoryNVS        |0
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiReservedMemoryType   |0
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiRuntimeServicesData  |80
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiRuntimeServicesCode  |40
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiBootServicesCode     |400
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiBootServicesData     |3000
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiLoaderCode           |10
  gEmbeddedTokenSpaceGuid.PcdMemoryTypeEfiLoaderData           |0
  
  gEmbeddedTokenSpaceGuid.PcdMemoryBase                             |0x80000000
  gEmbeddedTokenSpaceGuid.PcdMemorySize                             |0x40000000
  
  gArmTokenSpaceGuid.PcdCpuVectorBaseAddress                   |0x81500000
  gArmTokenSpaceGuid.PcdCpuResetAddress                        |0x81500000
  
  gArmTokenSpaceGuid.PcdGicDistributorBase                     |0x02000000
  gArmTokenSpaceGuid.PcdGicInterruptInterfaceBase              |0x02002000
  
  gEmbeddedTokenSpaceGuid.PcdPrePiStackBase                    |0x81600000
  gEmbeddedTokenSpaceGuid.PcdPrePiStackSize                    |0x00040000
  
  gEmbeddedTokenSpaceGuid.PcdEmbeddedFdBaseAddress             |0x80300000
  gEmbeddedTokenSpaceGuid.PcdEmbeddedFdSize                    |0x00200000
  
  gEmbeddedTokenSpaceGuid.PcdFlashFvMainBase                   |0x80300000
  gEmbeddedTokenSpaceGuid.PcdFlashFvMainSize                   |0x00200000

  #gArmPlatformTokenSpaceGuid.PcdFirmwareVendor                |"Qualcomm Msm8960 by BinSys"
  
  gArmTokenSpaceGuid.PcdVFPEnabled                             |0x01
  
  # about 10ms
  gEmbeddedTokenSpaceGuid.PcdTimerPeriod                       |100000
  
  gEmbeddedTokenSpaceGuid.PcdMetronomeTickPeriod               |10
  
  #1 ticks about 148ns
  gEmbeddedTokenSpaceGuid.PcdEmbeddedPerformanceCounterPeriodInNanoseconds  |148

[PcdsPatchableInModule]
  # Console Resolution (Full HD)
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoHorizontalResolution|720
  gEfiMdeModulePkgTokenSpaceGuid.PcdVideoVerticalResolution|1280
  
################################################################################
#
# Components Section - list of all EDK II Modules needed by this Platform
#
################################################################################
[Components.common]

  #
  # SEC (SEC)
  #
  QcomPkg/Msm8960Pkg/Sec/Sec.inf
  
  #
  # DXE (DXE_CORE)
  #
  MdeModulePkg/Core/Dxe/DxeMain.inf{
    <LibraryClasses>
      PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
      NULL|MdeModulePkg/Library/DxeCrc32GuidedSectionExtractLib/DxeCrc32GuidedSectionExtractLib.inf
#      NULL|IntelFrameworkModulePkg/Library/LzmaCustomDecompressLib/LzmaCustomDecompressLib.inf
      NULL|EmbeddedPkg/Library/LzmaHobCustomDecompressLib/LzmaHobCustomDecompressLib.inf
  }
  
  # (DXE_DRIVER)
  MdeModulePkg/Universal/SecurityStubDxe/SecurityStubDxe.inf
  
  #(DXE_DRIVER)
  ArmPkg/Drivers/CpuDxe/CpuDxe.inf
  
  #(DXE_DRIVER)
  EmbeddedPkg/MetronomeDxe/MetronomeDxe.inf
  
  #(DXE_DRIVER)
  ArmPkg/Drivers/ArmGic/ArmGicDxe.inf{
    <LibraryClasses>
      ArmGicLib|ArmPkg/Drivers/ArmGic/ArmGicLib.inf
  }
  
  #(DXE_DRIVER)
  QcomPkg/Msm8960Pkg/Dxe/TimerDxe/TimerDxe.inf
  
  #(DXE_DRIVER)
  QcomPkg/Dxe/PM8921Dxe/PM8921Dxe.inf
  
  #(DXE_DRIVER)
  QcomPkg/Msm8960Pkg/Dxe/DisplayDxe/DisplayDxe.inf
  
  #(DXE_DRIVER)
  QcomPkg/Msm8960Pkg/Bds/Bds.inf
  
  #(DXE_DRIVER)
  QcomPkg/Msm8960Pkg/Dxe/MMCHSDxe/MMCHS.inf
  
  #(DXE_DRIVER)
  MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf
  
  #(DXE_DRIVER)
  MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf
  

  
  #(DXE_DRIVER)
  MdeModulePkg/Universal/WatchdogTimerDxe/WatchdogTimer.inf
  #     dept gEfiTimerArchProtocolGuid

  #(DXE_RUNTIME_DRIVER)
  MdeModulePkg/Core/RuntimeDxe/RuntimeDxe.inf
  
  #(DXE_RUNTIME_DRIVER)
  MdeModulePkg/Universal/Variable/EmuRuntimeDxe/EmuVariableRuntimeDxe.inf
  
  #(DXE_RUNTIME_DRIVER)
  MdeModulePkg/Universal/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf

  #(DXE_RUNTIME_DRIVER)
  EmbeddedPkg/EmbeddedMonotonicCounter/EmbeddedMonotonicCounter.inf
  
  #(DXE_RUNTIME_DRIVER)
  EmbeddedPkg/ResetRuntimeDxe/ResetRuntimeDxe.inf
  
  #(DXE_RUNTIME_DRIVER)
  EmbeddedPkg/RealTimeClockRuntimeDxe/RealTimeClockRuntimeDxe.inf
  
  #(UEFI_DRIVER)
  MdeModulePkg/Universal/Console/ConPlatformDxe/ConPlatformDxe.inf
  
  #(UEFI_DRIVER)
  MdeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf
  
  #(UEFI_DRIVER)
  MdeModulePkg/Universal/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf
  
  #(UEFI_DRIVER)
  MdeModulePkg/Universal/Console/TerminalDxe/TerminalDxe.inf 
  
  
  #(UEFI_DRIVER)
  MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIoDxe.inf
  
  #(UEFI_DRIVER)
  MdeModulePkg/Universal/Disk/PartitionDxe/PartitionDxe.inf
  
  #(UEFI_DRIVER)
  MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf
  
  #(UEFI_DRIVER)
  FatPkg/EnhancedFatDxe/Fat.inf
  
 
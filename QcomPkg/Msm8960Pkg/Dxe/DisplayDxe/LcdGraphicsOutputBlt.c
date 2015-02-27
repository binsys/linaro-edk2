/** @file

 Copyright (c) 2011-2013, ARM Ltd. All rights reserved.<BR>
 This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution.  The full text of the license may be found at
 http://opensource.org/licenses/bsd-license.php

 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

 **/

#include <PiDxe.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Guid/GlobalVariable.h>

#include "LcdGraphicsOutputDxe.h"

extern BOOLEAN mDisplayInitialized;

//
// Function Definitions
//

STATIC
EFI_STATUS
VideoCopyNoHorizontalOverlap(
	IN UINTN          BitsPerPixel,
	IN volatile VOID  *FrameBufferBase,
	IN UINT32         HorizontalResolution,
	IN UINTN          SourceX,
	IN UINTN          SourceY,
	IN UINTN          DestinationX,
	IN UINTN          DestinationY,
	IN UINTN          Width,
	IN UINTN          Height
)
{
	EFI_STATUS    Status = EFI_SUCCESS;
	UINTN         SourceLine;
	UINTN         DestinationLine;
	UINTN         WidthInBytes;
	UINTN         LineCount;
	INTN          Step;
	VOID          *SourceAddr;
	VOID          *DestinationAddr;
	INT32           BytesPerPixel;
	
	if (DestinationY <= SourceY)
	{
		// scrolling up (or horizontally but without overlap)
		SourceLine = SourceY;
		DestinationLine = DestinationY;
		Step = 1;
	}
	else
	{
		// scrolling down
		SourceLine = SourceY + Height;
		DestinationLine = DestinationY + Height;
		Step = -1;
	}

	switch (BitsPerPixel)
	{
	case LCD_BITS_PER_PIXEL_24:
		BytesPerPixel = 24 / 8;
		WidthInBytes = Width * BytesPerPixel;

		for (LineCount = 0; LineCount < Height; LineCount++)
		{
			// Update the start addresses of source & destination using 16bit pointer arithmetic
			SourceAddr = (VOID *)((UINT8 *)FrameBufferBase + SourceLine      * HorizontalResolution * BytesPerPixel + SourceX * BytesPerPixel);
			DestinationAddr = (VOID *)((UINT8 *)FrameBufferBase + DestinationLine * HorizontalResolution * BytesPerPixel + DestinationX * BytesPerPixel);

			// Copy the entire line Y from video ram to the temp buffer
			CopyMem(DestinationAddr, SourceAddr, WidthInBytes);

			// Update the line numbers
			SourceLine += Step;
			DestinationLine += Step;
		}
		break;
	default:
		// Can't handle this case
		DEBUG((DEBUG_ERROR, "ArmVeGraphics_Blt: EfiBltVideoToVideo: INVALID Number of Bits Per Pixel: %d\n", BitsPerPixel));
		Status = EFI_INVALID_PARAMETER;
		goto EXIT;
		// break;

	}

EXIT:
	return Status;
}

STATIC
EFI_STATUS
VideoCopyHorizontalOverlap(
	IN UINTN          BitsPerPixel,
	IN volatile VOID  *FrameBufferBase,
	UINT32            HorizontalResolution,
	IN UINTN          SourceX,
	IN UINTN          SourceY,
	IN UINTN          DestinationX,
	IN UINTN          DestinationY,
	IN UINTN          Width,
	IN UINTN          Height
)
{
	EFI_STATUS      Status = EFI_SUCCESS;

	UINT8 *PixelBuffer8bit;
	UINT8 *SourcePixel8bit;
	UINT8 *DestinationPixel8bit;

	UINT32          SourcePixelY;
	UINT32          DestinationPixelY;
	INT32           BytesPerPixel;

	switch (BitsPerPixel)
	{
	case LCD_BITS_PER_PIXEL_24:
		BytesPerPixel = 24 / 8;
		// Allocate a temporary buffer
		PixelBuffer8bit = (UINT8 *)AllocatePool((Height * Width) * sizeof(UINT8) * BytesPerPixel);

		if (PixelBuffer8bit == NULL)
		{
			Status = EFI_OUT_OF_RESOURCES;
			goto EXIT;
		}

		// Access each pixel inside the source area of the Video Memory and copy it to the temp buffer
		for (SourcePixelY = SourceY, DestinationPixel8bit = PixelBuffer8bit;
			SourcePixelY < SourceY + Height;
			SourcePixelY++, DestinationPixel8bit += Width * BytesPerPixel)
		{
			// Calculate the source address:
			SourcePixel8bit = (UINT8 *)(FrameBufferBase + (SourcePixelY * HorizontalResolution * BytesPerPixel + SourceX * BytesPerPixel));

			// Copy the entire line Y from Video to the temp buffer
			CopyMem((VOID *)DestinationPixel8bit, (CONST VOID *)SourcePixel8bit, Width * BytesPerPixel);
		}

		// Copy from the temp buffer into the destination area of the Video Memory

		for (DestinationPixelY = DestinationY, SourcePixel8bit = PixelBuffer8bit;
			DestinationPixelY < DestinationY + Height;
			DestinationPixelY++, SourcePixel8bit += Width * BytesPerPixel)
		{
			// Calculate the target address:
			DestinationPixel8bit = (UINT8 *)(FrameBufferBase + (DestinationPixelY * HorizontalResolution * BytesPerPixel + DestinationX * BytesPerPixel));

			// Copy the entire line Y from the temp buffer to Video
			CopyMem((VOID *)DestinationPixel8bit, (CONST VOID *)SourcePixel8bit, Width * BytesPerPixel);
		}

		// Free the allocated memory
		FreePool((VOID *)PixelBuffer8bit);

		break;
	default:
		// Can't handle this case
		DEBUG((DEBUG_ERROR, "ArmVeGraphics_Blt: EfiBltVideoToVideo: INVALID Number of Bits Per Pixel: %d\n", BitsPerPixel));
		Status = EFI_INVALID_PARAMETER;
		goto EXIT;
		// break;

	}

EXIT:
	return Status;
}

STATIC
EFI_STATUS
BltVideoFill(
	IN EFI_GRAPHICS_OUTPUT_PROTOCOL        *This,
	IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *EfiSourcePixel, OPTIONAL
	IN UINTN                               SourceX,
	IN UINTN                               SourceY,
	IN UINTN                               DestinationX,
	IN UINTN                               DestinationY,
	IN UINTN                               Width,
	IN UINTN                               Height,
	IN UINTN                               Delta           OPTIONAL   // Number of BYTES in a row of the BltBuffer
)
{
	EFI_PIXEL_BITMASK*  PixelInformation;
	EFI_STATUS         Status;
	UINT32             HorizontalResolution;
	LCD_BPP            BitsPerPixel;
	VOID            *FrameBufferBase;
	UINT8          *DestinationPixel8bit;
	UINT32          DestinationPixelX;
	UINT32          DestinationLine;
	INT32           BytesPerPixel;

	Status = EFI_SUCCESS;
	PixelInformation = &This->Mode->Info->PixelInformation;
	FrameBufferBase = (UINTN *)((UINTN)(This->Mode->FrameBufferBase));
	HorizontalResolution = This->Mode->Info->HorizontalResolution;

	LcdPlatformGetBpp(This->Mode->Mode, &BitsPerPixel);
	
	

	switch (BitsPerPixel)
	{
	case LCD_BITS_PER_PIXEL_24:
		BytesPerPixel = 24 / 8;
		// Copy the SourcePixel into every pixel inside the target rectangle
		for (DestinationLine = DestinationY;
			DestinationLine < DestinationY + Height;
			DestinationLine++)
		{
			for (DestinationPixelX = DestinationX;
				DestinationPixelX < DestinationX + Width;
				DestinationPixelX++)
			{
				// Calculate the target address:
				DestinationPixel8bit = (UINT8 *)(FrameBufferBase + DestinationLine * HorizontalResolution * BytesPerPixel + DestinationPixelX * BytesPerPixel);

				// Copy the pixel into the new target
				DestinationPixel8bit[0] = EfiSourcePixel->Red;
				DestinationPixel8bit[1] = EfiSourcePixel->Green;
				DestinationPixel8bit[2] = EfiSourcePixel->Blue;
			}
		}
		break;
	default:
		// Can't handle this case
		DEBUG((DEBUG_ERROR, "LcdGraphicsBlt: EfiBltVideoFill: INVALID Number of Bits Per Pixel: %d\n", BitsPerPixel));
		Status = EFI_INVALID_PARAMETER;
		break;
	}

	return Status;
}

STATIC
EFI_STATUS
BltVideoToBltBuffer(
	IN EFI_GRAPHICS_OUTPUT_PROTOCOL        *This,
	IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *BltBuffer, OPTIONAL
	IN UINTN                               SourceX,
	IN UINTN                               SourceY,
	IN UINTN                               DestinationX,
	IN UINTN                               DestinationY,
	IN UINTN                               Width,
	IN UINTN                               Height,
	IN UINTN                               Delta           OPTIONAL   // Number of BYTES in a row of the BltBuffer
)
{
	EFI_STATUS         Status;
	UINT32             HorizontalResolution;
	LCD_BPP            BitsPerPixel;
	EFI_PIXEL_BITMASK  *PixelInformation;
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL *EfiDestinationPixel;
	VOID   *FrameBufferBase;

	UINT8 *SourcePixel8bit;

	UINT32          SourcePixelX;
	UINT32          SourceLine;
	UINT32          DestinationPixelX;
	UINT32          DestinationLine;
	UINT32          BltBufferHorizontalResolution;
	INT32           BytesPerPixel;

	Status = EFI_SUCCESS;
	PixelInformation = &This->Mode->Info->PixelInformation;
	HorizontalResolution = This->Mode->Info->HorizontalResolution;
	FrameBufferBase = (UINTN *)((UINTN)(This->Mode->FrameBufferBase));

	if ((Delta != 0) && (Delta != Width * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)))
	{
		// Delta is not zero and it is different from the width.
		// Divide it by the size of a pixel to find out the buffer's horizontal resolution.
		BltBufferHorizontalResolution = (UINT32)(Delta / sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
	}
	else
	{
		BltBufferHorizontalResolution = Width;
	}

	LcdPlatformGetBpp(This->Mode->Mode, &BitsPerPixel);

	switch (BitsPerPixel)
	{
	case LCD_BITS_PER_PIXEL_24:
		BytesPerPixel = 24 / 8;
		// Access each pixel inside the Video Memory
		for (SourceLine = SourceY, DestinationLine = DestinationY;
			SourceLine < SourceY + Height;
			SourceLine++, DestinationLine++)
		{
			for (SourcePixelX = SourceX, DestinationPixelX = DestinationX;
				SourcePixelX < SourceX + Width;
				SourcePixelX++, DestinationPixelX++)
			{
				// Calculate the source and target addresses:
				SourcePixel8bit = (UINT8 *)(FrameBufferBase + SourceLine * HorizontalResolution * BytesPerPixel + SourcePixelX * BytesPerPixel);
				EfiDestinationPixel = BltBuffer + DestinationLine * BltBufferHorizontalResolution + DestinationPixelX;


				// Copy the pixel into the new target
				// There is no info for the Reserved byte, so we set it to zero
				EfiDestinationPixel->Red = SourcePixel8bit[0];
				EfiDestinationPixel->Green = SourcePixel8bit[1];
				EfiDestinationPixel->Blue = SourcePixel8bit[2];
				// EfiDestinationPixel->Reserved = (UINT8) 0;
			}
		}
		break;
	default:
		// Can't handle this case
		DEBUG((DEBUG_ERROR, "LcdGraphicsBlt: EfiBltVideoToBltBuffer: INVALID Number of Bits Per Pixel: %d\n", BitsPerPixel));
		Status = EFI_INVALID_PARAMETER;
		break;
	}
	return Status;
}

STATIC
EFI_STATUS
BltBufferToVideo(
	IN EFI_GRAPHICS_OUTPUT_PROTOCOL        *This,
	IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *BltBuffer, OPTIONAL
	IN UINTN                               SourceX,
	IN UINTN                               SourceY,
	IN UINTN                               DestinationX,
	IN UINTN                               DestinationY,
	IN UINTN                               Width,
	IN UINTN                               Height,
	IN UINTN                               Delta           OPTIONAL   // Number of BYTES in a row of the BltBuffer
)
{
	EFI_STATUS         Status;
	UINT32             HorizontalResolution;
	LCD_BPP            BitsPerPixel;
	EFI_PIXEL_BITMASK  *PixelInformation;
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL *EfiSourcePixel;
	VOID   *FrameBufferBase;
	UINT8 *DestinationPixel8bit;
	UINT32          SourcePixelX;
	UINT32          SourceLine;
	UINT32          DestinationPixelX;
	UINT32          DestinationLine;
	UINT32          BltBufferHorizontalResolution;
	INT32           BytesPerPixel;

	Status = EFI_SUCCESS;
	PixelInformation = &This->Mode->Info->PixelInformation;
	HorizontalResolution = This->Mode->Info->HorizontalResolution;
	FrameBufferBase = (UINTN *)((UINTN)(This->Mode->FrameBufferBase));

	if ((Delta != 0) && (Delta != Width * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)))
	{
		// Delta is not zero and it is different from the width.
		// Divide it by the size of a pixel to find out the buffer's horizontal resolution.
		BltBufferHorizontalResolution = (UINT32)(Delta / sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
	}
	else
	{
		BltBufferHorizontalResolution = Width;
	}

	LcdPlatformGetBpp(This->Mode->Mode, &BitsPerPixel);

	switch (BitsPerPixel)
	{
	case LCD_BITS_PER_PIXEL_24:
		BytesPerPixel = 24 / 8;
		// Access each pixel inside the BltBuffer Memory
		for (SourceLine = SourceY, DestinationLine = DestinationY;
			SourceLine < SourceY + Height;
			SourceLine++, DestinationLine++)
		{

			for (SourcePixelX = SourceX, DestinationPixelX = DestinationX;
				SourcePixelX < SourceX + Width;
				SourcePixelX++, DestinationPixelX++)
			{
				// Calculate the source and target addresses:
				EfiSourcePixel = BltBuffer + SourceLine * BltBufferHorizontalResolution + SourcePixelX;
				DestinationPixel8bit = (UINT8 *)(FrameBufferBase + DestinationLine * HorizontalResolution * BytesPerPixel + DestinationPixelX * BytesPerPixel);
				
				DestinationPixel8bit[0] = EfiSourcePixel->Red;
				DestinationPixel8bit[1] = EfiSourcePixel->Green;
				DestinationPixel8bit[2] = EfiSourcePixel->Blue;
			}
		}
		break;
	default:
		// Can't handle this case
		DEBUG((DEBUG_ERROR, "LcdGraphicsBlt: EfiBltBufferToVideo: INVALID Number of Bits Per Pixel: %d\n", BitsPerPixel));
		Status = EFI_INVALID_PARAMETER;
		break;
	}
	return Status;
}

STATIC
EFI_STATUS
BltVideoToVideo(
	IN EFI_GRAPHICS_OUTPUT_PROTOCOL        *This,
	IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *BltBuffer, OPTIONAL
	IN UINTN                               SourceX,
	IN UINTN                               SourceY,
	IN UINTN                               DestinationX,
	IN UINTN                               DestinationY,
	IN UINTN                               Width,
	IN UINTN                               Height,
	IN UINTN                               Delta           OPTIONAL   // Number of BYTES in a row of the BltBuffer
)
{
	EFI_STATUS         Status;
	UINT32             HorizontalResolution;
	LCD_BPP            BitsPerPixel;
	VOID   *FrameBufferBase;

	HorizontalResolution = This->Mode->Info->HorizontalResolution;
	FrameBufferBase = (UINTN *)((UINTN)(This->Mode->FrameBufferBase));

	//
	// BltVideo to BltVideo:
	//
	//  Source is the Video Memory,
	//  Destination is the Video Memory

	LcdPlatformGetBpp(This->Mode->Mode, &BitsPerPixel);
	FrameBufferBase = (UINTN *)((UINTN)(This->Mode->FrameBufferBase));

	// The UEFI spec currently states:
	// "There is no limitation on the overlapping of the source and destination rectangles"
	// Therefore, we must be careful to avoid overwriting the source data
	if (SourceY == DestinationY)
	{
		// Copying within the same height, e.g. horizontal shift
		if (SourceX == DestinationX)
		{
			// Nothing to do
			Status = EFI_SUCCESS;
		}
		else if (((SourceX > DestinationX) ? (SourceX - DestinationX) : (DestinationX - SourceX)) < Width)
		{
			// There is overlap
			Status = VideoCopyHorizontalOverlap(BitsPerPixel, FrameBufferBase, HorizontalResolution, SourceX, SourceY, DestinationX, DestinationY, Width, Height);
		}
		else
		{
			// No overlap
			Status = VideoCopyNoHorizontalOverlap(BitsPerPixel, FrameBufferBase, HorizontalResolution, SourceX, SourceY, DestinationX, DestinationY, Width, Height);
		}
	}
	else
	{
		// Copying from different heights
		Status = VideoCopyNoHorizontalOverlap(BitsPerPixel, FrameBufferBase, HorizontalResolution, SourceX, SourceY, DestinationX, DestinationY, Width, Height);
	}

	return Status;
}

extern void trigger_mdp_dsi(void);
/***************************************
 * GraphicsOutput Protocol function, mapping to
 * EFI_GRAPHICS_OUTPUT_PROTOCOL.Blt
 *
 * PRESUMES: 1 pixel = 4 bytes (32bits)
 *  ***************************************/
EFI_STATUS
EFIAPI
LcdGraphicsBlt(
	IN EFI_GRAPHICS_OUTPUT_PROTOCOL        *This,
	IN OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *BltBuffer, OPTIONAL
	IN EFI_GRAPHICS_OUTPUT_BLT_OPERATION   BltOperation,
	IN UINTN                               SourceX,
	IN UINTN                               SourceY,
	IN UINTN                               DestinationX,
	IN UINTN                               DestinationY,
	IN UINTN                               Width,
	IN UINTN                               Height,
	IN UINTN                               Delta           OPTIONAL   // Number of BYTES in a row of the BltBuffer
)
{
	EFI_STATUS         Status;
	UINT32             HorizontalResolution;
	UINT32             VerticalResolution;
	LCD_INSTANCE*      Instance;

	Instance = LCD_INSTANCE_FROM_GOP_THIS(This);

	// Setup the hardware if not already done
	if (!mDisplayInitialized)
	{
		Status = InitializeDisplay(Instance);
		if (EFI_ERROR(Status))
		{
			goto EXIT;
		}
	}

	HorizontalResolution = This->Mode->Info->HorizontalResolution;
	VerticalResolution = This->Mode->Info->VerticalResolution;

	//DEBUG((DEBUG_INFO, "LcdGraphicsBlt (BltOperation:%d,DestX:%d,DestY:%d,Width:%d,Height:%d) res(%d,%d)\n",
	//	BltOperation, DestinationX, DestinationY, Width, Height, HorizontalResolution, VerticalResolution));

	// Check we have reasonable parameters
	if (Width == 0 || Height == 0)
	{
		DEBUG((DEBUG_ERROR, "LcdGraphicsBlt: ERROR - Invalid dimension: Zero size area.\n"));
		Status = EFI_INVALID_PARAMETER;
		goto EXIT;
	}

	if ((BltOperation == EfiBltVideoFill) || (BltOperation == EfiBltBufferToVideo) || (BltOperation == EfiBltVideoToBltBuffer))
	{
		ASSERT(BltBuffer != NULL);
	}

	/*if ((DestinationX >= HorizontalResolution) || (DestinationY >= VerticalResolution)) {
	  DEBUG((DEBUG_ERROR, "LcdGraphicsBlt: ERROR - Invalid destination.\n" ));
	  Status = EFI_INVALID_PARAMETER;
	  goto EXIT;
	  }*/

	// If we are reading data out of the video buffer, check that the source area is within the display limits
	if ((BltOperation == EfiBltVideoToBltBuffer) || (BltOperation == EfiBltVideoToVideo))
	{
		if ((SourceY + Height > VerticalResolution) || (SourceX + Width > HorizontalResolution))
		{
			DEBUG((DEBUG_INFO, "LcdGraphicsBlt: ERROR - Invalid source resolution.\n"));
			DEBUG((DEBUG_INFO, "                      - SourceY=%d + Height=%d > VerticalResolution=%d.\n", SourceY, Height, VerticalResolution));
			DEBUG((DEBUG_INFO, "                      - SourceX=%d + Width=%d > HorizontalResolution=%d.\n", SourceX, Width, HorizontalResolution));
			Status = EFI_INVALID_PARAMETER;
			goto EXIT;
		}
	}

	// If we are writing data into the video buffer, that the destination area is within the display limits
	if ((BltOperation == EfiBltVideoFill) || (BltOperation == EfiBltBufferToVideo) || (BltOperation == EfiBltVideoToVideo))
	{
		if ((DestinationY + Height > VerticalResolution) || (DestinationX + Width > HorizontalResolution))
		{
			DEBUG((DEBUG_INFO, "LcdGraphicsBlt: ERROR - Invalid destination resolution.\n"));
			DEBUG((DEBUG_INFO, "                      - DestinationY=%d + Height=%d > VerticalResolution=%d.\n", DestinationY, Height, VerticalResolution));
			DEBUG((DEBUG_INFO, "                      - DestinationX=%d + Width=%d > HorizontalResolution=%d.\n", DestinationX, Width, HorizontalResolution));
			Status = EFI_INVALID_PARAMETER;
			goto EXIT;
		}
	}

	//
	// Perform the Block Transfer Operation
	//

	switch (BltOperation)
	{
	case EfiBltVideoFill:
		Status = BltVideoFill(This, BltBuffer, SourceX, SourceY, DestinationX, DestinationY, Width, Height, Delta);
		break;

	case EfiBltVideoToBltBuffer:
		Status = BltVideoToBltBuffer(This, BltBuffer, SourceX, SourceY, DestinationX, DestinationY, Width, Height, Delta);
		break;

	case EfiBltBufferToVideo:
		Status = BltBufferToVideo(This, BltBuffer, SourceX, SourceY, DestinationX, DestinationY, Width, Height, Delta);
		break;

	case EfiBltVideoToVideo:
		Status = BltVideoToVideo(This, BltBuffer, SourceX, SourceY, DestinationX, DestinationY, Width, Height, Delta);
		break;

	case EfiGraphicsOutputBltOperationMax:
	default:
		DEBUG((DEBUG_ERROR, "LcdGraphicsBlt: Invalid Operation\n"));
		Status = EFI_INVALID_PARAMETER;
		break;
	}
	
	trigger_mdp_dsi();

EXIT:
	return Status;
}

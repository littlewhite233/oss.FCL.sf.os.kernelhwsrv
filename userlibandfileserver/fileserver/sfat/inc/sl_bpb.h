// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// f32\sfat\inc\sl_bpb.h
// @file
// @internalTechnology
// 
//


#ifndef SL_BPB_H 
#define SL_BPB_H


#include "filesystem_fat.h"
using FileSystem_FAT::TFatSubType;
typedef TFatSubType TFatType;


const TInt KVolumeLabelSize			=11;    ///< Volume lable size
const TInt KFileSysTypeSize			=8;     ///< File system type parameter size
const TInt KVendorIdSize			=8;     ///< Vendor ID parameter size
const TInt KBootSectorSignature		=0xAA55;///< File system Boot sector signiture

const TInt KSizeOfFatBootSector	    =62;    ///< Size in bytes of Boot sector parameter block (BPB), 62 for fat16|12
const TInt KFat16VolumeLabelPos     =43;    ///< Position of volume lable in BPB for Fat12/16

const TUint32 KBootSectorNum        =0;     ///< Main Boot Sector number (always 0)

//-------------------------------------------------------------------------------------------------------------------

/**
Boot sector parameter block, enables access to all file system parameters.
Data is populated at mount time from the BPB sector
*/
class TFatBootSector
	{
public:
    //-- simple getters / setters
	inline const TPtrC8 VendorId() const;
	inline TInt BytesPerSector() const;
	inline TInt SectorsPerCluster() const;
	inline TInt ReservedSectors() const;
	inline TInt NumberOfFats() const;
	inline TInt RootDirEntries() const;
	inline TInt TotalSectors() const;
	inline TUint8 MediaDescriptor() const;
	inline TInt FatSectors() const;
	inline TInt SectorsPerTrack() const;
	inline TInt NumberOfHeads() const;
	inline TInt HiddenSectors() const;
	inline TInt HugeSectors() const;
	inline TInt PhysicalDriveNumber() const;
	inline TInt ExtendedBootSignature() const;
	inline TUint32 UniqueID() const;
	inline const TPtrC8 VolumeLabel() const;
	inline const TPtrC8 FileSysType() const;
	inline TInt BootSectorSignature() const;

    inline TUint32 RootClusterNum() const  {return 0;} //-- dummy	
	inline TUint16 FSInfoSectorNum() const {return 0;} //-- dummy	
	inline TUint16 BkBootRecSector() const {return 0;} //-- dummy	


	inline void SetJumpInstruction();
	inline void SetVendorID(const TDesC8& aDes);
	inline void SetBytesPerSector(TInt aBytesPerSector);
	inline void SetSectorsPerCluster(TInt aSectorsPerCluster);
	inline void SetReservedSectors(TInt aReservedSectors);
	inline void SetNumberOfFats(TInt aNumberOfFats);
	inline void SetRootDirEntries(TInt aRootDirEntries);
	inline void SetTotalSectors(TInt aTotalSectors);
	inline void SetMediaDescriptor(TUint8 aMediaDescriptor);
	inline void SetFatSectors(TInt aFatSectors);
	inline void SetSectorsPerTrack(TInt aSectorsPerTrack);
	inline void SetNumberOfHeads(TInt aNumberOfHeads);
	inline void SetHiddenSectors(TUint32 aHiddenSectors);
	inline void SetHugeSectors(TUint32 aTotalSectors);
	inline void SetPhysicalDriveNumber(TInt aPhysicalDriveNumber);
	inline void SetReservedByte(TUint8 aReservedByte);
	inline void SetExtendedBootSignature(TInt anExtendedBootSignature);
	inline void SetUniqueID(TUint32 anUniqueID);
	inline void SetVolumeLabel(const TDesC8& aDes);
	inline void SetFileSysType(const TDesC8& aDes);

public:

    TFatBootSector();

    void Initialise();
    TBool IsValid() const;
    TFatType FatType(void) const;
	
    void Internalize(const TDesC8& aBuf);
    void Externalize(TDes8& aBuf) const;
	void PrintDebugInfo() const;

    //-- more advanced API, works for all FAT types
    TInt FirstFatSector() const;
    TInt RootDirStartSector() const;
	TInt FirstDataSector() const;
    
    TUint32 VolumeTotalSectorNumber() const;
    TUint32 TotalFatSectors() const;
    TUint32 RootDirSectors() const;


protected:
    
    TUint8  iJumpInstruction[3];            ///< +0         Jump instruction used for bootable volumes
    TUint8  iVendorId[KVendorIdSize];       ///< +3         Vendor ID of the file system that formatted the volume
    TUint16 iBytesPerSector;                ///< +11/0x0b   Bytes per sector 
    TUint8  iSectorsPerCluster;             ///< +13/0x0d   Sectors per cluster ratio
    TUint16 iReservedSectors;               ///< +14/0x0e   Number of reserved sectors on the volume
    TUint8  iNumberOfFats;                  ///< +16/0x10   Number of Fats on the volume
    TUint16 iRootDirEntries;	            ///< +17/0x11   Number of entries allowed in the root directory, specific to Fat12/16, zero for FAT32
    TUint16 iTotalSectors;                  ///< +19/0x13   Total sectors on the volume, zero for FAT32
    TUint8  iMediaDescriptor;               ///< +12/0x15   Media descriptor
    TUint16 iFatSectors;                    ///< +22/0x16   Sectors used for the Fat table, zero for FAT32
    TUint16 iSectorsPerTrack;               ///< +24/0x18   Sectors per track
    TUint16 iNumberOfHeads;                 ///< +26/0x1a   Number of heads 
    TUint32 iHiddenSectors;                 ///< +28/0x1c   Number of hidden sectors in the volume
    TUint32 iHugeSectors;                   ///< +32/0x20   Total sectors in the volume, Used if totalSectors > 65535
	//this is (boot sector) offset 36 for FAT12 and 16 but comes after additional FAT36 elements
	TUint8 iPhysicalDriveNumber;            ///< +36/0x24   Physical drive number, not used in Symbian OS
    TUint8 iReserved;                       ///< +37/0x25   Reserved byte
    TUint8 iExtendedBootSignature;          ///< +38/0x26   Extended boot signiture
    TUint32 iUniqueID;                      ///< +39/0x27   Unique volume ID
    TUint8 iVolumeLabel[KVolumeLabelSize];  ///< +43/0x2b   The volume's label
	TUint8 iFileSysType[KFileSysTypeSize];  ///< +54/0x36   File system type
	};





#endif //SL_BPB_H

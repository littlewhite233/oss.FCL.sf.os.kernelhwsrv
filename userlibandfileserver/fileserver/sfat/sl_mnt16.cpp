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
// f32\sfat32\sl_mnt16.cpp
// CFatMountCB code, specific to the EFAT.FSY
// 
//

/**
 @file 
*/

#include "sl_std.h"
#include "sl_cache.h"
#include "sl_leafdir_cache.h"


//-------------------------------------------------------------------------------------------------------------------
/**
    Write aligned members of TFatBootSector to media
 
    @param  aMediaPos   media position the data will be written to
    @param  aBootSector data to write
    @return Media write error code
*/
TInt CFatMountCB::DoWriteBootSector(TInt64 aMediaPos, const TFatBootSector& aBootSector) const
    {
    __PRINT2(_L("#- CFatMountCB::DoWriteBootSector() drv:%d, pos:0x%x"),Drive().DriveNumber(), (TUint32)aMediaPos);   

    ASSERT(aMediaPos>=0);

    TBuf8<KDefaultSectorSize> bootSecBuf(KDefaultSectorSize);
    bootSecBuf.FillZ();

    //-- externalize boot sector to the data buffer
    aBootSector.Externalize(bootSecBuf);

    //-- put a boot sector signature to the last 2 bytes
    bootSecBuf[KDefaultSectorSize-2] = 0x55;
    bootSecBuf[KDefaultSectorSize-1] = 0xaa;

    //-- write boot sector to the media
    TInt r=LocalDrive()->Write(aMediaPos, bootSecBuf);
    if (r!=KErrNone)
        {//-- write failure
        __PRINT2(_L("CFatMountCB::DoWriteBootSector() failed! drv:%d, code:%d"),Drive().DriveNumber(),r);
        }

    return r;
    }

//-------------------------------------------------------------------------------------------------------------------

/**
    Read non aligned boot data from media into TFatBootSector structure

    @param  aMediaPos   media position the data will be read from
    @param  aBootSector refrence to TFatBootSector populate
    @return Media read error code
*/
TInt CFatMountCB::DoReadBootSector(TInt64 aMediaPos, TFatBootSector& aBootSector) const 
    {
    __PRINT2(_L("#- CFatMountCB::DoReadBootSector() drv:%d, pos:0x%x"),Drive().DriveNumber(), (TUint32)aMediaPos);   

    ASSERT(aMediaPos>=0);

    TBuf8<KSizeOfFatBootSector> bootSecBuf(KSizeOfFatBootSector);
    
    //-- read boot sector from the media
    TInt r=LocalDrive()->Read(aMediaPos, KSizeOfFatBootSector, bootSecBuf);
    if (r != KErrNone)
        {
        __PRINT2(_L("CFatMountCB::DoReadBootSector() failed! drv:%d, code:%d"),Drive().DriveNumber(),r);

	    //-- fiddling with the error code; taken from MountL()
        if (r==KErrNotSupported)
		    return KErrNotReady;
    #if defined(_LOCKABLE_MEDIA)
	    else if(r==KErrLocked)
		    return KErrLocked;
    #endif
	    else if (r!=KErrNoMemory && r!=KErrNotReady && r!=KErrCorrupt && r!=KErrUnknown)
                return KErrCorrupt; 

        return r;
        }

    ASSERT(r==KErrNone);

    //-- initialise TFatBootSector object
    aBootSector.Internalize(bootSecBuf);

    //-- Validate the partition size, and fix up if the out of bounds
    TLocalDriveCapsV2Buf localDriveCaps;
    r = LocalDrive()->Caps(localDriveCaps);
    ASSERT(r==KErrNone);
    
    if(!(localDriveCaps().iMediaAtt & KMediaAttVariableSize))
        {//-- this is not a RAM drive.
        const TUint32 maxSectors = I64LOW(localDriveCaps().iSize >> KDefSectorSzLog2);

        if(aBootSector.TotalSectors())
            aBootSector.SetTotalSectors(Min(aBootSector.TotalSectors(), maxSectors));
        else
            aBootSector.SetHugeSectors(Min(aBootSector.HugeSectors(), maxSectors));
        }

    return KErrNone;
    }

//-------------------------------------------------------------------------------------------------------------------

/**
    Read and validate the boot sector.
    @param      aBootSector reference to the boot sector object to be read.
    @param      aDoNotReadBkBootSec if true, there won't be an attempt to read backup sector (N/A for FAT12/16)
    @return     standard error code.
*/
TInt CFatMountCB::ReadBootSector(TFatBootSector& aBootSector, TBool /*aDoNotReadBkBootSec=EFalse*/)
    {
    //-- read main boot sector from the sector 0
    TInt nRes = DoReadBootSector(KBootSectorNum << KDefSectorSzLog2, aBootSector); 
    if(nRes == KErrNone)
        {
        if(aBootSector.IsValid())
            {//-- main boot sector is valid, everything is OK
            return KErrNone;
            }
        else
            {
            __PRINT(_L("Boot Sector is invalid! dump:\n"));
            aBootSector.PrintDebugInfo();
            return KErrCorrupt;
            }
        }

    //-- can't read boot sector
    return nRes;
    }

//-------------------------------------------------------------------------------------------------------------------

/**
Write a new volume label to BPB in media

@param aVolumeLabel Descriptor containing the new volume label
@leave 
*/
void CFatMountCB::WriteVolumeLabelL(const TDesC8& aVolumeLabel) const
	{
    if(aVolumeLabel.Length() > KVolumeLabelSize)
        User::Leave(KErrArgument);

	User::LeaveIfError(LocalDrive()->Write(KFat16VolumeLabelPos,aVolumeLabel)); 
    }



//-------------------------------------------------------------------------------------------------------------------

const TUint16 KFat16CleanShutDownMask	= 0x08000;    ///< Mask used to indicate test clean/dirty bit for Fat16

/**
Set or reset "VolumeClean" (ClnShutBitmask) flag.

@param  aClean if ETrue, marks the volume as clean, otherwise as dirty.
@leave  if write error occured.        
*/
void CFatMountCB::SetVolumeCleanL(TBool aClean) 
    {
	//-- The volume can't be set clean if there are objects opened on it. This precondition must be checked before calling this function
    if(aClean && LockStatus()!=0)
        {
        __PRINT1(_L("#- CFatMountCB::SetVolumeCleanL drive:%d isn't free!"),DriveNumber());
        ASSERT(0);
        User::Leave(KErrInUse);
        return;
        }

    if(FatType() == EFat12)
        {//-- Fat12 doesn't support this feature; do nothing other than notify the underlying drive
		 //   (ignoring any error for now as there's nothing we can do with it)
		(void)LocalDrive()->Finalise(aClean);
		return;
        }

    //-- further read and write will be directly from the CProxyDrive, bypassing FAT cache. 
    //-- this is because CFatTable doesn't allow access to FAT[1]

    if(Is16BitFat())
        {//-- Fat16
        __PRINT2(_L("#- CFatMountCB::SetVolumeCleanL, drive:%d, param:%d, FAT16, efat.fsy"),DriveNumber(), aClean);

        if(FatConfig().FAT16_UseCleanShutDownBit())
            {

            TFat16Entry fatEntry;
            const TInt  KFatEntrySize=sizeof(fatEntry); //-- FAT entry size in bytes
            TPtr8       ptrFatEntry((TUint8*)&fatEntry,KFatEntrySize);
        
            User::LeaveIfError(LocalDrive()->Read(StartOfFatInBytes()+KFatEntrySize, KFatEntrySize, ptrFatEntry)); //read FAT16[1] entry
        
            const TFat16Entry tmp = fatEntry;
        
            if(aClean)
                fatEntry |= KFat16CleanShutDownMask;  //-- set ClnShutBit flag
            else
                fatEntry &= ~KFat16CleanShutDownMask; //-- reset ClnShutBit flag

            if(tmp != fatEntry)
                {//-- write FAT[1] entry to all available FATs
                for(TInt i=0; i<NumberOfFats(); ++i)
                    {
                    const TInt64 pos = StartOfFatInBytes()+KFatEntrySize+(FatSizeInBytes()*i);
                    User::LeaveIfError(LocalDrive()->Write(pos, ptrFatEntry)); //write FAT16[1] entry
                    }
                }
		    
		     //-- Notify the underlying media that the mount is consistent
		     //   (ignoring any error for now as there's nothing we can do with it)
		    (void)LocalDrive()->Finalise(aClean);

            __PRINT2(_L("#- CFatMountCB::SetVolumeCleanL() entry:  %x->%x"), tmp, fatEntry);    
        
            }
        else //if(FatConfig().FAT16_UseCleanShutDownBit())
            {
            __PRINT(_L("#- changing FAT16[1] is disabled in config!"));    
            }
       
        }// if(Is16BitFat())
    else
        {//-- must never get here
        ASSERT(0);
        }
    
    }

//-------------------------------------------------------------------------------------------------------------------

/**
Determine whether "VolumeClean" (ClnShutBitmask) flag is set.

@return ETrue if the volume is marked as clean and EFalse otherwise.
@leave  if is called for FAT12 or if read error occured.
*/
TBool CFatMountCB::VolumeCleanL()
    {
    TFatDriveInterface& drive = DriveInterface();
    
    if(Is16BitFat())
        {//-- Fat16
        TFat16Entry fatEntry;
        const TInt  KFatEntrySize=sizeof(fatEntry); //-- FAT entry size in bytes
        TPtr8       ptrFatEntry((TUint8*)&fatEntry, KFatEntrySize);
        
        User::LeaveIfError(drive.ReadNonCritical(StartOfFatInBytes()+KFatEntrySize, KFatEntrySize, ptrFatEntry)); //read FAT16[1] entry
        return (fatEntry & KFat16CleanShutDownMask);
        }
    else
        {//-- Fat12 doesn't support this feature, shan't get here, actually
        ASSERT(0);
        User::Leave(KErrNotSupported);
        return ETrue; //-- to satisfy the compiler
        }
    }

//-------------------------------------------------------------------------------------------------------------------

/**
Mount a Fat volume. 

@param aForceMount Flag to indicate whether mount should be forced to succeed if an error occurs
@leave KErrNoMemory,KErrNotReady,KErrCorrupt,KErrUnknown.
*/
void CFatMountCB::MountL(TBool aForceMount)
	{
    const TInt driveNo = Drive().DriveNumber();
    
    __PRINT2(_L("CFatMountCB::MountL() drv:%d, forceMount=%d\n"),driveNo,aForceMount);

    ASSERT(State() == ENotMounted || State() == EDismounted);
    SetState(EMounting);
    SetReadOnly(EFalse);
   

	User::LeaveIfError(CreateDrive(Drive().DriveNumber()));

    //-- read FAT configuration parameters from estart.txt
    iFatConfig.ReadConfig(driveNo);


    //-- initialise interface to the low-level drive access
    if(!iDriverInterface.Init(this))
        User::LeaveIfError(KErrNoMemory);    

	//-- get drive capabilities
    TLocalDriveCapsV2Buf capsBuf;
	User::LeaveIfError(LocalDrive()->Caps(capsBuf));
	
    iSize=capsBuf().iSize;
    iRamDrive = EFalse;

    if(capsBuf().iMediaAtt & KMediaAttVariableSize)
    {//-- this is a RAM drive
		UserSvr::UnlockRamDrive();
        iRamDrive = ETrue;
	}

	if(aForceMount)
	{//-- the state is "forcedly mounted", special case. This is an inconsistent state.
        SetState(EInit_Forced);  
    	return;
    }

    //-- read and validate boot sector (sector 0)
    TFatBootSector bootSector;
    User::LeaveIfError(ReadBootSector(bootSector, iRamDrive));

    //-- print out boot sector debug information
    bootSector.PrintDebugInfo();

	//-- determine FAT type by data from boot sector. This is done by counting number of clusters, not by BPB_RootEntCnt
    iFatType=bootSector.FatType();
    ASSERT(iFatType != EInvalid); //-- this shall be checked in ReadBootSector()

    //-- values from the boot sector are checked in TFatBootSector::IsValid()
    //-- store volume UID, it can be checked on Remount
    iUniqueID = bootSector.UniqueID();

    //-- populate volume parameters with the values from boot sector. They had been validated in TFatBootSector::IsValid()
    iVolParam.Populate(bootSector); 

    //-- initialize the volume
    InitializeL(capsBuf());
    ASSERT(State()==EInit_R);

    GetVolumeLabelFromDiskL(bootSector);

	__PRINT2(_L("CFatMountCB::MountL() Completed, drv: %d, state:%d"), DriveNumber(), State());
	}



//-------------------------------------------------------------------------------------------------------------------

/**
Initialize the FAT cache and disk access

@param  aLocDrvCaps local drive capabilities
@leave KErrNoMemory,KErrNotReady,KErrCorrupt,KErrUnknown.
*/
void CFatMountCB::InitializeL(const TLocalDriveCaps& aLocDrvCaps, TBool /*aIgnoreFSInfo=EFalse*/)
	{
    __PRINT1(_L("CFatMountCB::InitializeL() drv:%d"), DriveNumber());

    ASSERT(State() == EMounting); //-- we must get here only from MountL()

    //========== Find out number of clusters on the volume
	if(iRamDrive && SectorsPerCluster()!=1)
		{// Align iFirstFreeByte to cluster boundary if internal ram drive
		const TInt sectorsPerClusterLog2=ClusterSizeLog2()-SectorSizeLog2();
		const TInt rootDirEndSector=RootDirEnd()>>SectorSizeLog2();
		const TInt alignedSector=((rootDirEndSector+SectorsPerCluster()-1)>>sectorsPerClusterLog2)<<sectorsPerClusterLog2;
		iFirstFreeByte=alignedSector<<SectorSizeLog2();
		}
	else
		{
		iFirstFreeByte=RootDirEnd();
		}


	    {//-- check if volume geometry looks valid
	    const TInt usableSectors=TotalSectors()-(iFirstFreeByte>>SectorSizeLog2());
	    iUsableClusters=usableSectors>>(ClusterSizeLog2()-SectorSizeLog2());

        const TUint32 KMinClusters = 32; //-- absolute minimum number of clusters on the volume
        const TUint32 KMaxClusters =(TotalSectors()-FirstFatSector()-NumberOfFats()*(FatSizeInBytes()>>SectorSizeLog2())) >> (ClusterSizeLog2()-SectorSizeLog2());
        
        if(usableSectors <=0 || iUsableClusters < KMinClusters || iUsableClusters > KMaxClusters)
            {
            __PRINT(_L("CFatMountCB::InitializeL() Wrong number of usable cluster/sectors on the volume!"));
            User::Leave(KErrCorrupt);
            }
        }


	//========== initialise RawDisk interface
	//-- CFatMountCB parameters might have changed, e.g. after formatting. Reconstruct directory cache with new parameters
	
    delete iRawDisk;
	iRawDisk=CRawDisk::NewL(*this, aLocDrvCaps);
    iRawDisk->InitializeL();
    

    //========== create FAT table object
	delete iFatTable;
	iFatTable=CFatTable::NewL(*this, aLocDrvCaps);

    //========== create and setup leaf direcotry cache if cache limit is set bigger than one 
	const TUint32 cacheLimit = iFatConfig.LeafDirCacheSize();
	if (cacheLimit > 1)
		{
		// destroy the old leaf dir cache to avoid memory leak.
		delete iLeafDirCache;
		iLeafDirCache = CLeafDirCache::NewL(cacheLimit);
		}
	else
		{
		iLeafDirCache = NULL;
		}

    //==========  find out free clusters number on the volume
    FAT().CountFreeClustersL();
    
    SetState(EInit_R);  //-- the state is "Initialized, but not writen"

    //-- make a callback, telling FileServer about free space discovered.
    const TInt64 freeSpace = ((TInt64)FAT().NumberOfFreeClusters()) << ClusterSizeLog2();
    SetDiskSpaceChange(freeSpace);

    __PRINT3(_L("#- CFatMountCB::InitializeL() done. drv:%d, Free clusters:%d, 1st Free cluster:%d"),DriveNumber(), FAT().NumberOfFreeClusters(), FAT().FreeClusterHint());
	}

//-------------------------------------------------------------------------------------------------------------------



/**
Checks for end of file for all Fat types

@param aCluster Cluster to check
@return Result of test
*/
TBool CFatMountCB::IsEndOfClusterCh(TInt aCluster) const
	{
	if(Is16BitFat())
		return(aCluster>=0xFFF8 && aCluster<=0xFFFF);
	else
		return(aCluster>=0xFF8 && aCluster<=0xFFF);
	}

/**
Set a cluster to the end of cluster chain marker

@param aCluster cluster to set to end of chain marker
*/
void CFatMountCB::SetEndOfClusterCh(TInt &aCluster) const
	{
	if(Is16BitFat())
		aCluster=0xFFF8;
	else
		aCluster=0xFF8;
	}

/**
Initialize data to represent the root directory

@param anEntry Entry to initialise
*/
void CFatMountCB::InitializeRootEntry(TFatDirEntry & anEntry) const
	{
	anEntry.SetName(_L8("ROOT"));
	anEntry.SetAttributes(KEntryAttDir);
	anEntry.SetStartCluster(0);		
	}


/**
Implementation of CMountCB::FileSystemSubType(). Retrieves the sub type of Fat file system
and returns the name as a descriptor.

@param aName Name of the sub type of Fat file system
@return KErrNone if successful; KErrArgument if aName is not long enough; KErrNotReady if
		the mount is not ready.

@see CMountCB::FileSystemSubType()
*/
TInt CFatMountCB::SubType(TDes& aName) const
	{
	if(aName.MaxLength() < 5)
		return KErrArgument;
	
	switch (iFatType)
		{
		case EFat12:
			{
			aName = KFSSubType_FAT12;
			return KErrNone;
			}
		case EFat16:
			{
			aName = KFSSubType_FAT16;
			return KErrNone;
			}
		default:
		// case EInvalidFatType
			return KErrNotReady;
		}
	}

//-------------------------------------------------------------------------------------------------------------------
/**
    CFatMountCB control method.
    @param  aLevel  specifies the operation to perfrom on the mount
    @param  aOption specific option for the given operation
    @param  aParam  pointer to generic parameter, its meaning depends on aLevel and aOption

    @return standard error code.
*/
TInt CFatMountCB::MountControl(TInt aLevel, TInt aOption, TAny* aParam)
    {
    TInt nRes = KErrNotSupported; 

    if(aLevel == ECheckFsMountable)
        {
        return MntCtl_DoCheckFileSystemMountable();
        }
    
    //-- mount state query: check if is in finalised state 
    if(aLevel == EMountStateQuery && aOption == ESQ_IsMountFinalised)
        {
        TBool bFinalised;
        nRes = IsFinalised(bFinalised);
        if(nRes == KErrNone)
            {
            *((TBool*)aParam) = bFinalised;
            }
        
        return nRes;
        }

    //-- File System - specific queries 
    if(aLevel == EMountFsParamQuery && aOption == ESQ_GetMaxSupportedFileSize)
        {//-- this is a query to provide the max. supported file size; aParam is a pointer to TUint64 to return the value
        *(TUint64*)aParam = KMaxSupportedFatFileSize;    
        return KErrNone;
        }


    return nRes;
    }

//-------------------------------------------------------------------------------------------------------------------
/**
Reports whether the specified interface is supported - if it is,
the supplied interface object is modified to it

@param aInterfaceId     The interface of interest
@param aInterface       The interface object
@return                 KErrNone if the interface is supported, otherwise KErrNotFound 

@see CMountCB::GetInterface()
*/
TInt CFatMountCB::GetInterface(TInt aInterfaceId, TAny*& aInterface,TAny* aInput)
    {
    switch(aInterfaceId)
        {
        case (CMountCB::EFileAccessor):
            ((CMountCB::MFileAccessor*&) aInterface) = this;
            return KErrNone;
        
        case (CMountCB::EGetFileSystemSubType):
            aInterface = (MFileSystemSubType*) (this);
            return KErrNone;

        case (CMountCB::EGetClusterSize):
            aInterface = (MFileSystemClusterSize*) (this);
            return KErrNone;

        case CMountCB::ELocalBufferSupport:
            // RAM drives doesn't support local buffers (this results in file caching being disabled)
            if (iRamDrive) 
                return KErrNotSupported;
            else
                return LocalDrive()->LocalBufferSupport();

		case EGetProxyDrive:
			((CProxyDrive*&)aInterface) = LocalDrive();
			return KErrNone;
        
        default:
            return(CMountCB::GetInterface(aInterfaceId, aInterface, aInput));
        }
    }

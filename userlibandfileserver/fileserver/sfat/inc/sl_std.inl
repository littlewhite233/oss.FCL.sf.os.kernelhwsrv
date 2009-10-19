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
// f32\sfat\inc\sl_std.inl
// 
//

#ifndef SL_STD_INL
#define SL_STD_INL



TBool IsPowerOf2(TUint32 aVal)
    {
	if (aVal==0)
		return EFalse;

    return !(aVal & (aVal-1));
    }

TUint32 Pow2(TUint32 aVal)
    {
        //ASSERT(aVal<32);
        return 1 << aVal;
    }


//---------------------------------------------------------------------------------------------------------------------------------
// class TEntryPos
TUint32 TEntryPos::Cluster() const 
    {
    return (TUint32) iCluster;
    }

TUint32 TEntryPos::Pos() const 
    {
    return (TUint32) iPos;
    }

TBool TEntryPos::operator==(const TEntryPos& aRhs) const
    {
    ASSERT(this != &aRhs);
    return (iCluster == aRhs.iCluster && iPos == aRhs.iPos);
    }


//---------------------------------------------------------------------------------------------------------------------------------
// class CFatMountCB

inline TInt CFatMountCB::RootDirectorySector() const
    {return iVolParam.RootDirectorySector();}

inline TUint CFatMountCB::RootDirEnd() const
    {return iVolParam.RootDirEnd();}

inline TUint32 CFatMountCB::RootClusterNum() const
    {return iVolParam.RootClusterNum(); }        


inline TInt CFatMountCB::StartCluster(const TFatDirEntry & anEntry) const
	{return anEntry.StartCluster();}



/**
returns true for root dir on Fat12/16 (fixed root dir versions of Fat). 
this function is used to handle special cases for reading/writing the root directory on FAT via the use of cluster zero.

@param aEntry position on volume being queried
@return Whether Root dir position or not
*/
TBool CFatMountCB::IsRootDir(const TEntryPos &aEntry) const
	{return((aEntry.iCluster==0) ? (TBool)ETrue : (TBool)EFalse);}

/**
Indicates the root directory cluster, For Fat12/16 root is always indicated by cluster number zero
@return The root cluster indicator
*/
TInt CFatMountCB::RootIndicator() const
	{return 0;}

   
TBool CFatMountCB::Is16BitFat() const
	{return(iFatType == EFat16);}

/** @return Log2 of cluster size on volume */
TInt CFatMountCB::ClusterSizeLog2() const
    {return(iVolParam.ClusterSizeLog2());}

/** @return Log2 of media sector size  */
TInt CFatMountCB::SectorSizeLog2() const
    {return(iVolParam.SectorSizeLog2());}

/** @return sector per cluster */
TInt CFatMountCB::SectorsPerCluster() const
    {return(1<<(iVolParam.ClusterSizeLog2()-iVolParam.SectorSizeLog2()));}

/** @return the base position of a cluster */
TInt CFatMountCB::ClusterBasePosition() const
	{return(iFirstFreeByte);}

/** @return the offset into a cluster of a byte address */
TInt CFatMountCB::ClusterRelativePos(TInt aPos) const
	{return(aPos&((1<<ClusterSizeLog2())-1));}

/**
Calculates the maximum number of clusters
@return  maximum number of clusters
*/
TUint32 CFatMountCB::MaxClusterNumber() const
    {return(TotalSectors()>>(ClusterSizeLog2()-SectorSizeLog2()));}

/** @return the the total sectors on volume */
TInt CFatMountCB::TotalSectors() const
    {return iVolParam.TotalSectors();}

/** @return total size of a Fat in bytes */
TInt CFatMountCB::FatSizeInBytes() const
    {return iVolParam.FatSizeInBytes();}

/** @return first sector of the Fat */
TUint32 CFatMountCB::FirstFatSector() const
    {return iVolParam.FirstFatSector();}

/** @return the byte offset of the Fat */
TInt CFatMountCB::StartOfFatInBytes() const
	{return(FirstFatSector()<<SectorSizeLog2());}

/** @return Number of Fats used by the volume */
TInt CFatMountCB::NumberOfFats() const
    {return iVolParam.NumberOfFats();}


/** @return refrence to the fat table owned by the mount */
CFatTable& CFatMountCB::FAT() const
	{return(*iFatTable);}
/**
Returns a refrence to the file system object owned by the coresponding TDrive for a drive used for creating subsession objects
@return refrence to file system object
*/
CFatFileSystem& CFatMountCB::FatFileSystem() const
	{return((CFatFileSystem&)Drive().FSys());}


/** @return  refrence to a raw disk object owned by the mount */
CRawDisk& CFatMountCB::RawDisk() const
	{return(*iRawDisk);}

/**
@return ETrue if aCluster value is bad cluster marker defined in FAT specification
*/
TBool CFatMountCB::IsBadCluster(TInt aCluster) const
	{return Is16BitFat() ? aCluster==0xFFF7 : aCluster==0xFF7;}

/**
Returns whether the current mount is running as rugged Fat or not, this is held in the file system object
@return Is rugged fat flag
*/
TBool CFatMountCB::IsRuggedFSys() const
	{return Drive().IsRugged();}

/**
Sets the rugged flag in the file system object
@param Flag to set or clear the rugged flag
*/
void CFatMountCB::SetRuggedFSys(TBool aVal)
	{Drive().SetRugged(aVal);}

/** @return the usable clusters count for a volume */
TUint32 CFatMountCB::UsableClusters() const
	{return(iUsableClusters);}


TUint CFatMountCB::StartOfRootDirInBytes() const
    {return iVolParam.RootDirectorySector()<<SectorSizeLog2();}


/** @return FAT type for this mount */
TFatType CFatMountCB::FatType() const
{
    return iFatType;
}

CAsyncNotifier* CFatMountCB::Notifier() const
	{return iNotifier;}	


/**
    Set or reset Read Only mode for the mount.
    @param    aReadOnlyMode if ETrue, the mount will be set RO.
*/
void  CFatMountCB::SetReadOnly(TBool aReadOnlyMode) 
{
    iReadOnly = aReadOnlyMode;
}

    
/**
    @return ETrue if the volume is in Read-Only state
*/
TBool CFatMountCB::ReadOnly(void) const
	{
    return iReadOnly;
	}

/** @return state of the CFatMountCB*/
CFatMountCB::TFatMntState CFatMountCB::State() const 
    {
    return iState;
    }

/** 
    Set state of the CFatMountCB
    @param  aState state to set
*/
void CFatMountCB::SetState(TFatMntState aState)
    {
    //__PRINT3(_L("#- CFatMountCB::SetState() drv:%d, %d->%d\n"),DriveNumber(),iState,aState);
    iState = aState;
    }


TFatDriveInterface& CFatMountCB::DriveInterface() const 
    {
    return (TFatDriveInterface&)iDriverInterface; 
    }

const TFatConfig& CFatMountCB::FatConfig() const 
    {
    return iFatConfig;
    }

//---------------------------------------------------------------------------------------------------------------------------------
/** 
Check if the XFileCreationHelper object is initialised.
*/
TBool CFatMountCB::XFileCreationHelper::IsInitialised() const 
	{
	return isInitialised;
	}

/** 
Get number of new entries for file creation.
*/
TUint16	CFatMountCB::XFileCreationHelper::NumOfAddingEntries() const
	{
	ASSERT(isInitialised); 
	return iNumOfAddingEntries;
	}

/** 
Get position of new entries for file creation.
*/
TEntryPos CFatMountCB::XFileCreationHelper::EntryAddingPos() const 
	{
	ASSERT(isInitialised); 
	return iEntryAddingPos;
	}

/** 
Check if position of new entries has been found.
*/
TBool CFatMountCB::XFileCreationHelper::IsNewEntryPosFound() const 
	{
	ASSERT(isInitialised); 
	return isNewEntryPosFound;
	}

/** 
Check if file name of the new file is a legal dos name.
*/
TBool CFatMountCB::XFileCreationHelper::IsTrgNameLegalDosName() const
	{
	ASSERT(isInitialised); 
	return isTrgNameLegalDosName;
	}

/** 
Set entry position for new entries to be added.
*/
void CFatMountCB::XFileCreationHelper::SetEntryAddingPos(const TEntryPos& aEntryPos) 
	{
	iEntryAddingPos = aEntryPos;
	}

/** 
Set condition if position of new entries has been found.
*/
void CFatMountCB::XFileCreationHelper::SetIsNewEntryPosFound(TBool aFound) 
	{
	isNewEntryPosFound = aFound;
	}



//-------  debug methods
#ifdef  _DEBUG
/**
Debug function indicates whether write fails are active or not, for test
@return ETrue if write fails on or not
*/
TBool CFatMountCB::IsWriteFail()const
	{return(iIsWriteFail);}
/**
Switches write fails on or off, for test
@param aIsWriteFail set true or false to set write fails on or off
*/
void CFatMountCB::SetWriteFail(TBool aIsWriteFail)
	{iIsWriteFail=aIsWriteFail;}

/** @return number of write fails to occur, for test */
TInt CFatMountCB::WriteFailCount()const
	{return(iWriteFailCount);}

/**
Set the number of Write fails 
@param aFailCount number of write fails, for test
*/
void CFatMountCB::SetWriteFailCount(TInt aFailCount)
	{iWriteFailCount=aFailCount;}

/** Decrement the number of write fails, for test */
void CFatMountCB::DecWriteFailCount()
	{--iWriteFailCount;}

/** @return Error for a write failure, for test */
TInt CFatMountCB::WriteFailError()const
	{return iWriteFailError;}

/**
Set the write fail error code, for test
@param aErrorValue The Error for a write fails
*/
void CFatMountCB::SetWriteFailError(TInt aErrorValue)
	{iWriteFailError=aErrorValue;}

#endif


//---------------------------------------------------------------------------------------------------------------------------------
// class CFatFormatCB

TBool CFatFormatCB::Is16BitFat() const
    {return(iFileSystemName==KFileSystemName16);}

CFatMountCB& CFatFormatCB::FatMount()
	{return *(CFatMountCB*)&Mount();}

/**
Returns the local drive used by the file systems from the owning mount
@return Pointer to the local drive 
*/
CProxyDrive* CFatFormatCB::LocalDrive()
	{return(FatMount().LocalDrive());}


//---------------------------------------------------------------------------------------------------------------------------------
// class CFatFileCB

/**
Returns the owning mount from file object

@return pointer to the owning mount object
*/
CFatMountCB& CFatFileCB::FatMount() const
	{return((CFatMountCB&)Mount());}

/**
Returns the fat table used by the file system for this mount

@return Refrence to the Fat table owned by the mount
*/
CFatTable& CFatFileCB::FAT()
	{return(FatMount().FAT());}

/**
Position with in a cluster for a given address

@param aPos Byte position 
*/
TInt CFatFileCB::ClusterRelativePos(TInt aPos)
	{return(FatMount().ClusterRelativePos(aPos));}
/**
Returns Log2 of cluster size from mount

@return cluster size
*/
TInt CFatFileCB::ClusterSizeLog2()
	{return(FatMount().ClusterSizeLog2());}

/*
 Note: this replaces SeekIndex() which was only used in sl_mnt
 to verify whether the seek index had been created/initialised
*/
inline TBool CFatFileCB::IsSeekIndex() const
{return (iSeekIndex==NULL?(TBool)EFalse:(TBool)ETrue); }


//---------------------------------------------------------------------------------------------------------------------------------
// class CFatDirCB

/**
Returns the owning mount from directory object

@return pointer to the owning mount object
*/
CFatMountCB& CFatDirCB::FatMount()
	{return((CFatMountCB&)Mount());}



//---------------------------------------------------------------------------------------------------------------------------------
// class CFatTable

TUint32 CFatTable::FreeClusters() const 
    {
    return iFreeClusters;
    }


TBool CFatTable::IsEof16Bit(TInt aCluster) const
	{return(aCluster>=0xFFF8 && aCluster<=0xFFFF);}

TBool CFatTable::IsEof12Bit(TInt aCluster) const
	{return(aCluster>=0xFF8 && aCluster<=0xFFF);}

TInt CFatTable::SectorSizeLog2() const
	{return(iOwner->SectorSizeLog2());}

//---------------------------------------------------------------------------------------------------------------------------------

inline TBool CFatTable::IsFat12() const 
    {
    return iFatType == EFat12;
    }

inline TBool CFatTable::IsFat16() const 
    {
    return iFatType == EFat16;
    }

inline TFatType CFatTable::FatType() const 
    {
    return iFatType;
    }
/**
@return Maximal number of addresable FAT entries. This value is taken from the owning mount
*/
inline TUint32 CFatTable::MaxEntries() const
    {
        ASSERT(iMaxEntries > 0);
        return iMaxEntries;
    }


// class TFatDriveInterface
TBool TFatDriveInterface::NotifyUser() const
	{return(iMount->GetNotifyUser());}



//----------------------------------------------------------------------------------------------------
// class CRawDisk

/**
    Get pointer to the directory cache interface. Any client that reads/writes directory entries 
    MUST do it via this interface.
    Default implementation returns NULL

    @return     pointer to the MWTCacheInterface interface, or NULL if it is not present.
*/
MWTCacheInterface* CRawDisk::DirCacheInterface()
    {
    return NULL;
    }

//---------------------------------------------------------------------------------------------------------------------------------	
//-- class RBitVector

/** @return size of the vector (number of bits) */
inline TUint32 RBitVector::Size() const
    {
    return iNumBits;
    } 

/**
    Get a bit by index
    
    @param aIndex  index in a bit vector
    @return 0 if the bit at pos aIndex is 0, not zero otherwise
    @panic EIndexOutOfRange if aIndex is out of range
*/
inline TBool RBitVector::operator[](TUint32 aIndex) const
    {
    __ASSERT_ALWAYS(aIndex < iNumBits, Panic(EIndexOutOfRange));
    return (ipData[WordNum(aIndex)] & (1<<BitInWord(aIndex)));
    }

/**
    Set a bit at pos aIndex to '1'
    @param aIndex  index in a bit vector
    @panic EIndexOutOfRange if aIndex is out of range
*/
inline void RBitVector::SetBit(TUint32 aIndex)
    {
    __ASSERT_ALWAYS(aIndex < iNumBits, Panic(EIndexOutOfRange));
    ipData[WordNum(aIndex)] |= (1<<BitInWord(aIndex));
    }

/**
    Set a bit at pos aIndex to '0'
    @param aIndex  index in a bit vector
    @panic EIndexOutOfRange if aIndex is out of range
*/
inline void RBitVector::ResetBit(TUint32 aIndex)
    {
    __ASSERT_ALWAYS(aIndex < iNumBits, Panic(EIndexOutOfRange));
    ipData[WordNum(aIndex)] &= ~(1<<BitInWord(aIndex));
    }

/**
    Invert a bit at pos aIndex
    @param aIndex  index in a bit vector
    @panic EIndexOutOfRange if aIndex is out of range
*/
inline void RBitVector::InvertBit(TUint32 aIndex)
    {
    __ASSERT_ALWAYS(aIndex < iNumBits, Panic(EIndexOutOfRange));
    ipData[WordNum(aIndex)] ^= (1<<BitInWord(aIndex));
    }

/**
    Set bit value at position aIndex
    @param aIndex  index in a bit vector
    @panic EIndexOutOfRange if aIndex is out of range
*/
inline void RBitVector::SetBitVal(TUint32 aIndex, TBool aVal)
    {
    if(aVal) 
        SetBit(aIndex);
    else 
        ResetBit(aIndex);
    }


inline TUint32 RBitVector::MaskLastWord(TUint32 aVal) const
    {
    const TUint32 shift = (32-(iNumBits & 0x1F)) & 0x1F;
    return (aVal << shift) >> shift; //-- mask unused high bits
    }



inline TUint32 RBitVector::WordNum(TUint32 aBitPos)  const
    {
    return aBitPos >> 5;
    }

inline TUint32 RBitVector::BitInWord(TUint32 aBitPos) const 
    {
    return aBitPos & 0x1F;
    }

/**
    Calculate offset of the page starting position in the cluster 
    @param aPos  the current entry position in bytes in the cluster
    @param aPageSzLog2	page size in log2
    @return		the starting position of the page that contains aPos
*/
inline TUint32 CalculatePageOffsetInCluster(TUint32 aPos, TUint aPageSzLog2)
	{
	return (aPos >> aPageSzLog2) << aPageSzLog2;
	}

#endif //SL_STD_INL





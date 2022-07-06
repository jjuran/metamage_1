/*
	vcbs.cc
	-------
*/

// Mac OS
#ifndef __APPLE__
#ifndef __FILES__
#include <Files.h>
#endif
#endif

// Standard C
#include <stdio.h>

// iota
#include "iota/char_types.hh"

// chars
#include "conv/mac_utf8.hh"


int main( int argc, char** argv )
{
#if ! TARGET_API_MAC_CARBON
	
	const QHdr& VCBQHdr = *(QHdr*) 0x0356;
	
	VCB* vcb = (VCB*) VCBQHdr.qHead;
	
	while ( vcb != NULL )
	{
		unsigned char* p = vcb->vcbVN;
		
		unsigned len = *p++;
		
		char utf[ 27 * 3 + 1 ];  // worst case
		
		size_t n = conv::utf8_from_mac( utf, sizeof utf, (char*) p, len );
		
		utf[ n ] = '\0';
		
		const uint16_t sig  = vcb->vcbSigWord;
		const uint16_t fsid = vcb->vcbFSID;
		
		printf( "Name:     %s\n",   utf              );
		printf( "DrvNum:   %d\n",   vcb->vcbDrvNum   );
		printf( "DRefNum:  %d\n",   vcb->vcbDRefNum  );
		printf( "VRefNum:  %d\n",   vcb->vcbVRefNum  );
		printf( "FSID:     %x ('%c%c')\n",
		        fsid,
		        iota::is_print( fsid >> 8 ) ? fsid >> 8 : '.',
		        iota::is_print( fsid      ) ? fsid      : '.' );
		printf( "SigWord:  %x ('%c%c')\n",
		        sig,
		        iota::is_print( sig >> 8 ) ? sig >> 8 : '.',
		        iota::is_print( sig      ) ? sig      : '.' );
		
		printf( "Flags:    %.4x\n", vcb->vcbFlags    );
		printf( "Created:  %lu\n",  vcb->vcbCrDate   );
		printf( "Updated:  %lu\n",  vcb->vcbLsMod    );
		printf( "Attrs:    %.4x\n", vcb->vcbAtrb     );
		printf( "NmFls:    %u\n",   vcb->vcbNmFls    );
		printf( "VBMSt:    %d\n",   vcb->vcbVBMSt    );
		printf( "AllocPtr: %d\n",   vcb->vcbAllocPtr );
		printf( "NmAlBlks: %u\n",   vcb->vcbNmAlBlks );
		printf( "AlBlkSiz: %ld\n",  vcb->vcbAlBlkSiz );
		printf( "ClpSiz:   %ld\n",  vcb->vcbClpSiz   );
		printf( "AlBlSt:   %d\n",   vcb->vcbAlBlSt   );
		printf( "NxtCNID:  %ld\n",  vcb->vcbNxtCNID  );
		printf( "FreeBks:  %u\n",   vcb->vcbFreeBks  );
		// vcbVN
		// vcbDrvNum
		// vcbDRefNum
		// vcbFSID
		// vcbVRefNum
		printf( "MAdr:     %p\n",   vcb->vcbMAdr     );
		printf( "BufAdr:   %p\n",   vcb->vcbBufAdr   );
		printf( "MLen:     %d\n",   vcb->vcbMLen     );
		printf( "DirIndex: %d\n",   vcb->vcbDirIndex );
		printf( "DirBlk:   %d\n",   vcb->vcbDirBlk   );
		printf( "VolBkUp:  %lu\n",  vcb->vcbVolBkUp  );
		printf( "VSeqNum:  %u\n",   vcb->vcbVSeqNum  );
		printf( "WrCnt:    %ld\n",  vcb->vcbWrCnt    );
		printf( "XTClpSiz: %ld\n",  vcb->vcbXTClpSiz );
		printf( "CTClpSiz: %ld\n",  vcb->vcbCTClpSiz );
		printf( "NmRtDirs: %u\n",   vcb->vcbNmRtDirs );
		printf( "FilCnt:   %ld\n",  vcb->vcbFilCnt   );
		printf( "DirCnt:   %ld\n",  vcb->vcbDirCnt   );
		// vcbFndrInfo
		printf( "VCSize:   %u\n",   vcb->vcbVCSize   );
		printf( "VBMCSiz:  %u\n",   vcb->vcbVBMCSiz  );
		printf( "CtlCSiz:  %u\n",   vcb->vcbCtlCSiz  );
		printf( "XTAlBlks: %u\n",   vcb->vcbXTAlBlks );
		printf( "CTAlBlks: %u\n",   vcb->vcbCTAlBlks );
		printf( "XTRef:    %d\n",   vcb->vcbXTRef    );
		printf( "CTRef:    %d\n",   vcb->vcbCTRef    );
		printf( "CtlBuf:   %p\n",   vcb->vcbCtlBuf   );
		printf( "DirIDM:   %ld\n",  vcb->vcbDirIDM   );
		printf( "OffsM:    %d\n",   vcb->vcbOffsM    );
		
		printf( "\n" );
		
		vcb = (VCB*) vcb->qLink;
	}
	
#endif
	
	return 0;
}

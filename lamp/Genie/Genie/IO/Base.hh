/*	=======
 *	Base.hh
 *	=======
 */

#ifndef GENIE_IO_BASE_HH
#define GENIE_IO_BASE_HH

// boost
#include <boost/enable_shared_from_this.hpp>

// Genie
#include "Genie/FileSystem/FSTree.hh"


namespace Genie
{
	
	enum
	{
		kPollRead   = 1,
		kPollWrite  = 2,
		kPollExcept = 4
	};
	
	/*
		Each file descriptor refers to an I/O handle.
		Several file descriptors may share the same I/O handle.
		Each I/O handle has a type.
		I/O handle types are arranged in an inheritence tree, with the base type
		at the root node, and actual (non-abstract) types as leaf nodes.
		Each type (abstract as well as leaf) has a node in the tree.
		
		The game here is to assign each node a positive integer value,
		with these conditions:
		
			* each value must be unique
			* the value for a node must be a multiple of its parent node's value
			  (and transitively, each of its ancestor nodes' values)
			* the value for a node must not be a multiple of any non-ancestor
			* the mapping must not be dependent on limiting the size of the tree
			  in any way, so new nodes may be added without renumbering old ones
		
		Our solution involves prime factors.
		
		First, we define a prime's index as its (1-based) index in the set of
		prime numbers sorted least to greatest.  So index(2) == 1, index(3) == 2,
		and index(5) == 3.
		
		Next, we define the order of a prime as follows:  For convenience, the
		order of a non-prime is zero.  The order of a prime number is equal to
		1 + the order of its index.  We divide the set of prime numbers into
		first-order primes, second-order primes, and so on.
		
		Index	Prime	o(i)	Order
		1		2		0		1
		2		3		1		2
		3		5		2		3
		4		7		0		1
		5		11		3		4
		6		13		0		1
		7		17		1		2
		
		Each node has a depth, which is zero for the root node, one for the root
		node's child nodes, two for their children, etc.  Each group of child
		nodes is tagged with a prime number selected from the set of prime
		numbers whose order equals the depth of the child nodes.  For example,
		the immediate children of the root node (depth of one) are tagged with
		first-order primes:  2, 7, 13, etc.  Each of their children are tagged
		with second-order primes, reusing the set for each group of children.
		
		The value of each child node equals its tag times the parent's value.
		The root's first child node has value 2 and children of value 6 and 34,
		and the second root child has value 7 and children of value 21 and 119.
		
		Root = 1
		
			1	2	3	4		5		6
			-	-	-	-		-		-
		1	2	3	5	11		31		127
		2	7	17	59	277		1787	15299
		3	13	41	179	1063	8527
		4	19	67	331	2221	19577
		5	23	83	431	3001	27457
		6	29	109	599	4397
		
	*/
	
	enum TypeCode
	{
		kBaseType = 1,
			kStreamType    = 2,
				kFileType   = kStreamType * 3,  // 6
					kRegularFileType = kFileType * 5,  // 30
						kMacFileType   = kRegularFileType * 11,  // 330
							kMacDataForkType = kMacFileType * 31,  // 10230
							kMacRsrcForkType = kMacFileType * 1787,  // 589710
						kQueryFileType = kRegularFileType * 277,  // 8310
					kDeviceFileType  = kFileType * 59,  // 295
						kSimpleDeviceType = kDeviceFileType * 11,  // 649
						kSerialDeviceType = kDeviceFileType * 277,  // 16343
						kTTYType          = kDeviceFileType * 1063,  // 313585
							kConsoleTTYType = kTTYType * 31,  // 9721135
							kPseudoTTYType  = kTTYType * 1787,  // 560376395
				kPipeType   = kStreamType * 17,  // 34
				kSocketType = kStreamType * 41,  // 82
			kResFileType   = 7,
			kDirectoryType = 13,
			kWindowType    = 19,
			kFooType       = 23,
			kBarType       = 29
	};
	
	class IOHandle : public boost::enable_shared_from_this< IOHandle >
	{
		private:
			virtual IOHandle* Next() const  { return NULL; }
		
		public:
			virtual ~IOHandle();
			
			static TypeCode Type()  { return kBaseType; }
			
			virtual TypeCode ActualType() const  { return Type(); }
			
			IOHandle* GetBaseForCast( TypeCode desiredType );
			
			virtual FSTreePtr GetFile() const;
			
			virtual void IOCtl( unsigned long request, int* argp );
			
	};
	
	void Check_IOHandle_Cast_Result( IOHandle* cast );
	
	template < class Handle >
	Handle* IOHandle_Cast( IOHandle* handle )
	{
		if ( handle == NULL )
		{
			return NULL;
		}
		
		IOHandle* base = handle->GetBaseForCast( Handle::Type() );
		
		return static_cast< Handle* >( base );
	}
	
	template < class Handle >
	Handle& IOHandle_Cast( IOHandle& handle )
	{
		Handle* cast = IOHandle_Cast< Handle >( &handle );
		
		Check_IOHandle_Cast_Result( cast );
		
		return *cast;
	}
	
}

#endif


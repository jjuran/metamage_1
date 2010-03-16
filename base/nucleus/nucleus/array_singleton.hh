// nucleus/array_singleton.hh
// --------------------------

// Part of the Nitrogen project.
//
// Written 2002-2003 by Lisa Lippincott.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NUCLEUS_ARRAYSINGLETON_HH
#define NUCLEUS_ARRAYSINGLETON_HH


namespace nucleus
{
	
	template < class Element >
	struct array_singleton_0
	{
		static const int size = 0;
		
		static const Element *array()
		{
			return 0;
		}
	};
	
	template < class Element, Element e0 >
	struct array_singleton_1
	{
		static const int size = 1;
		
		static const Element item0 = e0;
		
		static const Element *array()
		{
			static const Element the_array[ size ] =
			{
				item0
			};
			
			return the_array;
		}
	};

	template < class Element, Element e0, Element e1 >
	struct array_singleton_2
	{
		static const int size = 2;
		
		static const Element item0 = e0;
		static const Element item1 = e1;
		
		static const Element *array()
		{
			static const Element the_array[ size ] =
			{
				item0, item1
			};
			
			return the_array;
		}
	};

	template < class Element, Element e0, Element e1, Element e2 >
	struct array_singleton_3
	{
		static const int size = 3;
		
		static const Element item0 = e0;
		static const Element item1 = e1;
		static const Element item2 = e2;
		
		static const Element *array()
		{
			static const Element the_array[ size ] =
			{
				item0, item1, item2
			};
			
			return the_array;
		}
	};

	template < class Element, Element e0, Element e1, Element e2, Element e3 >
	struct array_singleton_4
	{
		static const int size = 4;
		
		static const Element item0 = e0;
		static const Element item1 = e1;
		static const Element item2 = e2;
		static const Element item3 = e3;
		
		static const Element *array()
		{
			static const Element the_array[ size ] =
			{
				item0, item1, item2, item3
			};
			
			return the_array;
		}
	};

	template < class Element, Element e0, Element e1, Element e2, Element e3, Element e4 >
	struct array_singleton_5
	{
		static const int size = 5;
		
		static const Element item0 = e0;
		static const Element item1 = e1;
		static const Element item2 = e2;
		static const Element item3 = e3;
		static const Element item4 = e4;
		
		static const Element *array()
		{
			static const Element the_array[ size ] =
			{
				item0, item1, item2, item3, item4
			};
			
			return the_array;
		}
	};

	template < class Element, Element e0, Element e1, Element e2, Element e3, Element e4, Element e5 >
	struct array_singleton_6
	{
		static const int size = 6;
		
		static const Element item0 = e0;
		static const Element item1 = e1;
		static const Element item2 = e2;
		static const Element item3 = e3;
		static const Element item4 = e4;
		static const Element item5 = e5;
		
		static const Element *array()
		{
			static const Element the_array[ size ] =
			{
				item0, item1, item2, item3, item4, item5
			};
			
			return the_array;
		}
	};

	template < class Element, Element e0, Element e1, Element e2, Element e3, Element e4, Element e5, Element e6 >
	struct array_singleton_7
	{
		static const int size = 7;
		
		static const Element item0 = e0;
		static const Element item1 = e1;
		static const Element item2 = e2;
		static const Element item3 = e3;
		static const Element item4 = e4;
		static const Element item5 = e5;
		static const Element item6 = e6;
		
		static const Element *array()
		{
			static const Element the_array[ size ] =
			{
				item0, item1, item2, item3, item4, item5, item6
			};
			
			return the_array;
		}
	};
	
	template < class Element, Element e0, Element e1, Element e2, Element e3, Element e4, Element e5, Element e6, Element e7 >
	struct array_singleton_8
	{
		static const int size = 8;
		
		static const Element item0 = e0;
		static const Element item1 = e1;
		static const Element item2 = e2;
		static const Element item3 = e3;
		static const Element item4 = e4;
		static const Element item5 = e5;
		static const Element item6 = e6;
		static const Element item7 = e7;
		
		static const Element *array()
		{
			static const Element the_array[ size ] =
			{
				item0, item1, item2, item3, item4, item5, item6, item7
			};
			
			return the_array;
		}
	};
	
	
	template < class Element >
	struct array_singleton
	{
		template < Element e0 = 0, Element e1 = 0, Element e2 = 0, Element e3 = 0,
		           Element e4 = 0, Element e5 = 0, Element e6 = 0, Element e7 = 0 >
		struct array_type
		:
			array_singleton_8< Element, e0, e1, e2, e3, e4, e5, e6, e7 >
		{
		};
		
		template < Element e0, Element e1, Element e2, Element e3, Element e4, Element e5, Element e6 >
		struct array_type< e0, e1, e2, e3, e4, e5, e6, 0 >
		:
			array_singleton_7< Element, e0, e1, e2, e3, e4, e5, e6 >
		{
		};
		
		template < Element e0, Element e1, Element e2, Element e3, Element e4, Element e5 >
		struct array_type< e0, e1, e2, e3, e4, e5, 0, 0 >
		:
			array_singleton_6< Element, e0, e1, e2, e3, e4, e5 >
		{
		};
		
		template < Element e0, Element e1, Element e2, Element e3, Element e4 >
		struct array_type< e0, e1, e2, e3, e4, 0, 0, 0 >
		:
			array_singleton_5< Element, e0, e1, e2, e3, e4 >
		{
		};
		
		template < Element e0, Element e1, Element e2, Element e3 >
		struct array_type< e0, e1, e2, e3, 0, 0, 0, 0 >
		:
			array_singleton_4< Element, e0, e1, e2, e3 >
		{
		};
		
		template < Element e0, Element e1, Element e2 >
		struct array_type< e0, e1, e2, 0, 0, 0, 0, 0 >
		:
			array_singleton_3< Element, e0, e1, e2 >
		{
		};
		
		template < Element e0, Element e1 >
		struct array_type< e0, e1, 0, 0, 0, 0, 0, 0 >
		:
			array_singleton_2< Element, e0, e1 >
		{
		};
		
		template < Element e0 >
		struct array_type< e0, 0, 0, 0, 0, 0, 0, 0 >
		:
			array_singleton_1< Element, e0 >
		{
		};
	};
	
	#if 0
		
		// This double-specialization freaks out CodeWarrior 8.3.
		// There's a (single) specialization in CarbonEvents.h to avoid this case.
		template < class Element >
		template <>
		struct array_singleton< Element >::array_type< 0, 0, 0, 0, 0, 0, 0, 0 >
		:
			array_singleton_0< Element >
		{
		};
		
	#endif
}

#endif
